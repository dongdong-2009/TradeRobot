#include "BtdcPlatform.h"

/* C++ */
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Md5HmacSha256.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Functions */
#include "Include/Functions/MarketPollerInterface.h"
#include "Include/Functions/EnumHelperInterface.h"

#ifdef HasBtcTradeDotComPlatform

/* local header files */
#include "BtdcNonceHelper.h"

#if defined(_WIN32) && defined(_MSC_VER)
#	pragma comment(lib, "libcurl.lib")
#endif

using namespace std;
using namespace std::placeholders;

typedef ClassFactoriesRegistor<PlatformInterface, std::string, std::shared_ptr<PlatformEntity>> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, "btctrade.com", BcdcPlatform::CreateInstance);

/**********************class BcdcPlatform**********************/
BcdcPlatform::BcdcPlatform()
{}

BcdcPlatform::~BcdcPlatform()
{}

PlatformInterface* BcdcPlatform::CreateInstance(std::shared_ptr<PlatformEntity> ptmEnt)
{
	/* In C++11, the following is guaranteed to perform thread-safe initialisation: */
    static BcdcPlatform instance;
    assert(instance.GetPlatformEntity() == nullptr || instance.GetPlatformEntity()->GetId() == ptmEnt->GetId());
    instance.SetPlatformEntity(ptmEnt);
	return &instance;
}

Money BcdcPlatform::GetRoundedPrice(Money price, CoinType coinType) const
{
    std::map<CoinType, int> precisions =
    {
        { CoinType::Btc, 2 }, { CoinType::Eth, 2 }, { CoinType::Ltc, 2}
    };
    auto iter = precisions.find(coinType);
    assert(iter != precisions.end());

    return price.Round(iter->second);
}

/* pravite functions */
CURL* BcdcPlatform::CreateCurlInstance(const char *url, seconds timeout)
{
    CURLcode curlCode;
    CURL *curl = curl_easy_init();
    curlCode = curl_easy_setopt(curl, CURLOPT_URL, url);
    assert((curlCode == CURLE_OK));
    curlCode = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout.count());
    assert((curlCode == CURLE_OK));
    curlCode = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout.count());
    assert((curlCode == CURLE_OK));
    curlCode = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlOutputHandler);
    assert((curlCode == CURLE_OK));
    /* reference material:
    1 https://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
    2 http://blog.chinaunix.net/uid-23145525-id-4343403.html
    */
    curlCode = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    assert((curlCode == CURLE_OK));
    curlCode = curl_easy_setopt(curl, CURLOPT_POST, 1);
    assert((curlCode == CURLE_OK));

    return curl;
}

std::shared_ptr<AccountCurlInfo> BcdcPlatform::GetAccountCurlInfo(TableId accountId)
{
    static std::map<TableId, std::shared_ptr<AccountCurlInfo>> accountCurlInfoes;
    static std::mutex mtx;
    lock_guard<std::mutex> lock(mtx);

    auto iter = accountCurlInfoes.find(accountId);
    if (iter == accountCurlInfoes.end())
    {
        shared_ptr<PlatformApiRuleEntity> apiRule;
        shared_ptr<AccountCurlInfo> accountCurlInfo = make_shared<AccountCurlInfo>();

        /* for create order */
        apiRule = ptmEnt->GetApiRule(ApiType::Buy);
        accountCurlInfo->buyCurl = CreateCurlInstance(apiRule->GetUrl().c_str(), apiRule->GetCurlTimeout());
        apiRule = ptmEnt->GetApiRule(ApiType::Sell);
        accountCurlInfo->sellCurl = CreateCurlInstance(apiRule->GetUrl().c_str(), apiRule->GetCurlTimeout());

        /* for cancel order */
        apiRule = ptmEnt->GetApiRule(ApiType::CancelOrder);
        accountCurlInfo->cancelCurl = CreateCurlInstance(apiRule->GetUrl().c_str(), apiRule->GetCurlTimeout());

        /* for fetch order */
        apiRule = ptmEnt->GetApiRule(ApiType::FetchOrder);
        accountCurlInfo->fetchOrderCurl = CreateCurlInstance(apiRule->GetUrl().c_str(), apiRule->GetCurlTimeout());

        /* for fetch orders */
        apiRule = ptmEnt->GetApiRule(ApiType::FetchOrders);
        accountCurlInfo->fetchOrdersCurl = CreateCurlInstance(apiRule->GetUrl().c_str(), apiRule->GetCurlTimeout());

        /* for fetch banlance */
        apiRule = ptmEnt->GetApiRule(ApiType::FetchBalance);
        accountCurlInfo->fetchBalanceCurl = CreateCurlInstance(apiRule->GetUrl().c_str(), apiRule->GetCurlTimeout());

        accountCurlInfoes.insert(make_pair(accountId, accountCurlInfo));
        return accountCurlInfo;
    }

    return iter->second;
}

bool BcdcPlatform::TryToCreateBuyOrder(std::shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    assert(orderEnt->GetOrderType() == OrderType::Buy);

    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();
    Nonce nonce = BtdcNonceHelperInterface::GetInstance().GetNonce(accEnt->GetId());

    ostringstream os;
    os << "key=" << accEnt->GetPublicKey().c_str()
        << "&nonce=" << nonce
        << "&coin=" << GetEnumName(orderEnt->GetCoinType())
        << "&amount=" << orderEnt->GetCoinNumber()
        << "&price=" << orderEnt->GetLimitedPrice()
        << "&version=2";

    string signature = GetMd5HmacSha256(accEnt->GetPrivateKey().c_str(), os.str().c_str());
    string para = os.str() + string("&signature=") + signature;
    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::Buy);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->buyCurl;
    HttpStringHandlerX handlerX = bind(&BcdcPlatform::HandleHttpBuyOrderCreateString, this, _1, _2, _3, _4, orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, para.c_str(), accEnt, handlerX);
    if (GetResultSBit(resultCode) != SBitOk)
    {
        ostringstream os;
        uint32_t errCode = GetResultErrCode(resultCode);
        os << "failed, error code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")"
            << ", robot id=" << orderEnt->GetRobot()->GetId()
            << ", account id=" << accEnt->GetId()
            << ", coin type=" << GetEnumName(orderEnt->GetCoinType())
            << ", price=" << orderEnt->GetLimitedPrice()
            << ", coin number=" << orderEnt->GetCoinNumber();

		CheckDeadlineAndFBit(deadline, resultCode, os.str().c_str());
		LOG4_WARN(os.str().c_str());
        this_thread::sleep_for(milliseconds(500));
        if (IsCurlInterruptedError(errCode) || errCode == OptResultJsonFailed)
        {
            return FetchUnknownOrderImpl(orderEnt) || TryToCreateBuyOrder(orderEnt, deadline);
        }

        return TryToCreateBuyOrder(orderEnt, deadline);
    }

    if (GetResultErrCode(resultCode) != 0)
    {
        uint32_t errCode = GetResultErrCode(resultCode);
		LOG4_WARN("SBitOk, err code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")");
    }
    return true;
}

bool BcdcPlatform::CreateBuyOrderImpl(std::shared_ptr<OrderEntity> orderEnt)
{
    assert(orderEnt->GetOrderType() == OrderType::Buy);

    seconds duration = ptmEnt->GetApiRule(ApiType::Buy)->GetCurlTimeout() * 2;
    return TryToCreateBuyOrder(orderEnt, system_clock::now() + duration);
}

bool BcdcPlatform::TryToCreateSellOrder(shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    assert(orderEnt->GetOrderType() == OrderType::Sell);

    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();
    Nonce nonce = BtdcNonceHelperInterface::GetInstance().GetNonce(accEnt->GetId());

    ostringstream os;
    os << "key=" << accEnt->GetPublicKey().c_str()
        << "&nonce=" << nonce
        << "&coin=" << GetEnumName(orderEnt->GetCoinType())
        << "&amount=" << orderEnt->GetCoinNumber()
        << "&price=" << orderEnt->GetLimitedPrice()
        << "&version=2";

    string signature = GetMd5HmacSha256(accEnt->GetPrivateKey().c_str(), os.str().c_str());
    string para = os.str() + string("&signature=") + signature;
    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::Sell);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->sellCurl;
    HttpStringHandlerX handlerX = bind(&BcdcPlatform::HandleHttpSellOrderCreateString, this, _1, _2, _3, _4, orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, para.c_str(), accEnt, handlerX);
    if (GetResultSBit(resultCode) == SBitNotOk)
    {
        ostringstream os;
        uint32_t errCode = GetResultErrCode(resultCode);
        os << "failed, error code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")"
            << ", robot id=" << orderEnt->GetRobot()->GetId()
            << ", account id=" << accEnt->GetId()
            << ", coin type=" << GetEnumName(orderEnt->GetCoinType())
            << ", price=" << orderEnt->GetLimitedPrice()
            << ", coin number=" << orderEnt->GetCoinNumber();

		CheckDeadlineAndFBit(deadline, resultCode, os.str().c_str());
		LOG4_WARN(os.str().c_str());
        this_thread::sleep_for(milliseconds(500));
        if (IsCurlInterruptedError(errCode) || errCode == OptResultJsonFailed)
        {
            return FetchUnknownOrderImpl(orderEnt) || TryToCreateSellOrder(orderEnt, deadline);
        }

        return TryToCreateSellOrder(orderEnt, deadline);
    }

    if (GetResultErrCode(resultCode) != 0)
    {
        uint32_t errCode = GetResultErrCode(resultCode);
		LOG4_WARN("SBitOk, err code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")");
    }
    return true;
}

bool BcdcPlatform::CreateSellOrderImpl(std::shared_ptr<OrderEntity> orderEnt)
{
    assert(orderEnt->GetOrderType() == OrderType::Sell);

    seconds duration = ptmEnt->GetApiRule(ApiType::Sell)->GetCurlTimeout() * 2;
    return TryToCreateSellOrder(orderEnt, system_clock::now() + duration);
}

bool BcdcPlatform::TryToCancelOrder(shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();
    Nonce nonce = BtdcNonceHelperInterface::GetInstance().GetNonce(accEnt->GetId());

    EnumHelperInterface& enumHelper = EnumHelperInterface::GetInstance();
    ostringstream os;
    os << "key=" << accEnt->GetPublicKey().c_str()
        << "&nonce=" << nonce
        << "&id=" << orderEnt->GetPlatformOrderId()
        << "&version=2";

    string signature = GetMd5HmacSha256(accEnt->GetPrivateKey().c_str(), os.str().c_str());
    string para = os.str() + string("&signature=") + signature;
    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::CancelOrder);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->cancelCurl;
    HttpStringHandlerX handlerX = bind(&BcdcPlatform::HandleHttpOrderCancelString, this,
        _1, _2, _3, _4, orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, para.c_str(), accEnt, handlerX);
    if (GetResultSBit(resultCode) != SBitOk)
    {
        ostringstream os;
        uint32_t errCode = GetResultErrCode(resultCode);
        os << "failed, error code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")"
            << ", robot id=" << orderEnt->GetRobot()->GetId()
            << ", account id=" << accEnt->GetId()
            << ", order type=" << GetEnumName(orderEnt->GetOrderType())
            << ", coin type=" << GetEnumName(orderEnt->GetCoinType())
            << ", price=" << orderEnt->GetLimitedPrice()
            << ", coin number=" << orderEnt->GetCoinNumber();

		CheckDeadlineAndFBit(deadline, resultCode, os.str().c_str());
		LOG4_WARN(os.str().c_str());
        this_thread::sleep_for(milliseconds(500));
        return TryToCancelOrder(orderEnt, deadline);
    }
    assert(orderEnt->GetClosingTime() != nullptr);

    if (GetResultErrCode(resultCode) != 0)
    {
        uint32_t errCode = GetResultErrCode(resultCode);
		LOG4_WARN("SBitOk, err code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")");
    }

    return true;
}

bool BcdcPlatform::CancelOrderImpl(std::shared_ptr<OrderEntity> orderEnt)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::CancelOrder)->GetCurlTimeout() * 2;
    return TryToCancelOrder(orderEnt, system_clock::now() + duration);
}

bool BcdcPlatform::TryToFetchOrder(shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();
    Nonce nonce = BtdcNonceHelperInterface::GetInstance().GetNonce(accEnt->GetId());

    EnumHelperInterface& enumHelper = EnumHelperInterface::GetInstance();
    ostringstream os;
    os << "key=" << accEnt->GetPublicKey().c_str()
        << "&nonce=" << nonce
        << "&id=" << orderEnt->GetPlatformOrderId()
        << "&version=2";

    string signature = GetMd5HmacSha256(accEnt->GetPrivateKey().c_str(), os.str().c_str());
    string para = os.str() + string("&signature=") + signature;
    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::FetchOrder);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->fetchOrderCurl;
    HttpStringHandlerX handlerX = bind(&BcdcPlatform::HandleHttpOrderFetchString, this, _1, _2, _3, _4, orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, para.c_str(), accEnt, handlerX);
    if (GetResultSBit(resultCode) != SBitOk)
    {
        ostringstream os;
        uint32_t errCode = GetResultErrCode(resultCode);
        os << "failed, error code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")"
            << ", robot id='" << orderEnt->GetRobot()->GetId() << "'"
            << ", account id='" << accEnt->GetId() << "'"
            << ", order type=" << GetEnumName(orderEnt->GetOrderType())
            << ", coin type=" << GetEnumName(orderEnt->GetCoinType())
            << ", price=" << orderEnt->GetLimitedPrice()
            << ", coin number=" << orderEnt->GetCoinNumber();

		CheckDeadlineAndFBit(deadline, resultCode, os.str().c_str());
        LOG4_WARN(os.str().c_str());
        this_thread::sleep_for(milliseconds(500));
        return TryToFetchOrder(orderEnt, deadline);
    }

    if (GetResultErrCode(resultCode) != 0)
    {
        uint32_t errCode = GetResultErrCode(resultCode);
		LOG4_WARN("SBitOk, err code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")");
    }
    return true;
}

bool BcdcPlatform::FetchOrderImpl(std::shared_ptr<OrderEntity> orderEnt)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::FetchOrder)->GetCurlTimeout() * 2;
    return TryToFetchOrder(orderEnt, system_clock::now() + duration);
}

bool BcdcPlatform::TryToFetchUnknownOrder(shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();
    Nonce nonce = BtdcNonceHelperInterface::GetInstance().GetNonce(accEnt->GetId());

    /* 经测试, 同一个order 本地记录的时间和btctrade.com上记录的时间大约有20秒的误差, 为了委托起见,
     * 我们查询30秒以内的order.
     */
    #define TimeDeviation 30
    time_t startTime = system_clock::to_time_t(orderEnt->GetCreatingTime()) - TimeDeviation;
    ostringstream os;
    os << "coin=" << GetEnumName(orderEnt->GetCoinType())
        << "&key=" << accEnt->GetPublicKey().c_str()
        << "&nonce=" << nonce
        << "&type=all"
        << "&since=" << startTime
        << "&ob=ASC"
        << "&version=2";
    string signature = string("&signature=") + GetMd5HmacSha256(accEnt->GetPrivateKey().c_str(), os.str().c_str());
    string para = os.str() + signature;

    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::FetchOrders);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->fetchOrdersCurl;
    std::string strBuffer;
    HttpStringHandlerX handlerX = bind(&BcdcPlatform::HandleHttpUnknownOrderFetchString, this,
        _1, _2, _3, _4, ref(strBuffer), orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, para.c_str(), accEnt, handlerX);
    if (GetResultSBit(resultCode) != SBitOk)
    {
        ostringstream os;
        uint32_t errCode = GetResultErrCode(resultCode);
        os << "failed, error code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")"
            << ", robot id=" << orderEnt->GetRobot()->GetId()
            << ", account id=" << accEnt->GetId();

		CheckDeadlineAndFBit(deadline, resultCode, os.str().c_str());
		LOG4_WARN(os.str().c_str());
        this_thread::sleep_for(milliseconds(500));
        return TryToFetchUnknownOrder(orderEnt, deadline);
    }

    if (GetResultErrCode(resultCode) != 0)
    {
        uint32_t errCode = GetResultErrCode(resultCode);
		LOG4_WARN("SBitOk, err code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")");
    }
    return true;
}

bool BcdcPlatform::FetchUnknownOrderImpl(shared_ptr<OrderEntity> orderEnt)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::FetchOrders)->GetCurlTimeout() * 2;
    return TryToFetchUnknownOrder(orderEnt, system_clock::now() + duration);
}

bool BcdcPlatform::TryToFetchBalance1(shared_ptr<AccountEntity> accEnt, Money& balance, time_point deadline)
{
    Nonce nonce = BtdcNonceHelperInterface::GetInstance().GetNonce(accEnt->GetId());

    ostringstream os;
    os << "key=" << accEnt->GetPublicKey().c_str()
        << "&nonce=" << nonce
        << "&version=2";

    string signature = GetMd5HmacSha256(accEnt->GetPrivateKey().c_str(), os.str().c_str());
    string para = os.str() + string("&signature=") + signature;
    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::FetchBalance);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->fetchBalanceCurl;
    HttpStringHandlerX handlerX = bind(&BcdcPlatform::HandleHttpBalanceFetchString1, this,
        _1, _2, _3, _4, ref(balance));

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, para.c_str(), accEnt, handlerX);
    if (GetResultSBit(resultCode) != SBitOk)
    {
        ostringstream os;
        uint32_t errCode = GetResultErrCode(resultCode);
        os << "failed, error code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")"
            << ", account id=" << accEnt->GetId();

		CheckDeadlineAndFBit(deadline, resultCode, os.str().c_str());
		LOG4_WARN(os.str().c_str());
        this_thread::sleep_for(milliseconds(500));
        return TryToFetchBalance1(accEnt, balance, deadline);
    }

    if (GetResultErrCode(resultCode) != 0)
    {
        uint32_t errCode = GetResultErrCode(resultCode);
		LOG4_WARN("SBitOk, err code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")");
    }
    return true;
}

bool BcdcPlatform::FetchBalanceImpl1(std::shared_ptr<AccountEntity> accEnt, Money& balance)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::FetchBalance)->GetCurlTimeout() * 2;
    return TryToFetchBalance1(accEnt, balance, system_clock::now() + duration);
}

bool BcdcPlatform::TryToFetchBalance2(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance,
    time_point deadline)
{
    Nonce nonce = BtdcNonceHelperInterface::GetInstance().GetNonce(accEnt->GetId());

    ostringstream os;
    os << "key=" << accEnt->GetPublicKey().c_str()
        << "&nonce=" << nonce
        << "&version=2";

    string signature = GetMd5HmacSha256(accEnt->GetPrivateKey().c_str(), os.str().c_str());
    string para = os.str() + string("&signature=") + signature;
    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::FetchBalance);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->fetchBalanceCurl;
    HttpStringHandlerX handlerX = bind(&BcdcPlatform::HandleHttpBalanceFetchString2, this,
        _1, _2, _3, _4, coinType, ref(balance));

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, para.c_str(), accEnt, handlerX);
    if (GetResultSBit(resultCode) != SBitOk)
	{
        ostringstream os;
        uint32_t errCode = GetResultErrCode(resultCode);
        os << "failed, error code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")"
            << ", account id=" << accEnt->GetId();

		CheckDeadlineAndFBit(deadline, resultCode, os.str().c_str());
		LOG4_WARN(os.str().c_str());
        this_thread::sleep_for(milliseconds(500));
        return TryToFetchBalance2(accEnt, coinType, balance, deadline);
    }

    if (GetResultErrCode(resultCode) != 0)
    {
        uint32_t errCode = GetResultErrCode(resultCode);
		LOG4_WARN("SBitOk, err code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")");
    }
    return true;
}

bool BcdcPlatform::FetchBalanceImpl2(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::FetchBalance)->GetCurlTimeout() * 2;
    return TryToFetchBalance2(accEnt, coinType, balance, system_clock::now() + duration);
}

size_t BcdcPlatform::HandleHttpOrderCreateString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, std::shared_ptr<OrderEntity> orderEnt, ResultCode whatForSU002)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    picojson::value value;
    std::string err;
    picojson::parse(value, (char*)ptr, (char*)ptr + nmemb*size, &err);
	CheckJsonResult(err);
    picojson::object& o = value.get<picojson::object>();

    LOG4_TRACE((char*)ptr);
    picojson::object::iterator result = o.find("result");
    assert(result != o.end());
    if (!result->second.get<bool>())
    {
        /*
        {
        "message" : "xxx xxx",
        "result" : false
        }
        */
        assert(o.find("message") != o.end());
        string message = o["message"].get<string>();
        LOG4_WARN("failed,  message=" << ConvertUtf8ToString(message.c_str()));

        uint32_t errCode = ConvertToErrCode(message);
        switch (errCode)
        {
        case OptResultResourceNotEnough:
            /* buy : OptResultMoneyNotEnough
               sell: OptResultCoinNotEnough
            */
            resultCode = MakeResultCode(SBitNotOk, FBitFalse, whatForSU002);
            break;

        case OptResultOptTooFrequent:
            resultCode = MakeResultCode(SBitNotOk, FBitTrue, errCode);
            break;

        default:
            resultCode = MakeResultCode(SBitNotOk, FBitFalse, errCode);
        }

        return (nmemb * size);
    }

    if (o.find("id") == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    resultCode = ResultCodeOk;
    orderEnt->SetPlatformOrderId((PlatformOrderId)stoull(o["id"].get<string>().c_str()));
    return (nmemb * size);
}

size_t BcdcPlatform::HandleHttpBuyOrderCreateString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, std::shared_ptr<OrderEntity> orderEnt)
{
    return HandleHttpOrderCreateString(ptr, size, nmemb, resultCode, orderEnt, OptResultMoneyNotEnough);
}

size_t BcdcPlatform::HandleHttpSellOrderCreateString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, std::shared_ptr<OrderEntity> orderEnt)
{
    return HandleHttpOrderCreateString(ptr, size, nmemb, resultCode, orderEnt, OptResultCoinNotEnough);
}

size_t BcdcPlatform::HandleHttpOrderCancelString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, std::shared_ptr<OrderEntity> orderEnt)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    picojson::value value;
    std::string err;
    picojson::parse(value, (char*)ptr, (char*)ptr + nmemb*size, &err);
	CheckJsonResult(err);
    picojson::object& o = value.get<picojson::object>();

    LOG4_TRACE((char*)ptr);
    picojson::object::iterator result = o.find("result");
    assert(result != o.end());
    if (!result->second.get<bool>())
    {
        /*
        {
        "message" : "xxx xxx",
        "result" : false
        }
        */
        assert(o.find("message") != o.end());
        string message = o["message"].get<string>();
        LOG4_WARN("failed,  message=" << ConvertUtf8ToString(message.c_str()));

        uint32_t errCode = ConvertToErrCode(message);
        switch (errCode)
        {
        case OptResultOrderIsClosed:
            if (orderEnt->GetClosingTime() == nullptr)
                orderEnt->SetClosingTime(system_clock::now());
            resultCode = MakeResultCode(ResultCodeOk, FBitFalse, errCode);
            break;

        case OptResultOptTooFrequent:
            resultCode = MakeResultCode(SBitNotOk, FBitTrue, errCode);
            break;

        default:
            resultCode = MakeResultCode(SBitNotOk, FBitFalse, errCode);
        }

        return (nmemb * size);
    }

	orderEnt->SetClosingTime(system_clock::now());
    resultCode = ResultCodeOk;
    return (nmemb * size);
}

/*
{
    "amount_original" : 0.080,
    "amount_outstanding" : 0,
    "datetime" : "2016-10-28 10:18:48",
    "id" : 271399037,
    "price" : 4717.790,
    "status" : "cancelled",
    "trades" :
    {
        "avg_price" : 4717.790,
        "sum_money" : 47.1779000000000000000000,
        "sum_number" : 0.01000000000
    },
    "type" : "sell"
}
*/
size_t BcdcPlatform::HandleHttpOrderFetchString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, std::shared_ptr<OrderEntity> orderEnt)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    picojson::value value;
    std::string err;
    picojson::parse(value, (char*)ptr, (char*)ptr + nmemb*size, &err);
	CheckJsonResult(err);
    picojson::object& o = value.get<picojson::object>();

    LOG4_TRACE((char*)ptr);
    picojson::object::iterator result = o.find("result");
    if (result != o.end() && !result->second.get<bool>())
    {
        assert(o.find("message") != o.end());
        string message = o["message"].get<string>();
        LOG4_WARN("failed,  message=" << ConvertUtf8ToString(message.c_str()));

		/* 理论上来说, Fetch 操作不可能失败. 如果失败, 一定是可以retry的. */
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, ConvertToErrCode(message));
        return (nmemb * size);
    }

    if (o.find("amount_original") == o.end()
        || o.find("price") == o.end()
        || o.find("status") == o.end()
        || o.find("trades") == o.end())
    {
        LOG4_WARN("json string contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    resultCode = ResultCodeOk;
    string orderState = o["status"].get<string>();
    if (orderState.find("closed") != string::npos || orderState.find("cancelled") != string::npos)
    {
        if (orderEnt->GetClosingTime() == nullptr)
        {
			orderEnt->SetClosingTime(system_clock::now());
        }
    }
    assert(orderEnt->GetCoinNumber() ==  CoinNumber(o["amount_original"].get<double>()));
    assert(orderEnt->GetLimitedPrice() == Money(o["price"].get<double>()));

    picojson::object& trades = o["trades"].get<picojson::object>();
    if (trades.find("sum_number") == trades.end() || trades.find("avg_price") == trades.end())
    {
        LOG4_WARN("json string contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }
    CoinNumber filledCoinNumber(trades["sum_number"].get<double>());
    Money concludedPrice(trades["avg_price"].get<double>());

    orderEnt->SetFilledCoinNumber(filledCoinNumber);
    if (filledCoinNumber == CoinNumber::Zero())
    {
        assert(concludedPrice == Money::Zero());
    }
    else
    {
        orderEnt->SetConcludedPrice(concludedPrice);
    }

    return (nmemb * size);
}

size_t BcdcPlatform::HandleHttpUnknownOrderFetchString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, std::string& strBuffer, std::shared_ptr<OrderEntity> orderEnt)
{
    resultCode = MakeResultCode(SBitNotOk, FBitFalse, OptResultNoMatchedOrderId);
    char *str = (char *)ptr;
    if (strncmp(str + 2, "result", 6) == 0)
    {
        strBuffer.assign("");
        return size*nmemb;
    }

    if (str[0] == '[')
    {
        strBuffer.assign(str);
    }
    else
    {
        strBuffer.append(str);
    }

    string::size_type endpoint = strBuffer.find("]");
    if (endpoint != string::npos)
    {
        strBuffer = strBuffer.substr(0, endpoint + 1);
        picojson::value value;
        std::string err;
        picojson::parse(value, strBuffer.c_str(), strBuffer.c_str() + strBuffer.length(), &err);
		CheckJsonResult(err);
        picojson::array& orders = value.get<picojson::array>();

        LOG4_TRACE(strBuffer.c_str());
        set<PlatformOrderId> ptmOrderIds;
        list<lazy_shared_ptr<OrderEntity>>& orderEnts = orderEnt->GetAccount()->GetOrders();
        for (auto i = orderEnts.rbegin(); i != orderEnts.rend() && ptmOrderIds.size() < 20; ++i)
        {
            if (i->object_id() == orderEnt->GetId())
                continue;

            ptmOrderIds.insert(i->load()->GetPlatformOrderId());
            i->unload();
        }

        resultCode = MakeResultCode(SBitNotOk, FBitFalse, OptResultNoMatchedOrderId);
        for (auto i = orders.size(); i > 0; --i)
        {
            picojson::object& order = orders[i-1].get<picojson::object>();

            assert(order.find("id") != order.end()
                && order.find("amount_outstanding") != order.end()
                && order.find("datetime") != order.end()
                && order.find("type") != order.end()
                && order.find("price") != order.end()
                && order.find("amount_original") != order.end());

            PlatformOrderId ptmOrderId = stoull(order["id"].get<string>());

            if (ptmOrderIds.find(ptmOrderId) == ptmOrderIds.end())
            {
                EnumHelperInterface& enumHelper = EnumHelperInterface::GetInstance();
                assert(enumHelper.GetOrderType(order["type"].get<string>().c_str()) == orderEnt->GetOrderType());
                assert(Money(order["price"].get<double>()) == orderEnt->GetLimitedPrice());
                assert(CoinNumber(order["amount_original"].get<double>()) == orderEnt->GetCoinNumber());

                orderEnt->SetPlatformOrderId(ptmOrderId);
                resultCode = ResultCodeOk;
                break;
            }
        }
    }

    return (nmemb * size);
}

size_t BcdcPlatform::HandleHttpBalanceFetchString1(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, Money& balance)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    picojson::value value;
    std::string err;
    picojson::parse(value, (char*)ptr, (char*)ptr + nmemb*size, &err);
	CheckJsonResult(err);
    picojson::object& o = value.get<picojson::object>();

    LOG4_TRACE((char*)ptr);
    picojson::object::iterator result = o.find("result");
    if (result != o.end() && !result->second.get<bool>())
    {
        assert(o.find("message") != o.end());
        string message = o["message"].get<string>();
        LOG4_WARN("failed,  message=" << ConvertUtf8ToString(message.c_str()));

        resultCode = MakeResultCode(SBitNotOk, FBitTrue, ConvertToErrCode(message));
        return (nmemb * size);
    }

    if (o.find("uid") == o.end() || o.find("cny_balance") == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    resultCode = ResultCodeOk;
    balance = Money(strtod(o["cny_balance"].get<string>().c_str(), nullptr));

    return (nmemb * size);
}

size_t BcdcPlatform::HandleHttpBalanceFetchString2(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, CoinType coinType, CoinNumber& balance)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    picojson::value value;
    std::string err;
    picojson::parse(value, (char*)ptr, (char*)ptr + nmemb*size, &err);
	CheckJsonResult(err);
    picojson::object& o = value.get<picojson::object>();

    LOG4_TRACE((char*)ptr);
    picojson::object::iterator result = o.find("result");
    if (result != o.end() && !result->second.get<bool>())
    {
        assert(o.find("message") != o.end());
        string message = o["message"].get<string>();
        LOG4_WARN("failed,  message=" << ConvertUtf8ToString(message.c_str()));

        resultCode = MakeResultCode(SBitNotOk, FBitTrue, ConvertToErrCode(message));
        return (nmemb * size);
    }

    string name = EnumHelperInterface::GetInstance().GetName(coinType);
    name.append("_balance");
    if (o.find("uid") == o.end()
        || o.find("cny_balance") == o.end()
        || o.find(name.c_str()) == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    resultCode = ResultCodeOk;
    balance = CoinNumber(strtod(o[name.c_str()].get<string>().c_str(), nullptr));

    return (nmemb * size);
}

uint32_t BcdcPlatform::ConvertToErrCode(const string& msg)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    static list<pair<string, uint32_t>> msges =
    {
        { "nonce has expired", OptResultNonceHasExpired }, /* nonce has expired */
        { "[S_U_001]", OptResultOptTooFrequent },          /* 请过1秒后，再进行此操作[S_U_001] */
        { "[S_U_002]", OptResultResourceNotEnough },       /* 您没有足够的人民币[S_U_002] / 您没有足够的btc[S_U_002] */
        { "parameter error", OptResultParameterError },    /* parameter error */
        { conv.to_bytes(L"撤消失败，您的委托已经全部成交或已撤销"), OptResultOrderIsClosed },
    };

    uint32_t errCode = OptResultUnknownPtmError;
    for (auto i = msges.begin(); i != msges.end(); ++i)
    {
        if (msg.find(i->first) != string::npos)
        {
            errCode = i->second;
            break;
        }
    }

    return errCode;
}

#endif