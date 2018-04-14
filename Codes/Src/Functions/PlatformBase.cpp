#include "PlatformBase.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Functions */
#include "Include/Functions/EnumHelperInterface.h"
#include "Include/Functions/TableIndexHelperInterface.h"

/* local header files */
#include "CurlOutputHandler.h"

using namespace std;
using namespace std::placeholders;

/**********************class PlatformBase**********************/
/* public functions */
PlatformBase::PlatformBase()
{
	marketTimeStamp = 0;
}

PlatformBase::~PlatformBase()
{
    /* 程序退出时, Platform 需要清除自己注册到 market poller 的回调函数. */
    for (auto i = marketPollers.begin(); i != marketPollers.end(); ++i)
    {
        i->second->Stop();
    }
    marketPollers.clear();
}

std::shared_ptr<PlatformEntity> PlatformBase::GetPlatformEntity()
{
    lock_guard<mutex> lock(ptmEntMtx);
    return ptmEnt;
}

void PlatformBase::SetPlatformEntity(std::shared_ptr<PlatformEntity> ptmEnt)
{
    lock_guard<mutex> lock(ptmEntMtx);
    if (this->ptmEnt != ptmEnt)
        this->ptmEnt = ptmEnt;
}

const std::string& PlatformBase::GetPlatformName() const
{
    return ptmEnt->GetName();
}

milliseconds PlatformBase::GetMinMarketQueryInterval()
{
    return ptmEnt->GetMinMarketQueryInvl();
}

void PlatformBase::SetMinMarketQueryInterval(milliseconds minMarketQueryInterval)
{
    ptmEnt->SetMinMarketQueryInvl(minMarketQueryInterval);
}

milliseconds PlatformBase::GetMinOrderQueryInterval()
{
    return ptmEnt->GetMinOrderQueryInvl();
}

void PlatformBase::SetMinOrderQueryInterval(milliseconds minOrderQueryInterval)
{
    ptmEnt->SetMinOrderQueryInvl(minOrderQueryInterval);
}

CoinNumber PlatformBase::GetCoinMinTradingUnit(CoinType coinType) const
{
    shared_ptr<PlatformCoinRuleEntity> ptmCoinRuleEnt = ptmEnt->GetCoinRule(coinType);
    return ptmCoinRuleEnt->GetMinTradingUnit();
}

bool PlatformBase::CreateOrder(std::shared_ptr<OrderEntity> orderEnt)
{
    {
        lock_guard<mutex> lock(ptmEntMtx);
        assert(ptmEnt->GetId() == orderEnt->GetPlatform()->GetId());
    }
    /* if the interval between 2 operations (for a single robot) less than milliseconds(1000),
    btctrade.com will reject the 2nd operation.
    all of operation is from robot's thread, so we use thread_id as key to record the time point
    at which we do the last operation.
    */
    shared_ptr<AccountEntity> accountEntity = orderEnt->GetAccount();
    shared_ptr<AccountInfo> accountInfo = GetAccountInfo(accountEntity->GetId());
    std::lock_guard<mutex> lock(accountInfo->tradeMtx);

    milliseconds diff = chrono::duration_cast<milliseconds>(system_clock::now() - accountInfo->lastTradeTimePoint);
    if (diff < orderEnt->GetPlatform()->GetMinTradeInvl())
    {
        this_thread::sleep_for(orderEnt->GetPlatform()->GetMinTradeInvl() - diff);
    }

    bool result;
    if (orderEnt->GetOrderType() == OrderType::Buy)
    {
        result = CreateBuyOrderImpl(orderEnt);
    }
    else
    {
        result = CreateSellOrderImpl(orderEnt);
    }
    accountInfo->lastTradeTimePoint = system_clock::now();

    return result;
}

void PlatformBase::CancelOrder(std::shared_ptr<OrderEntity> orderEnt)
{
    {
        lock_guard<mutex> lock(ptmEntMtx);
        assert(ptmEnt->GetId() == orderEnt->GetPlatform()->GetId());
    }
    assert(orderEnt->GetClosingTime() == nullptr);

    /* if the interval between 2 operations (for a single robot) less than milliseconds(1000),
    btctrade.com will reject the 2nd operation.
    all of operation is from robot's thread, so we use thread_id as key to record the time point
    at which we do the last operation.
    */
    shared_ptr<AccountEntity> accountEntity = orderEnt->GetAccount();
    shared_ptr<AccountInfo> accountInfo = GetAccountInfo(accountEntity->GetId());
    std::lock_guard<mutex> lock(accountInfo->tradeMtx);

    milliseconds diff = chrono::duration_cast<milliseconds>(system_clock::now() - accountInfo->lastTradeTimePoint);
    if (diff < orderEnt->GetPlatform()->GetMinTradeInvl())
    {
        this_thread::sleep_for(orderEnt->GetPlatform()->GetMinTradeInvl() - diff);
    }

    bool result = CancelOrderImpl(orderEnt);
	if (!result)
	{
		LOG4_WARN("failed, result=false");
	}
	if (orderEnt->GetClosingTime() == nullptr)
	{
		LOG4_WARN("failed, orderEnt->GetClosingTime() == nullptr");
	}
    assert(result && orderEnt->GetClosingTime() != nullptr);
    accountInfo->lastTradeTimePoint = system_clock::now();
}

void PlatformBase::FetchOrder(std::shared_ptr<OrderEntity> orderEnt)
{
    {
        lock_guard<mutex> lock(ptmEntMtx);
        assert(ptmEnt->GetId() == orderEnt->GetPlatform()->GetId());
    }

    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();
	LOG4_TRACE("Start, account id=" << accEnt->GetId() << ", platform order id=%d" << orderEnt->GetPlatformOrderId());

    //to avoid 2 account of the same account group call me at the same time,
    //that will cause "nonce expire error"
    shared_ptr<AccountInfo> accountInfo = GetAccountInfo(accEnt->GetId());
    std::unique_lock<mutex> lock(accountInfo->fetchMtx);

    milliseconds diff = chrono::duration_cast<milliseconds>(system_clock::now() - accountInfo->lastFetchTimePoint);
    if (diff < orderEnt->GetPlatform()->GetMinOrderQueryInvl())
    {
        this_thread::sleep_for(orderEnt->GetPlatform()->GetMinOrderQueryInvl() - diff);
    }

    bool result = FetchOrderImpl(orderEnt);
    assert(result);
    accountInfo->lastFetchTimePoint = system_clock::now();
}

void PlatformBase::FetchBalance(std::shared_ptr<AccountEntity> accEnt, Money& balance)
{
    {
        lock_guard<mutex> lock(ptmEntMtx);
        assert(ptmEnt->GetId() == accEnt->GetPlatform()->GetId());
    }
    LOG4_TRACE("Start, account id=" << accEnt->GetId());

    //to avoid 2 account of the same account group call me at the same time,
    //that will cause "nonce expire error"
    shared_ptr<AccountInfo> accountInfo = GetAccountInfo(accEnt->GetId());
    std::unique_lock<mutex> lock(accountInfo->fetchMtx);

    milliseconds diff = chrono::duration_cast<milliseconds>(system_clock::now() - accountInfo->lastFetchTimePoint);
    if (diff < accEnt->GetPlatform()->GetMinOrderQueryInvl())
    {
        this_thread::sleep_for(accEnt->GetPlatform()->GetMinOrderQueryInvl() - diff);
    }

    bool result = FetchBalanceImpl1(accEnt, balance);
    assert(result);
    accountInfo->lastFetchTimePoint = system_clock::now();
}

void PlatformBase::FetchBalance(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance)
{
    {
        lock_guard<mutex> lock(ptmEntMtx);
        assert(ptmEnt->GetId() == accEnt->GetPlatform()->GetId());
    }
    LOG4_TRACE("Start, account id=" << accEnt->GetId());

    //to avoid 2 account of the same account group call me at the same time,
    //that will cause "nonce expire error"
    shared_ptr<AccountInfo> accountInfo = GetAccountInfo(accEnt->GetId());
    std::unique_lock<mutex> lock(accountInfo->fetchMtx);

    milliseconds diff = chrono::duration_cast<milliseconds>(system_clock::now() - accountInfo->lastFetchTimePoint);
    if (diff < accEnt->GetPlatform()->GetMinOrderQueryInvl())
    {
        this_thread::sleep_for(accEnt->GetPlatform()->GetMinOrderQueryInvl() - diff);
    }

    bool result = FetchBalanceImpl2(accEnt, coinType, balance);
    assert(result);
    accountInfo->lastFetchTimePoint = system_clock::now();
}

bool PlatformBase::FetchUnknownOrder(std::shared_ptr<OrderEntity> orderEnt)
{
    {
        lock_guard<mutex> lock(ptmEntMtx);
        assert(ptmEnt->GetId() == orderEnt->GetPlatform()->GetId());
    }
    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();

	LOG4_TRACE("Start, account id=" << accEnt->GetId() << ", order id=" << orderEnt->GetId());

    //to avoid 2 account of the same account group call me at the same time,
    //that will cause "nonce expire error"
    shared_ptr<AccountInfo> accountInfo = GetAccountInfo(accEnt->GetId());
    std::unique_lock<mutex> lock(accountInfo->fetchMtx);

    milliseconds diff = chrono::duration_cast<milliseconds>(system_clock::now() - accountInfo->lastFetchTimePoint);
    if (diff < orderEnt->GetPlatform()->GetMinOrderQueryInvl())
    {
        this_thread::sleep_for(orderEnt->GetPlatform()->GetMinOrderQueryInvl() - diff);
    }

    bool result = FetchUnknownOrderImpl(orderEnt);
    accountInfo->lastFetchTimePoint = system_clock::now();
    return result;
}

void PlatformBase::RegisterMarketHandler(TableId robotId, CoinType coinType, MarketHandler handler)
{
	LOG4_TRACE("Start, robot id=" << robotId << ", coin type=" << GetEnumName(coinType));
    lock_guard<mutex> lock(handlersMtx);

    MarketPollerRepertory::iterator iter = marketPollers.find(coinType);
    if (iter == marketPollers.end())
    {
        /* register market handler */
        HttpMarketHandler marketHandler = bind(&PlatformBase::HandleMarketInfor, this, _1, _2, _3, _4);

        const char *ptmName = ptmEnt->GetName().c_str();
        milliseconds invl = ptmEnt->GetMinMarketQueryInvl();
        shared_ptr<MarketPollerInterface> marketPoller(CreateMarketPollerInterface(ptmName, invl, coinType, marketHandler));

        marketPoller->Start();
        marketPollers.insert(make_pair(coinType, marketPoller));
    }

    assert(marketHandlers.find(make_pair(robotId, coinType)) == marketHandlers.end());
    auto pair = marketHandlers.insert(make_pair(make_pair(robotId, coinType), handler));
    assert(pair.second);
}

void PlatformBase::DeregisterMarketHandler(TableId robotId, CoinType coinType)
{
	LOG4_TRACE("Start, robot id=" << robotId << ", coin type=" << GetEnumName(coinType));
    unique_lock<mutex> lock(handlersMtx);

    assert(marketHandlers.find(make_pair(robotId, coinType)) != marketHandlers.end());
    marketHandlers.erase(marketHandlers.find(make_pair(robotId, coinType)));

    auto cmp = [coinType](const std::pair<std::pair<TableId, CoinType>, MarketHandler>& iter)->bool
    {
        return (coinType == iter.first.second);
    };
    MarketHandlerRepertory::iterator iter = find_if(marketHandlers.begin(), marketHandlers.end(), cmp);
    if (iter == marketHandlers.end())
    {
        auto pair = marketPollers.find(coinType);
        assert(pair != marketPollers.end());
        pair->second->Stop();    //waiting for poller thread to exit.
        marketPollers.erase(pair);
    }
	LOG4_TRACE("Succeeded");
}

void PlatformBase::UpdateAvailableStep0(shared_ptr<OrderEntity> orderEnt)
{
    assert(orderEnt->GetPlatform()->GetId() == ptmEnt->GetId());

    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();
    if (orderEnt->GetOrderType() == OrderType::Buy)
    {
        accEnt->DecAvailableMoney(orderEnt->GetFreezingAmount());
    }
    else
    {
        accEnt->DecAvailableCoinNumber(orderEnt->GetCoinNumber());
    }
}

void PlatformBase::UpdateAvailableStep1(shared_ptr<OrderEntity> orderEnt)
{
    assert(orderEnt->GetPlatform()->GetId() == ptmEnt->GetId());
    assert(orderEnt->GetClosingTime() != nullptr);

    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();
    shared_ptr<PlatformCoinRuleEntity> ptmCoinRuleEnt = ptmEnt->GetCoinRule(orderEnt->GetCoinType());
    if (orderEnt->GetOrderType() == OrderType::Buy)
    {
        accEnt->IncAvailableMoney(orderEnt->GetFreezingAmount() - orderEnt->GetFilledCoinAmount());
        accEnt->IncAvailableCoinNumber(orderEnt->GetFilledCoinNumber() * (1 - ptmCoinRuleEnt->GetTransactionFeeRate()));
    }
    else
    {
        accEnt->IncAvailableMoney(orderEnt->GetFilledCoinAmount() * (1 - ptmCoinRuleEnt->GetTransactionFeeRate()));
        accEnt->IncAvailableCoinNumber(orderEnt->GetUnfilledCoinNumber());
    }
}

void PlatformBase::Save()
{
    shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();

    odb::transaction t(db->begin());
    db->update(ptmEnt);
    t.commit();
}

/* protected functions */
uint32_t PlatformBase::ConvertCurlCodeToErrCode(CURLcode curlCode)
{
    map<CURLcode, uint32_t> curlCodes =
    {
        { CURLE_COULDNT_RESOLVE_HOST, OptResultCurlCouldntResolveHost },
        { CURLE_COULDNT_CONNECT,      OptResultCurlCouldntConnect },
        { CURLE_OPERATION_TIMEDOUT,   OptResultCurlOperationTimedout },
        { CURLE_GOT_NOTHING,          OptResultCurlGotNothing },
		{ CURLE_SEND_ERROR,           OptResultCurlSendError },
		{ CURLE_RECV_ERROR,           OptResultCurlRecvError }
    };

    auto iter = curlCodes.find(curlCode);
    if (iter == curlCodes.end())
        return OptResultUnknownCurlError;

    return iter->second;
}

bool PlatformBase::IsIgnorableCurlError(CURLcode curlCode)
{
	/* 因为我们所有的RPC调用都经过了基本的单元测试, 所以理论上来说, 所有的curl错误都可以视为可被忽略的错误.
	 * 在此记录可忽略的curl 错误列表是为了将来碰到某些特别的不能被忽略的curl错误时可以同可忽略的错误区别对待.
	 */
    static list<CURLcode> ignorableCurlErrores =
    {
        CURLE_COULDNT_CONNECT,
		CURLE_COULDNT_RESOLVE_HOST,
        CURLE_OPERATION_TIMEDOUT,
		CURLE_SSL_CONNECT_ERROR,
		CURLE_GOT_NOTHING,
		CURLE_SEND_ERROR,
		CURLE_RECV_ERROR
    };

    list<CURLcode>::iterator iter = find(ignorableCurlErrores.begin(), ignorableCurlErrores.end(), curlCode);
    return !(iter == ignorableCurlErrores.end());
}

bool PlatformBase::IsCurlInterruptedError(uint32_t errCode)
{
    static set<uint32_t> errCodes =
    {
        OptResultCurlCouldntResolveHost,
        OptResultCurlCouldntConnect,
        OptResultCurlOperationTimedout,
        OptResultCurlGotNothing
    };

    return (errCodes.find(errCode) != errCodes.end());
}

ResultCode PlatformBase::ExecuteHttpCommand(CURL *&curl, shared_ptr<PlatformApiRuleEntity> apiRule,
    const char* cmd, shared_ptr<AccountEntity> accEnt, HttpStringHandlerX handlerX)
{
    CURLcode curlCode;
    curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(cmd));
    assert((curlCode == CURLE_OK));
    curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cmd);
    assert((curlCode == CURLE_OK));

    ResultCode resultCode = ResultCodeOk;
    HttpStringHandler handler = bind(handlerX, _1, _2, _3, ref(resultCode));
    curlCode = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&handler);
    assert((curlCode == CURLE_OK));

    curlCode = curl_easy_perform(curl);
    if (curlCode != CURLE_OK)
    {
        if (IsIgnorableCurlError(curlCode))
            resultCode = MakeResultCode(SBitNotOk, FBitTrue, 1000 + curlCode);
        else
            resultCode = MakeResultCode(SBitNotOk, FBitFalse, 1000 + curlCode);

        curl_easy_cleanup(curl);
        curl = CreateCurlInstance(apiRule->GetUrl().c_str(), apiRule->GetCurlTimeout());
    }

    return resultCode;
}

/* pravite functions */
shared_ptr<AccountInfo> PlatformBase::GetAccountInfo(TableId accountId)
{
    static std::map<TableId, std::shared_ptr<AccountInfo>> accountInfores;
    static std::mutex mtx;
    lock_guard<std::mutex> lock(mtx);

    auto iter = accountInfores.find(accountId);
    if (iter == accountInfores.end())
    {
        shared_ptr<AccountInfo> accountInfo = make_shared<AccountInfo>();
        accountInfo->lastTradeTimePoint = chrono::system_clock::time_point();
        accountInfo->lastFetchTimePoint = chrono::system_clock::time_point();

        accountInfores.insert(make_pair(accountId, accountInfo));
        return accountInfo;
    }

    return iter->second;
}

bool PlatformBase::HandleMarketInfor(CoinType coinType, Money bidPrice, Money askPrice, Money lastPrice)
{
	LOG4_TRACE("Start, coin type=" << GetEnumName(coinType)
		<< ", bidPrice=" << bidPrice
		<< ", askPrice=" << askPrice
		<< ", lastPrice=" << lastPrice);

    /* When stop robot, the functions calling relationship and mutex are:
    * HedgeRobot::Stop(),  HedgeRobot::mtx
    * -> HedgeRobot::StopImpl()
    * -> DeregisterMarketHandler(), PlatformBase::handlersMtx
    *
    * when Market received, the functions calling relationship and mutex are:
    * void PlatformBase::HandleMarketInfor(),  PlatformBase::handlersMtx
    * -> void HedgeRobot::HandleMarket(),  HedgeRobot::mtx
    *
    * to avoid mutex dead lock, we use try_lock()
    */
    unique_lock<mutex> lock(handlersMtx, std::defer_lock);
    if (!lock.try_lock())
    {
        return false;
    }

    TableId id = TableIndexHelperInterface::GetInstance().GetUseableTableIndex("Market");
    shared_ptr<MarketEntity> market;
    market = make_shared<MarketEntity>(id, coinType, bidPrice, askPrice, lastPrice);
	market->SetPlatform(ptmEnt);

    list<lazy_shared_ptr<MarketEntity>>& marketEnts = ptmEnt->GetMarkets();
    if (marketEnts.size() > 1000)
    {
        /* MarketEntity产生的速度非常快, 每个平台大约每秒产生3条MarketEntity, 为了避免这些信息耗费
         * 过多的内存, 我们将不在被使用的MarketEntity从内存中清除掉.
         */
        list<lazy_shared_ptr<MarketEntity>>::iterator rangeBegin, rangeEnd;
        rangeBegin = marketEnts.begin();
        rangeEnd = marketEnts.begin();
        advance(rangeEnd, marketEnts.size() - 800);
        marketEnts.erase(rangeBegin, rangeEnd);
    }

    /* persist, 为了避免过多的market写入数据库, 我们限制每60秒最多只能记录一条market */
	time_t timeStamp = time(nullptr);
	if (marketTimeStamp < timeStamp - 60)
	{
		shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
		shared_ptr<odb::session> session = Database::GetInstance().GetSession();

		StartTransaction(db, session, MaxTransactionRetryNumber);
		db->persist(market);
		EndTransaction();

		marketTimeStamp = timeStamp;
	}

    for (MarketHandlerRepertory::iterator i = marketHandlers.begin(); i != marketHandlers.end(); ++i)
    {
        if (i->first.second == coinType)
            i->second(market);
    }
    return true;
}
