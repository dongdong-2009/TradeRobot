#ifndef _OcdcMarketPoller_h_
#define _OcdcMarketPoller_h_

/* Foundation */
#include "Include/Module/Module.h"
#ifdef HasOkCoinDotCnPlatform

#include "picojson/picojson.h"

/* curl and json */
#ifdef __CYGWIN__
#   include <sys/select.h>
#endif
#include <curl/curl.h>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* local header files */
#include "MarketPollerBase.h"

/**********************class OcdcMarketPoller**********************/
class OcdcMarketPoller : public MarketPollerBase
{
public:
    OcdcMarketPoller(milliseconds interval, CoinType coinType, HttpMarketHandler marketHandler);
    ~OcdcMarketPoller();

    static MarketPollerInterface* CreateInstance(milliseconds interval,
        CoinType coinType, HttpMarketHandler marketHandler);

protected:
    void QueryMarketInfoImpl();
    CURL* CreateCurlInstance();

private:
    size_t HandleHttpMarketString(void *ptr, size_t size, size_t nmemb, ResultCode& resultCode);
    uint32_t ConvertToErrCode(uint32_t ocdcCode);
    const char* ConvertToErrString(uint32_t ocdcCode);

private:
    CoinType coinType;
    HttpMarketHandler marketHandler;

    CURL *curl;
};

#endif //#ifdef HasOkCoinPlatform
#endif