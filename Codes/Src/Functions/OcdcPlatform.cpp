#include "OcdcPlatform.h"

/* C++ */
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Md5HmacSha256.h"
#include "Include/Foundation/Debug.h"

/* Functions */
#include "Include/Functions/EnumHelperInterface.h"
#include "Include/Functions/TableIndexHelperInterface.h"
#include "Include/Functions/MarketPollerInterface.h"

#ifdef HasOkCoinDotCnPlatform

/* local header files */
#include "OcdcCmdString.h"

#if defined(_WIN32) && defined(_MSC_VER)
#	pragma comment(lib, "libcurl.lib")
#endif

using namespace std;
using namespace std::placeholders;

typedef ClassFactoriesRegistor<PlatformInterface, string, shared_ptr<PlatformEntity>> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, "okcoin.cn", OcdcPlatform::CreateInstance);

/**********************class OcdcPlatform**********************/
OcdcPlatform::OcdcPlatform()
{}

OcdcPlatform::~OcdcPlatform()
{}

PlatformInterface* OcdcPlatform::CreateInstance(shared_ptr<PlatformEntity> ptmEnt)
{
	/* In C++11, the following is guaranteed to perform thread-safe initialisation: */
    static OcdcPlatform instance;
    assert(instance.GetPlatformEntity() == nullptr || instance.GetPlatformEntity()->GetId() == ptmEnt->GetId());
    instance.SetPlatformEntity(ptmEnt);
	return &instance;
}

Money OcdcPlatform::GetRoundedPrice(Money price, CoinType coinType) const
{
    std::map<CoinType, int> precisions =
    {
        { CoinType::Btc, 2 }, { CoinType::Eth, 2 }, { CoinType::Ltc, 2 }
    };
    auto iter = precisions.find(coinType);
    assert(iter != precisions.end());

    return price.Round(iter->second);
}

/* pravite functions */
CURL* OcdcPlatform::CreateCurlInstance(const char *url, seconds timeout)
{
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* CA verification */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);

    /* reference material:
    1 https://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
    2 http://blog.chinaunix.net/uid-23145525-id-4343403.html
    */
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    // 设置301、302跳转跟随location
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout.count());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout.count());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlOutputHandler);
    curl_easy_setopt(curl, CURLOPT_POST, 1);

    return curl;
}

shared_ptr<AccountCurlInfo> OcdcPlatform::GetAccountCurlInfo(TableId accountId)
{
    static std::map<TableId, shared_ptr<AccountCurlInfo>> accountCurlInfoes;
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

bool OcdcPlatform::TryToCreateBuyOrder(shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    assert(orderEnt->GetOrderType() == OrderType::Buy);
    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();

    OcdcCmdString cmdString;
    cmdString.AddParameter("amount", orderEnt->GetCoinNumber());
    cmdString.AddParameter("api_key", accEnt->GetPublicKey());
    cmdString.AddParameter("price", orderEnt->GetLimitedPrice());
    cmdString.AddParameter("symbol", orderEnt->GetCoinType());
    cmdString.AddParameter("type", orderEnt->GetOrderType());
    string sign = cmdString.GetSign(accEnt->GetPrivateKey());
    cmdString.AddParameter("secret_key", accEnt->GetPrivateKey());
    cmdString.AddParameter("sign", sign);

    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::Buy);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->buyCurl;
    HttpStringHandlerX handlerX = bind(&OcdcPlatform::HandleHttpOrderCreateString, this,
        _1, _2, _3, _4, orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, cmdString.GetCmdString().c_str(), accEnt, handlerX);
    if (GetResultSBit(resultCode) != SBitOk)
    {
        ostringstream os;
        uint32_t errCode = GetResultErrCode(resultCode);
        os << "failed, error code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")"
            << ", robot id=" << orderEnt->GetRobot()->GetId()
            << ", account id=" << accEnt->GetId()
            << ", coinType=" << GetEnumName(orderEnt->GetCoinType())
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

bool OcdcPlatform::CreateBuyOrderImpl(shared_ptr<OrderEntity> orderEnt)
{
    assert(orderEnt->GetOrderType() == OrderType::Buy);

    seconds duration = ptmEnt->GetApiRule(ApiType::Buy)->GetCurlTimeout() * 2;
    return TryToCreateBuyOrder(orderEnt, system_clock::now() + duration);
}

bool OcdcPlatform::TryToCreateSellOrder(shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    assert(orderEnt->GetOrderType() == OrderType::Sell);
    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();

    OcdcCmdString cmdString;
    cmdString.AddParameter("amount", orderEnt->GetCoinNumber());
    cmdString.AddParameter("api_key", accEnt->GetPublicKey());
    cmdString.AddParameter("price", orderEnt->GetLimitedPrice());
    cmdString.AddParameter("symbol", orderEnt->GetCoinType());
    cmdString.AddParameter("type", orderEnt->GetOrderType());
    string sign = cmdString.GetSign(accEnt->GetPrivateKey());
    cmdString.AddParameter("secret_key", accEnt->GetPrivateKey());
    cmdString.AddParameter("sign", sign);

    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::Sell);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->sellCurl;
    HttpStringHandlerX handlerX = bind(&OcdcPlatform::HandleHttpOrderCreateString, this,
        _1, _2, _3, _4, orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, cmdString.GetCmdString().c_str(), accEnt, handlerX);
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

bool OcdcPlatform::CreateSellOrderImpl(shared_ptr<OrderEntity> orderEnt)
{
    assert(orderEnt->GetOrderType() == OrderType::Sell);

    seconds duration = ptmEnt->GetApiRule(ApiType::Sell)->GetCurlTimeout() * 2;
    return TryToCreateSellOrder(orderEnt, system_clock::now() + duration);
}

bool OcdcPlatform::TryToCancelOrder(shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();

    OcdcCmdString cmdString;
    cmdString.AddParameter("api_key", accEnt->GetPublicKey());
    cmdString.AddParameter("order_id", orderEnt->GetPlatformOrderId());
    cmdString.AddParameter("symbol", orderEnt->GetCoinType());
    string sign = cmdString.GetSign(accEnt->GetPrivateKey());
    cmdString.AddParameter("secret_key", accEnt->GetPrivateKey());
    cmdString.AddParameter("sign", sign);

    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::CancelOrder);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->cancelCurl;
    HttpStringHandlerX handlerX = bind(&OcdcPlatform::HandleHttpOrderCancelString, this,
        _1, _2, _3, _4, orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, cmdString.GetCmdString().c_str(), accEnt, handlerX);
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

bool OcdcPlatform::CancelOrderImpl(shared_ptr<OrderEntity> orderEnt)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::CancelOrder)->GetCurlTimeout() * 2;
    return TryToCancelOrder(orderEnt, system_clock::now() + duration);
}

bool OcdcPlatform::TryToFetchOrder(shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();

    OcdcCmdString cmdString;
    cmdString.AddParameter("api_key", accEnt->GetPublicKey());
    cmdString.AddParameter("order_id", orderEnt->GetPlatformOrderId());
    cmdString.AddParameter("symbol", orderEnt->GetCoinType());
    string sign = cmdString.GetSign(accEnt->GetPrivateKey());
    cmdString.AddParameter("secret_key", accEnt->GetPrivateKey());
    cmdString.AddParameter("sign", sign);

    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::FetchOrder);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->fetchOrderCurl;
    HttpStringHandlerX handlerX = bind(&OcdcPlatform::HandleHttpOrderFetchString, this,
        _1, _2, _3, _4, orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, cmdString.GetCmdString().c_str(), accEnt, handlerX);
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

bool OcdcPlatform::FetchOrderImpl(shared_ptr<OrderEntity> orderEnt)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::FetchOrder)->GetCurlTimeout() * 2;
    return TryToFetchOrder(orderEnt, system_clock::now() + duration);
}

bool OcdcPlatform::TryToFetchUnknownOrder(shared_ptr<OrderEntity> orderEnt, time_point deadline)
{
    shared_ptr<AccountEntity> accEnt = orderEnt->GetAccount();

    OcdcCmdString cmdString;
    cmdString.AddParameter("api_key", accEnt->GetPublicKey());
    cmdString.AddParameter("current_page", uint32_t(1));
    cmdString.AddParameter("page_length", uint32_t(100));
    cmdString.AddParameter("status", uint32_t(0)); /* 0：未完成的订单 */
    cmdString.AddParameter("symbol", orderEnt->GetCoinType());
    string sign = cmdString.GetSign(accEnt->GetPrivateKey());
    cmdString.AddParameter("secret_key", accEnt->GetPrivateKey());
    cmdString.AddParameter("sign", sign);

    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::FetchOrders);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->fetchOrdersCurl;
    HttpStringHandlerX handlerX = bind(&OcdcPlatform::HandleHttpUnknownOrderFetchString, this,
        _1, _2, _3, _4, orderEnt);

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, cmdString.GetCmdString().c_str(), accEnt, handlerX);
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

bool OcdcPlatform::FetchUnknownOrderImpl(shared_ptr<OrderEntity> orderEnt)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::FetchOrders)->GetCurlTimeout() * 2;
    return TryToFetchUnknownOrder(orderEnt, system_clock::now() + duration);
}

bool OcdcPlatform::TryToFetchBalance1(shared_ptr<AccountEntity> accEnt, Money& balance, time_point deadline)
{
    OcdcCmdString cmdString;
    cmdString.AddParameter("api_key", accEnt->GetPublicKey());
    string sign = cmdString.GetSign(accEnt->GetPrivateKey());
    cmdString.AddParameter("secret_key", accEnt->GetPrivateKey());
    cmdString.AddParameter("sign", sign);

    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::FetchBalance);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->fetchBalanceCurl;
    HttpStringHandlerX handlerX = bind(&OcdcPlatform::HandleHttpBalanceFetchString1, this,
        _1, _2, _3, _4, ref(balance));

    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, cmdString.GetCmdString().c_str(), accEnt, handlerX);
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

bool OcdcPlatform::FetchBalanceImpl1(shared_ptr<AccountEntity> accEnt, Money& balance)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::FetchBalance)->GetCurlTimeout() * 2;
    return TryToFetchBalance1(accEnt, balance, system_clock::now() + duration);
}

bool OcdcPlatform::TryToFetchBalance2(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance,
    time_point deadline)
{
    OcdcCmdString cmdString;
    cmdString.AddParameter("api_key", accEnt->GetPublicKey());
    string sign = cmdString.GetSign(accEnt->GetPrivateKey());
    cmdString.AddParameter("secret_key", accEnt->GetPrivateKey());
    cmdString.AddParameter("sign", sign);

    shared_ptr<PlatformApiRuleEntity> apiRule = ptmEnt->GetApiRule(ApiType::FetchBalance);
    CURL* curl = GetAccountCurlInfo(accEnt->GetId())->fetchBalanceCurl;
    HttpStringHandlerX handlerX = bind(&OcdcPlatform::HandleHttpBalanceFetchString2, this,
        _1, _2, _3, _4, coinType, ref(balance));
    ResultCode resultCode = ExecuteHttpCommand(curl, apiRule, cmdString.GetCmdString().c_str(), accEnt, handlerX);

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

bool OcdcPlatform::FetchBalanceImpl2(shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance)
{
    seconds duration = ptmEnt->GetApiRule(ApiType::FetchBalance)->GetCurlTimeout() * 2;
    return TryToFetchBalance2(accEnt, coinType, balance, system_clock::now() + duration);
}

/*
{
"error_code" : 10000,
"result" : false
}
*/
size_t OcdcPlatform::HandleHttpOrderCreateString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, shared_ptr<OrderEntity> orderEnt)
{
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
        assert(o.find("error_code") != o.end());
        uint32_t ocdcCode = (uint32_t)o["error_code"].get<int64_t>();
        uint32_t errCode = ConvertToErrCode(ocdcCode);
        LOG4_WARN("failed, ocdc code=" << ocdcCode << ", error string=" << ConvertErrCodeToString(errCode));
        switch (errCode)
        {
        case OptResultOptTooFrequent:
            resultCode = MakeResultCode(SBitNotOk, FBitTrue, errCode);
            break;

        default:
            resultCode = MakeResultCode(SBitNotOk, FBitFalse, errCode);
        }

        return (nmemb * size);
    }

    if (o.find("order_id") == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    resultCode = ResultCodeOk;
    orderEnt->SetPlatformOrderId((PlatformOrderId)o["order_id"].get<int64_t>());
    return (nmemb * size);
}

/*
{
"error_code" : 10000,
"result" : false
}
*/
size_t OcdcPlatform::HandleHttpOrderCancelString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, shared_ptr<OrderEntity> orderEnt)
{
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
        assert(o.find("error_code") != o.end());
        uint32_t ocdcCode = (uint32_t)o["error_code"].get<int64_t>();
        uint32_t errCode = ConvertToErrCode(ocdcCode);
        LOG4_WARN("failed, ocdc code=" << ocdcCode << ", error string=" << ConvertErrCodeToString(errCode));
        switch (errCode)
        {
		case OptResultNoMatchedOrderId:
			/* 如果 order之前已经被取消或关闭, okcoin.cn会返回 "10009 订单不存在"的错误,
			* 此时， 我们只能假设所有的 10009 错误都是由于order 已经取消或关闭造成的。
			* 因为 robot 一定会在cancel之后 fetch order， 所以， 如果我们的程序真的有误，并
			* 使用的不存在的platform order id， 这个错误一定会在fetch 操作时被发现。
			*/
            orderEnt->SetClosingTime(system_clock::now());
            resultCode = MakeResultCode(SBitOk, FBitFalse, errCode);
			break;

		case OptResultCancelOptRepeated:
			/* 由于网络不稳定, 我们下达了cancel指令, 但是没有收到platform 的response. 于是我们又发出了重复的指令
			*/
			orderEnt->SetClosingTime(system_clock::now());
			resultCode = MakeResultCode(SBitOk, FBitFalse, errCode);
			break;

        case OptResultOptTooFrequent:
        case OptResulOcdcSystemError:
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
"error_code" : 10000,
"result" : false
}

{
    "orders" :
    [
        {
        "amount" : 0.10000000000000001,
        "avg_price" : 0,
        "create_date" : 1484808921000,
        "deal_amount" : 0,
        "order_id" : 8209564836,
        "orders_id" : 8209564836,
        "price" : 100,
        "status" : -1,
        "symbol" : "btc_cny",
        "type" : "buy"
        }
    ],
    "result" : true
}
*/
size_t OcdcPlatform::HandleHttpOrderFetchString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, shared_ptr<OrderEntity> orderEnt)
{
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
        assert(o.find("error_code") != o.end());
        uint32_t ocdcCode = (uint32_t)o["error_code"].get<int64_t>();
        uint32_t errCode = ConvertToErrCode(ocdcCode);
        LOG4_WARN("failed, ocdc code=" << ocdcCode << ", error string=" << ConvertErrCodeToString(errCode));
        assert(errCode != OptResultNoMatchedOrderId);

        /* 理论上来说, Fetch 操作不可能失败. 如果失败, 一定是可以retry的. */
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, ConvertToErrCode(ocdcCode));
        return (nmemb * size);
    }

    if (o.find("orders") == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }
    picojson::array& orders = o["orders"].get<picojson::array>();
    /* okcoin will delete order 2 days later when the order has been canceled.
     * 我们的程序不可能在运行的过程中中断这么长的时间， 所以所有取消或正常关闭
     * 的order不可能48小时后才对其做fetch操作。为了避免漏发现程序内部的其他错误，
     * 我们不允许(value["orders"].size() == 0)的情况出现。
     */
    assert(orders.size() != 0);
    resultCode = ResultCodeOk;

    /* status:-1:已撤销  0:未成交  1:部分成交  2:完全成交 4:撤单处理中 */
    picojson::object& order = orders[0].get<picojson::object>();
    if (order.find("status") == order.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }
    int orderState = (int)order["status"].get<int64_t>();
    if (orderState == -1 || orderState == 2)
    {
        if (orderEnt->GetClosingTime() == nullptr)
        {
			orderEnt->SetClosingTime(system_clock::now());
        }
    }

    if (order.find("amount") == order.end()
        || order.find("price") == order.end()
        || order.find("deal_amount") == order.end()
        || order.find("avg_price") == order.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    assert(orderEnt->GetCoinNumber() == CoinNumber(order["amount"].get<double>()));
    assert(orderEnt->GetLimitedPrice() == Money(order["price"].get<double>()));

    CoinNumber filledCoinNumber(order["deal_amount"].get<double>());
    Money concludedPrice(order["avg_price"].get<double>());

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

size_t OcdcPlatform::HandleHttpUnknownOrderFetchString(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, shared_ptr<OrderEntity> orderEnt)
{
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
        assert(o.find("error_code") != o.end());
        uint32_t ocdcCode = (uint32_t)o["error_code"].get<int64_t>();
        uint32_t errCode = ConvertToErrCode(ocdcCode);
		LOG4_WARN("failed, ocdc code=" << ocdcCode << ", error string=" << ConvertErrCodeToString(errCode));
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, errCode);
        return (nmemb * size);
    }

    set<PlatformOrderId> ptmOrderIds;
    list<lazy_shared_ptr<OrderEntity>>& orderEnts = orderEnt->GetAccount()->GetOrders();
    for (auto i = orderEnts.rbegin(); i != orderEnts.rend() && ptmOrderIds.size() < 20; ++i)
    {
        if (i->object_id() == orderEnt->GetId())
            continue;

        ptmOrderIds.insert(i->load()->GetPlatformOrderId());
        i->unload();
    }

    EnumHelperInterface& enumHelper = EnumHelperInterface::GetInstance();
    resultCode = MakeResultCode(SBitNotOk, FBitFalse, OptResultNoMatchedOrderId);
    if (o.find("orders") == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }
    picojson::array& orders = o["orders"].get<picojson::array>();
    for (auto i = orders.begin(); i != orders.end(); ++i)
    {
        picojson::object& order = i->get<picojson::object>();

        if (order.find("order_id") == order.end()
            || order.find("type") == order.end()
            || order.find("price") == order.end()
            || order.find("amount") == order.end())
        {
            LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
            resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
            return (nmemb * size);
        }

        PlatformOrderId ptmOrderId = (PlatformOrderId)order["order_id"].get<int64_t>();
        if (ptmOrderIds.find(ptmOrderId) == ptmOrderIds.end())
        {
            assert(enumHelper.GetOrderType(order["type"].get<string>().c_str()) == orderEnt->GetOrderType());
            assert(Money(order["price"].get<double>()) == orderEnt->GetLimitedPrice());
            assert(CoinNumber(order["amount"].get<double>()) == orderEnt->GetCoinNumber());

            orderEnt->SetPlatformOrderId(ptmOrderId);
            resultCode = ResultCodeOk;
            break;
        }
    }

    return (nmemb * size);
}

size_t OcdcPlatform::HandleHttpBalanceFetchString1(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, Money& balance)
{
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
        assert(o.find("error_code") != o.end());
        uint32_t ocdcCode = (uint32_t)o["error_code"].get<int64_t>();
        uint32_t errCode = ConvertToErrCode(ocdcCode);
        LOG4_WARN("failed, ocdc code=" << ocdcCode << ", error string=" << ConvertErrCodeToString(errCode));

        resultCode = MakeResultCode(SBitNotOk, FBitTrue, ConvertToErrCode(ocdcCode));
        return (nmemb * size);
    }

    if (o.find("info") == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    auto info = o["info"].get<picojson::object>();
    if (info.find("funds") == info.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    auto funds = info["funds"].get<picojson::object>();
    if (funds.find("free") == funds.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    auto fre = funds["free"].get<picojson::object>();

    resultCode = ResultCodeOk;
    balance = Money(strtod(fre["cny"].get<string>().c_str(), nullptr));
    return (nmemb * size);
}

size_t OcdcPlatform::HandleHttpBalanceFetchString2(void *ptr, size_t size, size_t nmemb,
    ResultCode& resultCode, CoinType coinType, CoinNumber& balance)
{
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
        assert(o.find("error_code") != o.end());
        uint32_t ocdcCode = (uint32_t)o["error_code"].get<int64_t>();
        uint32_t errCode = ConvertToErrCode(ocdcCode);
        LOG4_WARN("failed, ocdc code=" << ocdcCode << ", error string=" << ConvertErrCodeToString(errCode));

        resultCode = MakeResultCode(SBitNotOk, FBitTrue, ConvertToErrCode(ocdcCode));
        return (nmemb * size);
    }

    if (o.find("info") == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    auto info = o["info"].get<picojson::object>();
    if (info.find("funds") == info.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    auto funds = info["funds"].get<picojson::object>();
    if (funds.find("free") == funds.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    auto fre = funds["free"].get<picojson::object>();
    const char *name = EnumHelperInterface::GetInstance().GetName(coinType);
    if (fre.find(name) == fre.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed);
        return (nmemb * size);
    }

    resultCode = ResultCodeOk;
    balance = CoinNumber(strtod(fre[name].get<string>().c_str(), nullptr));
    return (nmemb * size);
}

/*
错误代码	详细描述
10000	必选参数不能为空
10001	用户请求过于频繁
10002	系统错误
10003	未在请求限制列表中,稍后请重试
10004	IP限制不能请求该资源
10005	密钥不存在
10006	用户不存在
10007	签名不匹配
10008	非法参数
10009	订单不存在
10010	余额不足
10011	买卖的数量小于BTC/LTC最小买卖额度
10012	当前网站暂时只支持btc_cny ltc_cny
10013	此接口只支持https请求
10014	下单价格不得≤0或≥1000000
10015	下单价格与最新成交价偏差过大
10016	币数量不足
10017	API鉴权失败
10018	借入不能小于最低限额[cny:100,btc:0.1,ltc:1]
10019	页面没有同意借贷协议
10020	费率不能大于1%
10021	费率不能小于0.01%
10023	获取最新成交价错误
10024	可借金额不足
10025	额度已满，暂时无法借款
10026	借款(含预约借款)及保证金部分不能提出
10027	修改敏感提币验证信息，24小时内不允许提现
10028	提币金额已超过今日提币限额
10029	账户有借款，请撤消借款或者还清借款后提币
10031	存在BTC/LTC充值，该部分等值金额需6个网络确认后方能提出
10032	未绑定手机或谷歌验证
10033	服务费大于最大网络手续费
10034	服务费小于最低网络手续费
10035	可用BTC/LTC不足
10036	提币数量小于最小提币数量
10037	交易密码未设置
10040	取消提币失败
10041	提币地址不存在或者未认证
10042	交易密码错误
10043	合约权益错误，提币失败
10044	取消借款失败
10047	当前为子账户，此功能未开放
10048	提币信息不存在
10049	小额委托（<0.15BTC)的未成交委托数量不得大于50个
10050	重复撤单
10100	账户被冻结
10101	订单类型错误
10102	不是本用户的订单
10103	私密订单密钥错误
10216	非开放API
503	    用户请求过于频繁(Http)
*/
uint32_t OcdcPlatform::ConvertToErrCode(uint32_t ocdcCode)
{
    static map<uint32_t, uint32_t> ocdcCodes =
    {
        { 503, OptResultOptTooFrequent },
        { 10002, OptResulOcdcSystemError },   /* 10002 系统错误 */
        { 10009, OptResultNoMatchedOrderId }, /* 10009 订单不存在 */
        { 10010, OptResultMoneyNotEnough },   /* 10010 余额不足 */
        { 10016, OptResultCoinNotEnough },    /* 10016 币数量不足 */
        { 10050, OptResultCancelOptRepeated } /* 10050 重复撤单*/
    };

    uint32_t errCode;
    map<uint_t, uint_t>::iterator iter = ocdcCodes.find(ocdcCode);
    if (iter == ocdcCodes.end())
    {
        errCode = OptResultUnknownPtmError;
    }
    else
    {
        errCode = iter->second;
    }

    return errCode;
}

#endif //#ifdef HasOkCoinDotCnPlatform