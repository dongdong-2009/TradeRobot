#ifndef _PlatformInterface_h_
#define _PlatformInterface_h_

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/ClassFactories.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

using std::chrono::seconds;
using std::chrono::milliseconds;


/**********************class PlatformInterface**********************/
class PlatformInterface
{
public:
    typedef std::function<void(std::shared_ptr<MarketEntity>)> MarketHandler;

    PlatformInterface(){};
    virtual ~PlatformInterface() {};

    /* platform entity */
    virtual std::shared_ptr<PlatformEntity> GetPlatformEntity() = 0;
    virtual void SetPlatformEntity(std::shared_ptr<PlatformEntity> ptmEnt) = 0;

    /* platform configuration */
    virtual const std::string& GetPlatformName() const = 0;

    virtual milliseconds GetMinMarketQueryInterval() = 0;
    virtual void SetMinMarketQueryInterval(milliseconds minMarketQueryInterval) = 0;

    virtual milliseconds GetMinOrderQueryInterval() = 0;
    virtual void SetMinOrderQueryInterval(milliseconds minOrderQueryInterval) = 0;

    /* coin rule configurations */
    virtual CoinNumber GetCoinMinTradingUnit(CoinType coinType) const = 0;

    /* platform functions */
    virtual Money GetRoundedPrice(Money price, CoinType coinType) const = 0;
    virtual bool CreateOrder(std::shared_ptr<OrderEntity> orderEnt) = 0;
    /* btctrade.com do not need the CoinType parameter, but okcoin.cn need the CoinType parameter. */
    virtual void CancelOrder(std::shared_ptr<OrderEntity> orderEnt) = 0;

    /* btctrade.com do not need the CoinType parameter, but okcoin.cn need the CoinType parameter. */
    virtual void FetchOrder(std::shared_ptr<OrderEntity> orderEnt) = 0;

    virtual bool FetchUnknownOrder(std::shared_ptr<OrderEntity> orderEnt) = 0;

    virtual void FetchBalance(std::shared_ptr<AccountEntity> accEnt, Money& balance) = 0;
    virtual void FetchBalance(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance) = 0;

    virtual void RegisterMarketHandler(TableId robotId, CoinType coinType, MarketHandler handler) = 0;
    virtual void DeregisterMarketHandler(TableId robotId, CoinType coinType) = 0;

    virtual void UpdateAvailableStep0(shared_ptr<OrderEntity> orderEnt) = 0;
    virtual void UpdateAvailableStep1(shared_ptr<OrderEntity> orderEnt) = 0;

    virtual void Save() = 0;

#ifdef UnitTestMode
    virtual void ClearOrders() {}
    virtual void SetMarket(CoinType coinType, Money bidPrice, Money askPrice, Money lastPrice) {}
    virtual void SetOrder(TableId id) {}
    virtual void SetOrder(TableId id, Money concludedPrice) {}
#endif
};

/**********************class PlatformInterface Factory**********************/
template<typename ... Types>
inline PlatformInterface& GetPlatformInstance(std::shared_ptr<PlatformEntity> ptmEnt)
{
    typedef ClassFactories<PlatformInterface, std::string, std::shared_ptr<PlatformEntity>> ClassFactoriesType;
    ClassFactoriesType& instance = ClassFactoriesType::GetInstance();

    return *instance.CreateInstance(ptmEnt->GetName(), ptmEnt);
}

#endif