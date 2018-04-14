#ifndef _PriceHelper_h_
#define _PriceHelper_h_

#include "PriceHelperInterface.h"

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/**********************class Controller**********************/
class PriceHelper : public PriceHelperInterface
{
public:
    PriceHelper();
    ~PriceHelper();
    static PriceHelperInterface* GetInstance();

    Money GetLowestPrice(CoinType coinType) const;
    Money GetHighestPrice(CoinType coinType) const;
};

#endif