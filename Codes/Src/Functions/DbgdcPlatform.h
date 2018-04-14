#ifndef _DbgdcPlatform_h_
#define _DbgdcPlatform_h_

#include "Include/Module/Module.h"
#ifdef HasDebugDotComPlatform

/* Curl and Json */
#ifdef __CYGWIN__
#   include <sys/select.h>
#endif
#include <curl/curl.h>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Fundation */
#include "Include/Foundation/SystemInclude.h"

/* local header files */
#include "PlatformBase.h"        /* PlatformBase */

/**********************class DbgdcPlatform**********************/
class DbgdcPlatform : public PlatformBase
{
public:
	DbgdcPlatform();
    ~DbgdcPlatform();

    /* Static function */
    static PlatformInterface* CreateInstance1(std::shared_ptr<PlatformEntity> ptmEnt);
    static PlatformInterface* CreateInstance2(std::shared_ptr<PlatformEntity> ptmEnt);

    Money GetRoundedPrice(Money price, CoinType coinType) const;

private:
    CURL* CreateCurlInstance(const char *url, std::chrono::seconds timeout);
    bool CreateBuyOrderImpl(std::shared_ptr<OrderEntity> orderEnt);
    bool CreateSellOrderImpl(std::shared_ptr<OrderEntity> orderEnt);
    bool CancelOrderImpl(std::shared_ptr<OrderEntity> orderEnt);
    bool FetchOrderImpl(std::shared_ptr<OrderEntity> orderEnt);
    bool FetchUnknownOrderImpl(std::shared_ptr<OrderEntity> orderEnt);
    bool FetchBalanceImpl1(std::shared_ptr<AccountEntity> accEnt, Money& balance);
    bool FetchBalanceImpl2(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance);

    void ClearOrders();
    void SetMarket(CoinType coinType, Money bidPrice, Money askPrice, Money lastPrice);
    void SetOrder(TableId id);
    void SetOrder(TableId id, Money concludedPrice);
    void InsertOrder(std::shared_ptr<OrderEntity> orderEnt);

private:
    std::mutex mtx;
    std::condition_variable cv;

    std::list<std::shared_ptr<OrderEntity>> orderEnts;
    std::map<TableId, bool> orderStatus;
};

#endif
#endif