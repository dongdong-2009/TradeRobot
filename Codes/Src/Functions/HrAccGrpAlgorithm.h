#ifndef _HrAccGrpAlgorithm_h_
#define _HrAccGrpAlgorithm_h_

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/StateMachine.h"

/* Entity */
#include "Include/Entity/Entities.h"

using std::shared_ptr;
using std::list;

class MarketEntity;
class HrEntity;
class HrAccountGroupEntity;
class HrAccountEntity;
class PlatformEntity;

class HrAccGrpAlgorithmInterface;

enum class HrAccGrpEvent
{
    StartUp,              /* 0 */
    BuyOrderCreated,      /* 1 */
    SellOrderCreated,     /* 2 */
    BuyOrderCancled,      /* 3 */
    SellOrderCancled,     /* 4 */
    BuyOrderFilled,       /* 5 */
    SellOrderFilled,      /* 6 */
    Order2ndTimeOut,      /* 7 */
    StopLossOrderCreated, /* 8 */
    StopLossOrderClosed   /* 9 */
};

/**********************class HrAccGrpAlgorithmInterface**********************/
class HrAccGrpAlgorithmInterface
{
public:
    HrAccGrpAlgorithmInterface() {};
    virtual ~HrAccGrpAlgorithmInterface() {};

    virtual void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt) = 0;
    virtual void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt) = 0;
};

namespace HrAccGrpAlgorithmNs
{
    class HrAccGrpAlgorithmBase;
    class NoOrder;
    class Step0Buying;
    class Step1Buying;
    class Step0Selling;
    class Step1Selling;
};

/**********************class HrAccGrpAlgorithm**********************/
class HrAccGrpAlgorithm : public HrAccGrpAlgorithmInterface,
                          public AutomatonBase<HrAccGrpAlgorithmInterface, HrAccGrpEvent>
{
public:
    friend class HrAccGrpAlgorithmNs::HrAccGrpAlgorithmBase;
    friend class HrAccGrpAlgorithmNs::NoOrder;
    friend class HrAccGrpAlgorithmNs::Step0Buying;
    friend class HrAccGrpAlgorithmNs::Step1Buying;
    friend class HrAccGrpAlgorithmNs::Step0Selling;
    friend class HrAccGrpAlgorithmNs::Step1Selling;

    HrAccGrpAlgorithm(shared_ptr<HrAccountGroupEntity> hrAccGrpEnt);
    ~HrAccGrpAlgorithm();

    void CastEvent(HrAccGrpEvent e, const char *func, int line);

    void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt);
    void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt);

private:
    shared_ptr<HrEntity> hrEnt;
    shared_ptr<HrAccountGroupEntity> hrAccGrpEnt;
    shared_ptr<HrAccountEntity> hrAccEnt0;  //hrAccGrpEnt->GetHrAccount0();
    shared_ptr<HrAccountEntity> hrAccEnt1;  //hrAccGrpEnt->GetHrAccount1();
    shared_ptr<PlatformEntity> ptmEnt0;
    shared_ptr<PlatformEntity> ptmEnt1;
    shared_ptr<PlatformCoinRuleEntity> ptm0CoinRuleEnt;
    shared_ptr<PlatformCoinRuleEntity> ptm1CoinRuleEnt;

    list<shared_ptr<HrOrderEntity>> openedOrders;
    /* 通过 shared_ptr<HrAccGrpAlgorithmInterface> 查找当前状态的text, 仅用于单元测试. */
    std::map<shared_ptr<HrAccGrpAlgorithmInterface>, const char*> algorithms;
};

CxxBeginNameSpace(HrAccGrpAlgorithmNs)
/**********************class HrAccGrpAlgorithmBase**********************/
class HrAccGrpAlgorithmBase : public StateBase<HrAccGrpAlgorithm, HrAccGrpEvent>,
                              public HrAccGrpAlgorithmInterface
{
public:
    HrAccGrpAlgorithmBase(HrAccGrpAlgorithm& automaton);
    virtual ~HrAccGrpAlgorithmBase();

public:
    virtual void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt);
    virtual void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt);

protected:
    Rate GetMargin(HrPlatformPairEntity& hrPtmPairEnt);
    Money GetBuyPrice(const MarketEntity& marketEnt, HrNs::OderPricingMode opm);
    Money GetSellPrice(const MarketEntity& marketEnt, HrNs::OderPricingMode opm);

    void CreateOrder(shared_ptr<HrAccountEntity> hrAccEnt, shared_ptr<HrOrderEntity> hrOrderEnt);
    void CancelOrder(shared_ptr<HrOrderEntity> hrOrderEnt, OrderCanceledReason orderCanceledReason);
    void FetchOrder(shared_ptr<HrOrderEntity> hrOrderEnt);

    void HandleStopLossOrder(shared_ptr<HrOrderEntity> hrOrderEnt);
};

/**********************class HrAccGrpNoOrderAlgorithm**********************/
class NoOrder : public HrAccGrpAlgorithmBase
{
public:
    NoOrder(HrAccGrpAlgorithm& automaton);
    ~NoOrder();

    void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt);
    void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt);
};

/**********************class Step0Buying**********************/
class Step0Buying : public HrAccGrpAlgorithmBase
{
public:
    Step0Buying(HrAccGrpAlgorithm& automaton);
    ~Step0Buying();

    void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt);
    void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt);
};

/**********************class Step0Bought**********************/
class Step0Bought : public HrAccGrpAlgorithmBase
{
public:
    Step0Bought(HrAccGrpAlgorithm& automaton) : HrAccGrpAlgorithmBase(automaton) {}
    ~Step0Bought() {}

    void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt)
    { assert(false); }

    void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt)
    { assert(false); }
};

/**********************class Step1Buying**********************/
class Step1Buying : public HrAccGrpAlgorithmBase
{
public:
    Step1Buying(HrAccGrpAlgorithm& automaton);
    ~Step1Buying();

    void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt);
    void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt);
};

/**********************class Step0Selling**********************/
class Step0Selling : public HrAccGrpAlgorithmBase
{
public:
    Step0Selling(HrAccGrpAlgorithm& automaton);
    ~Step0Selling();

    void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt);
    void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt);
};

/**********************class Step0Sold**********************/
class Step0Sold : public HrAccGrpAlgorithmBase
{
public:
    Step0Sold(HrAccGrpAlgorithm& automaton) : HrAccGrpAlgorithmBase(automaton) {}
    ~Step0Sold() {}

    void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt)
    {
        assert(false);
    }

    void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt)
    {
        assert(false);
    }
};

/**********************class Step1Selling**********************/
class Step1Selling : public HrAccGrpAlgorithmBase
{
public:
    Step1Selling(HrAccGrpAlgorithm& automaton);
    ~Step1Selling();

    void HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt);
    void HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
        const MarketEntity& ptm0MarketEnt,
        const MarketEntity& ptm1MarketEnt);
};

CxxEndNameSpace
#endif