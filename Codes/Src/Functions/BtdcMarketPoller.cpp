#include "BtdcMarketPoller.h"

/* C++ */
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/ClassFactories.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Md5HmacSha256.h"
#include "Include/Foundation/Debug.h"

/* Functions */
#include "Include/Functions/TableIndexHelperInterface.h"
#include "Include/Functions/EnumHelperInterface.h"
#include "Include/Functions/LogHelperInterface.h"

/* local header files */
#include "CurlOutputHandler.h"

#ifdef HasBtcTradeDotComPlatform

#define CurlMarcketQueryTimeoutSecs  2

using namespace std;
using namespace std::placeholders;

typedef ClassFactoriesRegistor<MarketPollerInterface, std::string, milliseconds, CoinType, HttpMarketHandler> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, "btctrade.com", BtdcMarketPoller::CreateInstance);

/**********************class BtdcMarketPoller**********************/
/* public function */
BtdcMarketPoller::BtdcMarketPoller(milliseconds interval, CoinType coinType, HttpMarketHandler marketHandler)
: MarketPollerBase(interval), coinType(coinType), marketHandler(marketHandler)
{
    curl = CreateCurlInstance();
}

BtdcMarketPoller::~BtdcMarketPoller()
{
    curl_easy_cleanup(curl);
}

MarketPollerInterface* BtdcMarketPoller::CreateInstance(milliseconds interval,
    CoinType coinType, HttpMarketHandler marketHandler)
{
    return new BtdcMarketPoller(interval, coinType, marketHandler);
}

/* private function */
void BtdcMarketPoller::QueryMarketInfoImpl()
{
    HttpStringHandlerX handlerX = std::bind(&BtdcMarketPoller::HandleHttpMarketString, this, _1, _2, _3, _4);

    ResultCode resultCode = ExecuteHttpCommand(curl, handlerX);
    if (GetResultSBit(resultCode) != SBitOk)
    {
        uint32_t errCode = GetResultErrCode(resultCode);
        LOG4_WARN("failed, err code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")");
    }
}

CURL* BtdcMarketPoller::CreateCurlInstance()
{
    ostringstream os;
    os << "http://api.btctrade.com/api/ticker?coin=" << EnumHelperInterface::GetInstance().GetName(coinType);

    CURL *curl = curl_easy_init();
    assert(curl != nullptr);

    /* curl will copy string for option "CURLOPT_URL", refer option "CURLOPT_POSTFIELDS" for detail. */
    curl_easy_setopt(curl, CURLOPT_URL, os.str().c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CurlMarcketQueryTimeoutSecs);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, CurlMarcketQueryTimeoutSecs);

    //parameter for callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlOutputHandler);
    /* reference material:
    1 https://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
    2 http://blog.chinaunix.net/uid-23145525-id-4343403.html
    */
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

    return curl;
}

size_t BtdcMarketPoller::HandleHttpMarketString(void *ptr, size_t size, size_t nmemb, ResultCode& resultCode)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    picojson::value value;
    std::string err;
    picojson::parse(value, (char*)ptr, (char*)ptr + nmemb*size, &err);
    CheckJsonResult(err);
    picojson::object& o = value.get<picojson::object>();

    LOG4_DEBUG((char*)ptr);
    picojson::object::iterator result = o.find("result");
    if (result != o.end() && !result->second.get<bool>())
    {
        string message = o["message"].get<string>();
        LOG4_WARN("failed,  message=" << ConvertUtf8ToString(message.c_str()));

        resultCode = MakeResultCode(SBitNotOk, FBitFalse, ConvertToErrCode(message));
        return (nmemb * size);
    }

    if (o.find("buy") == o.end()  || o.find("sell") == o.end() || o.find("last") == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitFalse, OptResultJsonFailed);
        return (nmemb * size);
    }

    resultCode = ResultCodeOk;
    marketHandler(coinType, Money(o["buy"].get<double>()),
        Money(o["sell"].get<double>()),
        Money(o["last"].get<double>()));

    return size*nmemb;
}

uint32_t BtdcMarketPoller::ConvertToErrCode(const string& msg)
{
    return OptResultUnknownPtmError;
}

#endif //#ifdef HasBtcTradePlatform
