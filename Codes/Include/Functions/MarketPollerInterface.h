#ifndef _MarketPollerInterface_h_
#define _MarketPollerInterface_h_

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/ClassFactories.h"

typedef std::function<bool(CoinType coinType, Money bidPrice, Money askPrice, Money lastPrice)> HttpMarketHandler;

/**********************class MarketPollerInterface**********************/
class MarketPollerInterface
{
public:
    MarketPollerInterface() {}
    virtual ~MarketPollerInterface() {}

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() = 0;
};

/**********************class MarketPollerInterface Factory**********************/
template<typename ... Types>
MarketPollerInterface* CreateMarketPollerInterface(const char *platformName, Types ... args)
{
    typedef ClassFactories<MarketPollerInterface, std::string, Types ...> ClassFactoriesType;
    ClassFactoriesType& instance = ClassFactoriesType::GetInstance();

    return instance.CreateInstance(platformName, args ...);
}

#endif