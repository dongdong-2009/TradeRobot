#include "BtdcNonceHelper.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"
#include "Include/Entity/EntityQueryType.h"

#ifdef HasBtcTradeDotComPlatform

using namespace std;

/**********************class BtdcNonceHelperInterface**********************/
BtdcNonceHelperInterface& BtdcNonceHelperInterface::GetInstance()
{
	/* In C++11, the following is guaranteed to perform thread-safe initialisation: */
	static BtdcNonceHelper instance;
	return instance;
}

/**********************class BtdcNonceHelper**********************/
BtdcNonceHelper::BtdcNonceHelper()
{}

BtdcNonceHelper::~BtdcNonceHelper()
{}

Nonce BtdcNonceHelper::GetNonce(TableId accountId)
{
    lock_guard<mutex> lock(mtx);
    uint64_t tm = time(nullptr);

    auto iter = accIds.find(accountId);
    if (iter == accIds.end())
    {
        accIds.insert(make_pair(accountId, make_pair(tm, 1)));
        return ((tm << 32) | 1);
    }

    if (iter->second.first == tm)
    {
        ++iter->second.second;
    }
    else
    {
        iter->second.first = tm;
        iter->second.second = 1;
    }

    return ((tm << 32) | iter->second.second);
}

#endif //#ifdef HasBtcTradePlatform