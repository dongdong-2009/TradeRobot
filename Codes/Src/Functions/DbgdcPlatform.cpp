#include "DbgdcPlatform.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Md5HmacSha256.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Functions */
#include "Include/Functions/MarketPollerInterface.h"
#include "Include/Functions/LogHelperInterface.h"

#ifdef HasDebugDotComPlatform

/* local header files */
using namespace std;
using namespace std::placeholders;

typedef ClassFactoriesRegistor<PlatformInterface, std::string, std::shared_ptr<PlatformEntity>> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, "debug.btctrade.com", DbgdcPlatform::CreateInstance1);
RegisterClassFactory(ClassFactoriesType, reg, "debug.okcoin.cn", DbgdcPlatform::CreateInstance2);

/**********************class DbgdcPlatform**********************/
DbgdcPlatform::DbgdcPlatform()
{}

DbgdcPlatform::~DbgdcPlatform()
{}

PlatformInterface* DbgdcPlatform::CreateInstance1(std::shared_ptr<PlatformEntity> ptmEnt)
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation. */
    static DbgdcPlatform instance;
    assert(instance.GetPlatformEntity() == nullptr || instance.GetPlatformEntity()->GetId() == ptmEnt->GetId());
    instance.SetPlatformEntity(ptmEnt);
	return &instance;
}

PlatformInterface* DbgdcPlatform::CreateInstance2(std::shared_ptr<PlatformEntity> ptmEnt)
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation. */
    static DbgdcPlatform instance;
    assert(instance.GetPlatformEntity() == nullptr || instance.GetPlatformEntity()->GetId() == ptmEnt->GetId());
    instance.SetPlatformEntity(ptmEnt);
    return &instance;
}

Money DbgdcPlatform::GetRoundedPrice(Money price, CoinType coinType) const
{
    std::map<CoinType, int> precisions =
    {
        { CoinType::Btc, 2 }, { CoinType::Eth, 2 }, { CoinType::Ltc, 2 }
    };
    auto iter = precisions.find(coinType);
    assert(iter != precisions.end());

    return price.Round(iter->second);
}

/* pravite functions */
CURL* DbgdcPlatform::CreateCurlInstance(const char *url, std::chrono::seconds timeout)
{
    return nullptr;
}

bool DbgdcPlatform::CreateBuyOrderImpl(shared_ptr<OrderEntity> orderEnt)
{
    orderEnt->SetPlatformOrderId(10000 + orderEnt->GetId());

    InsertOrder(orderEnt);
    return true;
}

bool DbgdcPlatform::CreateSellOrderImpl(shared_ptr<OrderEntity> orderEnt)
{
    orderEnt->SetPlatformOrderId(10000 + orderEnt->GetId());

    InsertOrder(orderEnt);
    return true;
}

bool DbgdcPlatform::CancelOrderImpl(shared_ptr<OrderEntity> orderEnt)
{
    orderEnt->SetClosingTime(system_clock::now());
    return true;
}

bool DbgdcPlatform::FetchOrderImpl(std::shared_ptr<OrderEntity> orderEnt)
{
    auto cmp = [](shared_ptr<OrderEntity> l, shared_ptr<OrderEntity> r)->bool
    {
        return l->GetId() == r->GetId();
    };

    auto iter = find_if(orderEnts.begin(), orderEnts.end(), bind(cmp, orderEnt, _1));
    assert(iter != orderEnts.end());

    orderEnt->SetFilledCoinNumber((*iter)->GetFilledCoinNumber());
    if ((*iter)->GetFilledCoinNumber() != CoinNumber::Zero())
    {
        orderEnt->SetConcludedPrice((*iter)->GetConcludedPrice());
    }

    if ((*iter)->GetClosingTime() != nullptr)
    {
        orderEnt->SetClosingTime(*(*iter)->GetClosingTime());
    }

    if ((*iter)->GetCanceledReason() != nullptr)
    {
        orderEnt->SetCanceledReason(*(*iter)->GetCanceledReason());
    }

    lock_guard<mutex> lock(mtx);
    orderStatus[(*iter)->GetId()] = true;
    cv.notify_one();

    return true;
}

bool DbgdcPlatform::FetchUnknownOrderImpl(std::shared_ptr<OrderEntity> orderEnt)
{
    return true;
}

bool DbgdcPlatform::FetchBalanceImpl1(std::shared_ptr<AccountEntity> accEnt, Money& balance)
{
    balance = Money(100.0);
    return true;
}

bool DbgdcPlatform::FetchBalanceImpl2(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance)
{
    balance = CoinNumber(1.0);
    return true;
}

void DbgdcPlatform::ClearOrders()
{
    orderEnts.clear();
}

void DbgdcPlatform::SetMarket(CoinType coinType, Money bidPrice, Money askPrice, Money lastPrice)
{
    for (int i = 0; i < 5; ++i)
    {
        if (HandleMarketInfor(coinType, bidPrice, askPrice, lastPrice))
            break;

        this_thread::sleep_for(milliseconds(100));
    }
}

void DbgdcPlatform::SetOrder(TableId id)
{
    auto cmp = [](TableId id, shared_ptr<OrderEntity> r)->bool
    {
        return id == r->GetId();
    };

    unique_lock<mutex> lock(mtx);
    auto iter = find_if(orderEnts.begin(), orderEnts.end(), bind(cmp, id, _1));
    assert(iter != orderEnts.end());

    orderStatus[id] = false;

    /* wait for the order been featched */
    while (orderStatus[id] == false)
    {
        cv.wait(lock);
    }
}

void DbgdcPlatform::SetOrder(TableId id, Money concludedPrice)
{
    auto cmp = [](TableId id, shared_ptr<OrderEntity> r)->bool
    {
        return id == r->GetId();
    };

    unique_lock<mutex> lock(mtx);
    auto iter = find_if(orderEnts.begin(), orderEnts.end(), bind(cmp, id, _1));
    assert(iter != orderEnts.end());

    (*iter)->SetFilledCoinNumber((*iter)->GetCoinNumber());
    (*iter)->SetConcludedPrice(concludedPrice);
    (*iter)->SetClosingTime(chrono::system_clock::now());

    orderStatus[id] = false;

    /* wait for the order been featched */
    while (orderStatus[id] == false)
    {
        cv.wait(lock);
    }
}

void DbgdcPlatform::InsertOrder(shared_ptr<OrderEntity> orderEnt)
{
    auto cmp = [](shared_ptr<OrderEntity> l, shared_ptr<OrderEntity> r)->bool
    {
        return l->GetId() == r->GetId();
    };

    lock_guard<mutex> lock(mtx);
    auto iter = find_if(orderEnts.begin(), orderEnts.end(), bind(cmp, orderEnt, _1));
    assert(iter == orderEnts.end());

    shared_ptr<OrderEntity> clone = orderEnt->Clone();
    orderEnts.push_back(clone);
    orderStatus[clone->GetId()] = false;
    cv.notify_one();
}

#endif