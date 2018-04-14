#ifndef _BtdcMarketPoller_h_
#define _BtdcMarketPoller_h_

/* Foundation */
#include "Include/Module/Module.h"
#ifdef HasBtcTradeDotComPlatform

#include "picojson/picojson.h"

/* curl */
#ifdef __CYGWIN__
#   include <sys/select.h>
#endif
#include <curl/curl.h>

#include "Include/Foundation/SystemInclude.h"

/* local header files */
#include "MarketPollerBase.h"

/**********************class BtdcMarketPoller**********************/
class BtdcMarketPoller : public MarketPollerBase
{
public:
    BtdcMarketPoller(milliseconds interval, CoinType coinType, HttpMarketHandler marketHandler);
    ~BtdcMarketPoller();

    static MarketPollerInterface* CreateInstance(milliseconds interval,
        CoinType coinType, HttpMarketHandler marketHandler);

protected:
    void QueryMarketInfoImpl();
    CURL* CreateCurlInstance();

private:
    size_t HandleHttpMarketString(void *ptr, size_t size, size_t nmemb, ResultCode& resultCode);
    uint32_t ConvertToErrCode(const std::string& msg);

private:
    CoinType coinType;
    HttpMarketHandler marketHandler;

    CURL *curl;
};

#endif //#ifdef HasBtcTradePlatform
#endif
