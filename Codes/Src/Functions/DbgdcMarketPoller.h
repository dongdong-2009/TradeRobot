#ifndef _DbgdcMarketPoller_h_
#define _DbgdcMarketPoller_h_

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"

/* local header files */
#include "MarketPollerBase.h"

using std::chrono::seconds;
using std::chrono::milliseconds;

#ifdef HasDebugDotComPlatform

/**********************class DbgdcMarketPoller**********************/
class DbgdcMarketPoller : public MarketPollerBase
{
public:
    DbgdcMarketPoller(milliseconds interval, CoinType coinType, HttpMarketHandler marketHandler);
    ~DbgdcMarketPoller();

    static MarketPollerInterface* CreateInstance(milliseconds interval,
        CoinType coinType, HttpMarketHandler marketHandler);

private:
    void QueryMarketInfoImpl();
    CURL* CreateCurlInstance();

private:
    CoinType coinType;
    HttpMarketHandler marketHandler;
};

#endif //#ifdef HasBtcTradePlatform
#endif