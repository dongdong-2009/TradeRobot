#include "DbgdcMarketPoller.h"

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
#include "Include/Functions/LogHelperInterface.h"
#include "Include/Functions/TableIndexHelperInterface.h"

#ifdef HasDebugDotComPlatform

using namespace std;
using namespace placeholders;

typedef ClassFactoriesRegistor<MarketPollerInterface, std::string, milliseconds, CoinType, HttpMarketHandler> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, "debug.btctrade.com", DbgdcMarketPoller::CreateInstance);
RegisterClassFactory(ClassFactoriesType, reg, "debug.okcoin.cn", DbgdcMarketPoller::CreateInstance);

/**********************class DbgdcMarketPoller**********************/
/* public function */
DbgdcMarketPoller::DbgdcMarketPoller(milliseconds interval, CoinType coinType, HttpMarketHandler marketHandler)
    : MarketPollerBase(interval), coinType(coinType), marketHandler(marketHandler)
{}

DbgdcMarketPoller::~DbgdcMarketPoller()
{}

MarketPollerInterface* DbgdcMarketPoller::CreateInstance(milliseconds interval,
    CoinType coinType, HttpMarketHandler marketHandler)
{
    return new DbgdcMarketPoller(interval, coinType, marketHandler);
}

/* private function */
void DbgdcMarketPoller::QueryMarketInfoImpl()
{}

CURL* DbgdcMarketPoller::CreateCurlInstance()
{
    return nullptr;
}

#endif