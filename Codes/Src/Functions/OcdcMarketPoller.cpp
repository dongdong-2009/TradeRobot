#include "OcdcMarketPoller.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Md5HmacSha256.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Functions */
#include "Include/Functions/EnumHelperInterface.h"
#include "Include/Functions/LogHelperInterface.h"
#include "Include/Functions/TableIndexHelperInterface.h"

/* local header files */
#include "CurlOutputHandler.h"

#ifdef HasOkCoinDotCnPlatform

#define CurlMarcketQueryTimeoutSecs  2

using namespace std;
using namespace std::placeholders;

typedef ClassFactoriesRegistor<MarketPollerInterface, std::string, milliseconds, CoinType, HttpMarketHandler> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, "okcoin.cn", OcdcMarketPoller::CreateInstance);

/**********************class OcdcMarketPoller**********************/
/* public functions */
OcdcMarketPoller::OcdcMarketPoller(milliseconds interval, CoinType coinType, HttpMarketHandler marketHandler)
: MarketPollerBase(interval), coinType(coinType), marketHandler(marketHandler)
{
    curl = CreateCurlInstance();
}

OcdcMarketPoller::~OcdcMarketPoller()
{
    curl_easy_cleanup(curl);
}

MarketPollerInterface* OcdcMarketPoller::CreateInstance(milliseconds interval,
    CoinType coinType, HttpMarketHandler marketHandler)
{
    return new OcdcMarketPoller(interval, coinType, marketHandler);
}

/* protected functions */
void OcdcMarketPoller::QueryMarketInfoImpl()
{
    HttpStringHandlerX handlerX = std::bind(&OcdcMarketPoller::HandleHttpMarketString, this, _1, _2, _3, _4);

    ResultCode resultCode = ExecuteHttpCommand(curl, handlerX);
    if (GetResultSBit(resultCode) != SBitOk)
    {
        uint32_t errCode = GetResultErrCode(resultCode);
        LOG4_WARN("failed, err code=" << errCode << "(" << ConvertErrCodeToString(errCode) << ")");
    }
}

CURL* OcdcMarketPoller::CreateCurlInstance()
{
    ostringstream os;
    os << "https://www.okcoin.cn/api/v1/ticker.do?symbol=" << EnumHelperInterface::GetInstance().GetName(coinType) << "_cny";

    CURL *curl = curl_easy_init();
    assert(curl != nullptr);

    /* curl will copy string for option "CURLOPT_URL", refer option "CURLOPT_POSTFIELDS" for detail */
    curl_easy_setopt(curl, CURLOPT_URL, os.str().c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CurlMarcketQueryTimeoutSecs);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, CurlMarcketQueryTimeoutSecs);

    /* CA verification */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    assert((access("root.pem", 0)) == 0);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "root.pem");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);

    //parameter for callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlOutputHandler);
    /* reference material:
    1 https://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
    2 http://blog.chinaunix.net/uid-23145525-id-4343403.html
    */
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    // 设置301、302跳转跟随location
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

    return curl;
}

/* private functions */
/*
# Response
{
    "date":"1410431279",
    "ticker":
    {
        "buy":"33.15",
        "high":"34.15",
        "last":"33.15",
        "low":"32.05",
        "sell":"33.16",
        "vol":"10532696.39199642"
    }
}

{
"error_code" : 10000,
"result" : false
}
*/
size_t OcdcMarketPoller::HandleHttpMarketString(void *ptr, size_t size, size_t nmemb, ResultCode& resultCode)
{
    picojson::value value;
    std::string err;
    picojson::parse(value, (char*)ptr, (char*)ptr + nmemb*size, &err);
    CheckJsonResult(err);
    picojson::object& o = value.get<picojson::object>();

    LOG4_DEBUG((char*)ptr);
    picojson::object::iterator result = o.find("result");
    if (result != o.end() && !result->second.get<bool>())
    {
        uint32_t ocdcCode = (uint32_t)o["error_code"].get<int64_t>();
        LOG4_WARN("failed, ocdc code=" << ocdcCode << ", msg=" << ConvertToErrString(ocdcCode));

        resultCode = MakeResultCode(SBitNotOk, FBitFalse, ConvertToErrCode(ocdcCode));
        return size*nmemb;
    }

    if (o.find("ticker") == o.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitFalse, OptResultJsonFailed);
        return (nmemb * size);
    }

    resultCode = ResultCodeOk;
    picojson::object& ticker = o["ticker"].get<picojson::object>();
    if (ticker.find("buy") == ticker.end() || ticker.find("sell") == ticker.end() || ticker.find("last") == ticker.end())
    {
        LOG4_WARN("json contents are wrong. json string=" << (char*)ptr);
        resultCode = MakeResultCode(SBitNotOk, FBitFalse, OptResultJsonFailed);
        return (nmemb * size);
    }
    marketHandler(coinType, Money(atof(ticker["buy"].get<string>().c_str())),
        Money(atof(ticker["sell"].get<string>().c_str())),
        Money(atof(ticker["last"].get<string>().c_str())));

    return size*nmemb;
}

uint32_t OcdcMarketPoller::ConvertToErrCode(uint32_t ocdcCode)
{
    return OptResultUnknownPtmError;
}

const char* OcdcMarketPoller::ConvertToErrString(uint32_t ocdcCode)
{
    return "unknown error";
}

#endif //#ifdef HasOkCoinPlatform