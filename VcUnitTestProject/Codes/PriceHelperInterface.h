#ifndef _PriceHelperInterface_h_
#define _PriceHelperInterface_h_

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/ClassFactories.h"

/**********************class PriceHelperInterface**********************/
class PriceHelperInterface
{
public:
    PriceHelperInterface() {};
    virtual ~PriceHelperInterface() {};

    virtual Money GetLowestPrice(CoinType coinType) const = 0;
    virtual Money GetHighestPrice(CoinType coinType) const = 0;
};

/**********************class ControllerInterface Factory**********************/
inline PriceHelperInterface& GetPriceHelperInterface()
{
    typedef ClassFactories<PriceHelperInterface, int> ClassFactoriesType;
    ClassFactoriesType& instance = ClassFactoriesType::GetInstance();

    return *instance.CreateInstance(0);
}

#endif