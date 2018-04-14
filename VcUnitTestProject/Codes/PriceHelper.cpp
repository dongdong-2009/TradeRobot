#include "PriceHelper.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Debug.h"

using namespace std;

typedef ClassFactoriesRegistor<PriceHelperInterface, int> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, 0, PriceHelper::GetInstance);

/**********************class PriceHelper**********************/
/* public function */
PriceHelper::PriceHelper()
{}

PriceHelper::~PriceHelper()
{}

PriceHelperInterface* PriceHelper::GetInstance()
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation. */
    static PriceHelper instance;
    return &instance;
}

Money PriceHelper::GetLowestPrice(CoinType coinType) const
{
    static map<CoinType, Money> prices = 
    {
        { CoinType::Btc, Money(100.0) },
        { CoinType::Eth, Money(50.0) }
    };

    auto iter = prices.find(coinType);
    assert(iter != prices.end());
    return iter->second;
}

Money PriceHelper::GetHighestPrice(CoinType coinType) const
{
    static map<CoinType, Money> prices =
    {
        { CoinType::Btc, Money(99999.0) },
        { CoinType::Eth, Money(99999.0) }
    };

    auto iter = prices.find(coinType);
    assert(iter != prices.end());
    return iter->second;
}