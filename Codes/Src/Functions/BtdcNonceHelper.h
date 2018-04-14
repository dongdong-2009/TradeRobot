#ifndef _BtccNonceHelper_h_
#define _BtccNonceHelper_h_

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"  /* std::shared_ptr */
#include "Include/Module/Module.h"
#include "Include/Foundation/ClassFactories.h"

using std::shared_ptr;
namespace odb
{
    class database;
};

#ifdef HasBtcTradeDotComPlatform

/**********************class BtdcNonceHelperInterface**********************/
class BtdcNonceHelperInterface
{
public:
    BtdcNonceHelperInterface() {}
    virtual ~BtdcNonceHelperInterface() {}

    static BtdcNonceHelperInterface& GetInstance();

    virtual Nonce GetNonce(TableId accountId) = 0;
};

/**********************class BtdcNonceHelper**********************/
class BtdcNonceHelper : public BtdcNonceHelperInterface
{
public:
    BtdcNonceHelper();
    ~BtdcNonceHelper();

    Nonce GetNonce(TableId accountId);

private:
    std::mutex mtx;
    std::map<TableId, std::pair<uint64_t, uint32_t>> accIds;
};

#endif //#ifdef HasBtcTradePlatform
#endif