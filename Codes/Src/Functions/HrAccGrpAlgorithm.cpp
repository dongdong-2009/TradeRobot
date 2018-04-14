#include "HrAccGrpAlgorithm.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* BaseType */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Functions */
#include "Include/Functions/TableIndexHelperInterface.h"
#include "Include/Functions/PlatformInterface.h"
#include "Include/Functions/EnumHelperInterface.h"
#include "Include/Functions/LogHelperInterface.h"

using namespace std;
using namespace std::placeholders;

using namespace HrNs;
using namespace HrAccGrpAlgorithmNs;

#define MsgIdHrAccGrpAlgorithmBaseHandleStopLossOrder 1
#define MsgIdNoOrderHandleMarketBuy                   2
#define MsgIdNoOrderHandleMarketSell                  3

#define PrintAvailable(accEnt) \
    LOG4_INFO("account id=" << accEnt->GetId() \
        << ", invested money=" << accEnt->GetInvestedMoney() \
        << ", available money=" << accEnt->GetAvailableMoney() \
        << ", invested coin=" << accEnt->GetInvestedCoinNumber() \
        << ", available coin=" << accEnt->GetAvailableCoinNumber())

/**********************class HrAccGrpAlgorithm**********************/
HrAccGrpAlgorithm::HrAccGrpAlgorithm(shared_ptr<HrAccountGroupEntity> hrAccGrpEnt)
    : hrEnt(hrAccGrpEnt->GetRobot()), hrAccGrpEnt(hrAccGrpEnt)
{
    shared_ptr<HrAccGrpAlgorithmInterface> noOrder = make_shared<NoOrder>(*this);
    shared_ptr<HrAccGrpAlgorithmInterface> step0Buying = make_shared<Step0Buying>(*this);
    shared_ptr<HrAccGrpAlgorithmInterface> step0Bought = make_shared<Step0Bought>(*this);
    shared_ptr<HrAccGrpAlgorithmInterface> step0Selling = make_shared<Step0Selling>(*this);
    shared_ptr<HrAccGrpAlgorithmInterface> step0Sold = make_shared<Step0Sold>(*this);
    shared_ptr<HrAccGrpAlgorithmInterface> step1Buying = make_shared<Step1Buying>(*this);
    shared_ptr<HrAccGrpAlgorithmInterface> step1Selling = make_shared<Step1Selling>(*this);

    algorithms.insert(make_pair(noOrder, "NoOrder"));
    algorithms.insert(make_pair(step0Buying, "Step0Buying"));
    algorithms.insert(make_pair(step0Bought, "Step0Bought"));
    algorithms.insert(make_pair(step0Selling, "Step0Selling"));
    algorithms.insert(make_pair(step0Sold, "Step0Sold"));
    algorithms.insert(make_pair(step1Buying, "Step1Buying"));
    algorithms.insert(make_pair(step1Selling, "Step1Selling"));

    /* Start: state machine convert map */
    AddEdge(nullptr, HrAccGrpEvent::StartUp, noOrder);

    AddEdge(noOrder, HrAccGrpEvent::BuyOrderCreated, step0Buying);
    AddEdge(noOrder, HrAccGrpEvent::SellOrderCreated, step0Selling);
    AddEdge(noOrder, HrAccGrpEvent::StopLossOrderCreated, noOrder);
    AddEdge(noOrder, HrAccGrpEvent::StopLossOrderClosed, noOrder);

    AddEdge(step0Buying, HrAccGrpEvent::BuyOrderCancled, noOrder);
    AddEdge(step0Buying, HrAccGrpEvent::BuyOrderFilled, step0Bought);
    AddEdge(step0Buying, HrAccGrpEvent::StopLossOrderCreated, step0Buying);
    AddEdge(step0Buying, HrAccGrpEvent::StopLossOrderClosed, step0Buying);

    AddEdge(step0Bought, HrAccGrpEvent::SellOrderCreated, step1Selling);

    AddEdge(step1Buying, HrAccGrpEvent::Order2ndTimeOut, noOrder);
    AddEdge(step1Buying, HrAccGrpEvent::BuyOrderCancled, noOrder);
    AddEdge(step1Buying, HrAccGrpEvent::BuyOrderFilled, noOrder);
    AddEdge(step1Buying, HrAccGrpEvent::StopLossOrderCreated, step1Buying);
    AddEdge(step1Buying, HrAccGrpEvent::StopLossOrderClosed, step1Buying);

    AddEdge(step0Selling, HrAccGrpEvent::SellOrderCancled, noOrder);
    AddEdge(step0Selling, HrAccGrpEvent::SellOrderFilled, step0Sold);
    AddEdge(step0Selling, HrAccGrpEvent::StopLossOrderCreated, step0Selling);
    AddEdge(step0Selling, HrAccGrpEvent::StopLossOrderClosed, step0Selling);

    AddEdge(step0Sold, HrAccGrpEvent::BuyOrderCreated, step1Buying);

    AddEdge(step1Selling, HrAccGrpEvent::Order2ndTimeOut, noOrder);
	AddEdge(step1Selling, HrAccGrpEvent::SellOrderCancled, noOrder);
    AddEdge(step1Selling, HrAccGrpEvent::SellOrderFilled, noOrder);
    AddEdge(step1Selling, HrAccGrpEvent::StopLossOrderCreated, step1Selling);
    AddEdge(step1Selling, HrAccGrpEvent::StopLossOrderClosed, step1Selling);
    CastEvent(HrAccGrpEvent::StartUp, __func__, __LINE__);
    /* End: state machine convert map */

    hrAccEnt0 = hrAccGrpEnt->GetHrAccount0();
    hrAccEnt1 = hrAccGrpEnt->GetHrAccount1();
    ptmEnt0 = hrAccEnt0->GetPlatform();
    ptmEnt1 = hrAccEnt1->GetPlatform();
    ptm0CoinRuleEnt = ptmEnt0->GetCoinRule(hrEnt->GetCoinType());
    ptm1CoinRuleEnt = ptmEnt1->GetCoinRule(hrEnt->GetCoinType());

    /* 为了避免误差引的错误, 如:本地计算的availabe money 大于platform上计算的availabe money导致platform创建买单失败,
     * 我们在此设置一个误差值
     */
    Money moneyDeviation(1.0);
    CoinNumber coinNumberDeviation(0.001);
    hrAccEnt0->SetAvailableMoney(hrAccEnt0->GetInvestedMoney() - moneyDeviation);
    hrAccEnt0->SetAvailableCoinNumber(hrAccEnt0->GetInvestedCoinNumber() - coinNumberDeviation);
    hrAccEnt1->SetAvailableMoney(hrAccEnt1->GetInvestedMoney() - moneyDeviation);
    hrAccEnt1->SetAvailableCoinNumber(hrAccEnt1->GetInvestedCoinNumber() - coinNumberDeviation);

    auto cmp = [](const lazy_shared_ptr<OrderEntity>& l, const lazy_shared_ptr<OrderEntity>& r)->bool
    {
        return (l.object_id() < r.object_id());
    };
    list<lazy_shared_ptr<OrderEntity>> orderEnts;
    copy(hrAccEnt0->GetOrders().begin(), hrAccEnt0->GetOrders().end(), back_inserter(orderEnts));
    copy(hrAccEnt1->GetOrders().begin(), hrAccEnt1->GetOrders().end(), back_inserter(orderEnts));
    orderEnts.sort(cmp);

    shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
	shared_ptr<odb::session> session = Database::GetInstance().GetSession();
    for (auto ordi = orderEnts.begin(); ordi != orderEnts.end(); ++ordi)
    {
        shared_ptr<HrAccountEntity> hrAccEnt;
        shared_ptr<HrOrderEntity> hrOrderEnt;
		StartTransaction(db, session, MaxTransactionRetryNumber);
        hrOrderEnt = dynamic_pointer_cast<HrOrderEntity>(ordi->load());
        hrAccEnt = dynamic_pointer_cast<HrAccountEntity>(hrOrderEnt->GetAccount());
        EndTransaction();
        LOG4_DEBUG("order id=" << hrOrderEnt->GetId());

        CoinNumber peerMinTradingUnit;
        if (hrOrderEnt->GetAccount()->GetId() == hrAccEnt0->GetId())
            peerMinTradingUnit = ptm1CoinRuleEnt->GetMinTradingUnit();
        else
            peerMinTradingUnit = ptm0CoinRuleEnt->GetMinTradingUnit();

        PlatformInterface& ptm = GetPlatformInstance(hrAccEnt->GetPlatform());
        ptm.UpdateAvailableStep0(hrOrderEnt);
        if (hrOrderEnt->GetClosingTime() == nullptr)
            openedOrders.push_back(hrOrderEnt);
        else
            ptm.UpdateAvailableStep1(hrOrderEnt);

        auto parent = hrOrderEnt->GetParentOrderId();
        if (hrOrderEnt->GetOrderType() == OrderType::Buy)
        {
            CastEvent(parent == nullptr ? HrAccGrpEvent::BuyOrderCreated : HrAccGrpEvent::StopLossOrderCreated,
                      __func__, __LINE__);
            if (hrOrderEnt->GetClosingTime() == nullptr)
            {
                continue;
            }

            if (parent == nullptr)
            {
                if (hrOrderEnt->GetCanceledReason() == nullptr ||
                    (state == step0Buying && hrOrderEnt->GetFilledCoinNumber() >= peerMinTradingUnit))
                {
                    CastEvent(HrAccGrpEvent::BuyOrderFilled, __func__, __LINE__);
                }
                else
                {
                    CastEvent(HrAccGrpEvent::BuyOrderCancled, __func__, __LINE__);
                }
            }
            else
            {
                CastEvent(HrAccGrpEvent::StopLossOrderClosed, __func__, __LINE__);
            }
            ordi->unload();
        }
        else
        {
            CastEvent(parent == nullptr ? HrAccGrpEvent::SellOrderCreated : HrAccGrpEvent::StopLossOrderCreated,
                      __func__, __LINE__);
            if (hrOrderEnt->GetClosingTime() == nullptr)
            {
                continue;
            }

            if (parent == nullptr)
            {
                if (hrOrderEnt->GetCanceledReason() == nullptr ||
                    (state == step0Selling && hrOrderEnt->GetFilledCoinNumber() >= peerMinTradingUnit))
                {
                    CastEvent(HrAccGrpEvent::SellOrderFilled, __func__, __LINE__);
                }
                else
                {
                    CastEvent(HrAccGrpEvent::SellOrderCancled, __func__, __LINE__);
                }
            }
            else
            {
                CastEvent(HrAccGrpEvent::StopLossOrderClosed, __func__, __LINE__);
            }
            ordi->unload();
        } /* else if (hrOrderEnt->GetOrderType() == OrderType::Buy) */
    } /* for (auto ordi = orderEnts.begin(); ordi != orderEnts.end(); ++ordi) */

    PrintAvailable(hrAccEnt0);
    PrintAvailable(hrAccEnt1);
}

HrAccGrpAlgorithm::~HrAccGrpAlgorithm()
{}

void HrAccGrpAlgorithm::CastEvent(HrAccGrpEvent e, const char *func, int line)
{
    static map<HrAccGrpEvent, const char*> events =
    {
        { HrAccGrpEvent::StartUp, "StartUp" },
        { HrAccGrpEvent::BuyOrderCreated,  "BuyOrderCreated" },
        { HrAccGrpEvent::SellOrderCreated, "SellOrderCreated" },
        { HrAccGrpEvent::BuyOrderCancled,  "BuyOrderCancled" },
        { HrAccGrpEvent::SellOrderCancled, "SellOrderCancled" },
        { HrAccGrpEvent::BuyOrderFilled,   "BuyOrderFilled" },
        { HrAccGrpEvent::SellOrderFilled,  "SellOrderFilled" },
        { HrAccGrpEvent::Order2ndTimeOut,  "Order2ndTimeOut" },
        { HrAccGrpEvent::StopLossOrderCreated, "StopLossOrderCreated" },
        { HrAccGrpEvent::StopLossOrderClosed, "StopLossOrderClosed" }
    };

    string oldState = hrAccGrpEnt->GetState();
    AutomatonBase<HrAccGrpAlgorithmInterface, HrAccGrpEvent>::CastEvent(e, func, line);
    hrAccGrpEnt->SetState(algorithms.find(state)->second);
    string newState = hrAccGrpEnt->GetState();

    LOG4_INFO_C(oldState << "=>" << newState << ", e=" << events[e] << ", " << func << " at " << line);
}

void HrAccGrpAlgorithm::HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt)
{
    LOG4_TRACE("Start");
    assert (ptm0MarketEnt.GetPlatform() != nullptr && ptm1MarketEnt.GetPlatform() != nullptr);
    state->HandleMarket(ptm0MarketEnt, ptm1MarketEnt);
    LOG4_TRACE("Succeeded");
}

void HrAccGrpAlgorithm::HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
    const MarketEntity& ptm0MarketEnt,
    const MarketEntity& ptm1MarketEnt)
{
    state->HandleOrder(hrAccEnt, ptm0MarketEnt, ptm1MarketEnt);
}

CxxBeginNameSpace(HrAccGrpAlgorithmNs)
/**********************class HrAccGrpAlgorithmBase**********************/
HrAccGrpAlgorithmBase::HrAccGrpAlgorithmBase(HrAccGrpAlgorithm& automaton)
    : StateBase(automaton)
{}

HrAccGrpAlgorithmBase::~HrAccGrpAlgorithmBase()
{}

void HrAccGrpAlgorithmBase::HandleMarket(const MarketEntity& ptm0MarketEnt,
                                         const MarketEntity& ptm1MarketEnt)
{
    assert(automaton.ptmEnt0->GetId() == ptm0MarketEnt.GetPlatform()->GetId());
    assert(automaton.ptmEnt1->GetId() == ptm1MarketEnt.GetPlatform()->GetId());
}

void HrAccGrpAlgorithmBase::HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
    const MarketEntity& ptm0MarketEnt,
    const MarketEntity& ptm1MarketEnt)
{
    assert(automaton.ptmEnt0->GetId() == ptm0MarketEnt.GetPlatform()->GetId());
    assert(automaton.ptmEnt1->GetId() == ptm1MarketEnt.GetPlatform()->GetId());
}

/* protected functions */
Rate HrAccGrpAlgorithmBase::GetMargin(HrPlatformPairEntity& hrPtmPairEnt)
{
    /* hrAccEntA: 买方Account */
    shared_ptr<HrAccountEntity> hrAccEntA;
    if (hrPtmPairEnt.GetPlatformA()->GetId() == automaton.ptmEnt0->GetId())
        hrAccEntA = automaton.hrAccEnt0;
    else
        hrAccEntA = automaton.hrAccEnt1;

    /* 根据买方Account的币使用情况动态调节margin.  如果买方帐户币的余额大于币的总量的1/2, 不做动态调节.
    * 否则, 根据买方帐户币的余额按比例下调margin
    * 0.0002 大约为下订单时为了加快订单完成的效率而使用的 MinPriceSize.
    * 最终margin的取值范围为: 0.0002 ~ 当前hrPtmPairEnt配置的值
    */
    CoinNumber totalCoinNumber = automaton.hrAccEnt0->GetInvestedCoinNumber() + automaton.hrAccEnt1->GetInvestedCoinNumber();
    Rate coinRate = hrAccEntA->GetAvailableCoinNumber() / (totalCoinNumber / 2.0);
    Rate margin = std::max(hrPtmPairEnt.GetMargin() * coinRate, Rate(0.0002));
    /* 根据margin level number 调节margin */
    margin += hrPtmPairEnt.GetMarginAdjustSize() * automaton.hrAccGrpEnt->GetAccGrpMln();

    margin += hrPtmPairEnt.GetPlatformA()->GetCoinRule(automaton.hrEnt->GetCoinType())->GetTransactionFeeRate();
    margin += hrPtmPairEnt.GetPlatformB()->GetCoinRule(automaton.hrEnt->GetCoinType())->GetTransactionFeeRate();

    return margin;
}

Money HrAccGrpAlgorithmBase::GetBuyPrice(const MarketEntity& marketEnt, OderPricingMode opm)
{
    Money price = Money::Zero();
    switch (opm)
    {
    case OderPricingMode::AskAskPrice:
    case OderPricingMode::AskBidPrice:
    case OderPricingMode::AskLastPrice:
        price = marketEnt.GetAskPrice();
        break;

    case OderPricingMode::BidAskPrice:
    case OderPricingMode::BidBidPrice:
    case OderPricingMode::BidLastPrice:
        price = marketEnt.GetBidPrice();
        break;

    case OderPricingMode::LastAskPrice:
    case OderPricingMode::LastBidPrice:
    case OderPricingMode::LastLastPrice:
        price = marketEnt.GetLastPrice();
        break;

    default:
        assert(false);
    }

    assert(price != Money::Zero());
    return price;
}

Money HrAccGrpAlgorithmBase::GetSellPrice(const MarketEntity& marketEnt, OderPricingMode opm)
{
    Money price = Money::Zero();
    switch (opm)
    {
    case OderPricingMode::AskAskPrice:
    case OderPricingMode::BidAskPrice:
    case OderPricingMode::LastAskPrice:
        price = marketEnt.GetAskPrice();
        break;

    case OderPricingMode::AskBidPrice:
    case OderPricingMode::BidBidPrice:
    case OderPricingMode::LastBidPrice:
        price = marketEnt.GetBidPrice();
        break;

    case OderPricingMode::AskLastPrice:
    case OderPricingMode::BidLastPrice:
    case OderPricingMode::LastLastPrice:
        price = marketEnt.GetLastPrice();
        break;

    default:
        assert(false);
    }

    assert(price != Money::Zero());
    return price;
}

void HrAccGrpAlgorithmBase::CreateOrder(shared_ptr<HrAccountEntity> hrAccEnt, shared_ptr<HrOrderEntity> hrOrderEnt)
{
    if (hrOrderEnt->GetOrderType() == OrderType::Buy)
    {
        if (hrOrderEnt->GetFreezingAmount() > hrAccEnt->GetAvailableMoney())
        {
            LOG4_FATAL("money is not enough, robot id=" << automaton.hrEnt->GetId()
                << ", account id=" << hrAccEnt->GetId()
                << ", required money=" << hrOrderEnt->GetFreezingAmount()
                << ", available money=" << hrAccEnt->GetAvailableMoney());;
            assert(false);
        }
    }
    else
    {
        if (hrOrderEnt->GetCoinNumber() > hrAccEnt->GetAvailableCoinNumber())
        {
            LOG4_FATAL("coin is not enough, robot id=" << automaton.hrEnt->GetId()
                << ", account id=" << hrAccEnt->GetId()
                << ", required coin number=" << hrOrderEnt->GetCoinNumber()
                << ", available coin number=" << hrAccEnt->GetAvailableCoinNumber());
            assert(false);
        }
    }

    hrOrderEnt->SetId(TableIndexHelperInterface::GetInstance().GetUseableTableIndex("Order"));
    hrAccEnt->Bind(hrOrderEnt);

    PlatformInterface& ptm = GetPlatformInstance(hrAccEnt->GetPlatform());
    /* platform 对不同的coin type 有不同的精度定义, 为了让数据库中的order 和platform 上的order的价格完全一致,
     * 我们根据platform的精度来调整 entity 的价格.
     */
    hrOrderEnt->SetLimitedPrice(ptm.GetRoundedPrice(hrOrderEnt->GetLimitedPrice(), automaton.hrEnt->GetCoinType()));
    if (!ptm.CreateOrder(hrOrderEnt))
    {
        LOG4_FATAL("failed, robot id=" << automaton.hrEnt->GetId()
            << ", account id=" << hrAccEnt->GetId());

        hrAccEnt->Unbind(hrOrderEnt);
        throw runtime_error("ptm.CreateOrder() failed.");
		return;
    }
    automaton.openedOrders.push_back(hrOrderEnt);
	ptm.UpdateAvailableStep0(hrOrderEnt);

    shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
	shared_ptr<odb::session> session = Database::GetInstance().GetSession();

	StartTransaction(db, session, MaxTransactionRetryNumber);
    db->update(hrAccEnt);
    db->persist(hrOrderEnt);
    EndTransaction();

    PrintAvailable(automaton.hrAccEnt0);
    PrintAvailable(automaton.hrAccEnt1);
}

void HrAccGrpAlgorithmBase::CancelOrder(shared_ptr<HrOrderEntity> hrOrderEnt, OrderCanceledReason orderCanceledReason)
{
    assert(hrOrderEnt->GetClosingTime() == nullptr);

    shared_ptr<HrAccountEntity> hrAccEnt = dynamic_pointer_cast<HrAccountEntity>(hrOrderEnt->GetAccount());
    PlatformInterface& ptm = GetPlatformInstance(hrAccEnt->GetPlatform());
    ptm.CancelOrder(hrOrderEnt);
    ptm.FetchOrder(hrOrderEnt);
	ptm.UpdateAvailableStep1(hrOrderEnt);

    if (hrOrderEnt->GetCoinNumber() != hrOrderEnt->GetFilledCoinNumber())
        hrOrderEnt->SetCanceledReason(orderCanceledReason);
    shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
	shared_ptr<odb::session> session = Database::GetInstance().GetSession();

	StartTransaction(db, session, MaxTransactionRetryNumber);
    db->update(hrAccEnt);
    db->update(hrOrderEnt);
    EndTransaction();

    PrintAvailable(automaton.hrAccEnt0);
    PrintAvailable(automaton.hrAccEnt1);
}

void HrAccGrpAlgorithmBase::FetchOrder(shared_ptr<HrOrderEntity> hrOrderEnt)
{
	bool opened = (hrOrderEnt->GetClosingTime() == nullptr);

	PlatformInterface& ptm = GetPlatformInstance(hrOrderEnt->GetPlatform());
	ptm.FetchOrder(hrOrderEnt);

	if (opened && hrOrderEnt->GetClosingTime() != nullptr)
    {
		ptm.UpdateAvailableStep1(hrOrderEnt);

        LogHelperInterface& logHelper = LogHelperInterface::GetInstance();
        TableId accId = hrOrderEnt->GetAccount()->GetId();
        if (hrOrderEnt->GetOrderType() == OrderType::Buy)
        {
            /* buy order filled, 以后再次挂卖单出现币不足的情况时， "coin is not enough" 消息将被打印 */
            logHelper.SetMessageNumber((MsgIdNoOrderHandleMarketSell << 16) | accId, 0);
        }
        else
        {
            /* sell order filled, 以后再次挂买单出现余额不足的情况时， "money is not enough" 消息将被打印 */
            logHelper.SetMessageNumber((MsgIdNoOrderHandleMarketBuy << 16) | accId, 0);
            logHelper.SetMessageNumber((MsgIdHrAccGrpAlgorithmBaseHandleStopLossOrder << 16) | accId, 0);
        }

        shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
		shared_ptr<odb::session> session = Database::GetInstance().GetSession();
		StartTransaction(db, session, MaxTransactionRetryNumber);
        db->update(hrOrderEnt);
        EndTransaction();
    }
}

void HrAccGrpAlgorithmBase::HandleStopLossOrder(shared_ptr<HrOrderEntity> hrOrderEnt)
{
    /* stop loss order 只可能属于 account1 */
    assert(hrOrderEnt->GetAccount()->GetId() == automaton.hrAccEnt1->GetId());
    assert(hrOrderEnt->GetPriceScheduleTime() != nullptr);

    FetchOrder(hrOrderEnt);
    if (hrOrderEnt->GetClosingTime() != nullptr)
    {
        LOG4_INFO("order closed, order id=" << hrOrderEnt->GetId());
        return;
    }

    if (*hrOrderEnt->GetPriceScheduleTime() > system_clock::now())
    {
        return;
    }

    Money adjustPrice = hrOrderEnt->GetLimitedPrice() * automaton.hrEnt->GetStopLossAdjustSize();
    /* 对于买单, adjustAmount 表示重新挂单时因为价格上涨所需要的额外的可用金额, 由于实际的价格会在
     * CreateOrder() 时根据platform定义的精度做四舍五入的操作, 所以, 我们通过ceil() 函数取浮点数的
     * 上限以确保当前帐户可用余额和adjustAmount的比较结果不会因为四舍五入而出现偏差.
     */
    Money adjustAmount = (adjustPrice *  hrOrderEnt->GetUnfilledCoinNumber()).Ceil();
    if ((hrOrderEnt->GetOrderType() == OrderType::Buy) && (adjustAmount > automaton.hrAccEnt1->GetAvailableMoney()))
    {
        /* 可用余额不足以对买单调整价， 放弃此次调价的机会, 10秒后在做尝试 */
        hrOrderEnt->SetPriceScheduleTime(*hrOrderEnt->GetPriceScheduleTime() + seconds(10));
        /* parent order id 被用于判断当前oder是否为stop loss order, 在此, 我们将其设置为自己的oder id */
        hrOrderEnt->SetParentOrderId(hrOrderEnt->GetId());

        LogHelperInterface& logHelper = LogHelperInterface::GetInstance();
        uint32_t msgId = (MsgIdHrAccGrpAlgorithmBaseHandleStopLossOrder << 16) | (automaton.hrAccEnt1->GetId());
        /* 记录日志, 不中断robot的运行.  */
        if (logHelper.GetMessageNumber(msgId) == 0)
        {
            logHelper.SetMessageNumber(msgId, 1);
            LOG4_WARN("failed to adjust price for stop loss buying order, robot id=" << automaton.hrEnt->GetId()
                << ", account id=" << automaton.hrAccEnt1->GetId()
                << ", order id=" << hrOrderEnt->GetId());
        }
        return;
    }

    /* 调整价格， 并重新挂买/卖单 */
    LOG4_INFO("cancel order, state=" << automaton.hrAccGrpEnt->GetState().c_str()
        << ", reason = " << GetEnumName(OrderCanceledReason::CanceledByOrder2ndTimeOut)
        << ", order id=" << hrOrderEnt->GetId()
        << ", order type=" << GetEnumName(hrOrderEnt->GetOrderType())
        << ", price=" << hrOrderEnt->GetLimitedPrice()
        << ", coin number=" << hrOrderEnt->GetCoinNumber());
    CancelOrder(hrOrderEnt, OrderCanceledReason::CanceledByOrder2ndTimeOut);
    CoinNumber coinNumber = hrOrderEnt->GetUnfilledCoinNumber();
    if (coinNumber < automaton.ptm1CoinRuleEnt->GetMinTradingUnit())
    {
        return;
    }

    shared_ptr<HrOrderEntity> newHrOrderEnt;
    if (hrOrderEnt->GetOrderType() == OrderType::Buy)
    {
        Money price = hrOrderEnt->GetLimitedPrice() + adjustPrice;
        newHrOrderEnt = make_shared<HrOrderEntity>(InvalidTableId, OrderType::Buy, price, coinNumber);
    }
    else
    {
        Money price = hrOrderEnt->GetLimitedPrice() - adjustPrice;
        newHrOrderEnt = make_shared<HrOrderEntity>(InvalidTableId, OrderType::Sell, price, coinNumber);
    }

    newHrOrderEnt->SetParentOrderId(hrOrderEnt->GetId());
    newHrOrderEnt->SetPriceScheduleTime(newHrOrderEnt->GetCreatingTime() + automaton.hrEnt->GetStopLossOrderTimeOut());

    LOG4_INFO("create order, state=" << automaton.hrAccGrpEnt->GetState().c_str()
        << ", order id=" << newHrOrderEnt->GetId()
        << ", order type=" << GetEnumName(newHrOrderEnt->GetOrderType())
        << ", price=" << newHrOrderEnt->GetLimitedPrice()
        << ", coin number=" << newHrOrderEnt->GetCoinNumber());
    CreateOrder(automaton.hrAccEnt1, newHrOrderEnt);
}

/**********************class NoOrder**********************/
NoOrder::NoOrder(HrAccGrpAlgorithm& automaton)
    : HrAccGrpAlgorithmBase(automaton)
{}

NoOrder::~NoOrder()
{}

void NoOrder::HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleMarket(ptm0MarketEnt, ptm1MarketEnt);

    LogHelperInterface& logHelper = LogHelperInterface::GetInstance();
    auto ptmEnt0 = automaton.ptmEnt0;
    auto ptm0MinPriceSize = automaton.ptm0CoinRuleEnt->GetMinPriceSize();

    /* hrAccEnt0 -> hrAccEnt1 */
    auto hrPtmPairEnt = automaton.hrEnt->GetPlatformPair(ptm0MarketEnt.GetPlatformId(), ptm1MarketEnt.GetPlatformId());
    Rate margin = GetMargin(*hrPtmPairEnt);
    CoinNumber coinNumber = automaton.hrEnt->GetMinTradingUnit();

    Money buyingPrice = GetBuyPrice(ptm0MarketEnt, hrPtmPairEnt->GetOpm());
    Money expSellPrice = buyingPrice * (1 + margin);
	//=> expSellPrice = (hrOrderEnt->GetLimitedPrice() - ptm0MinPriceSize) * (1 + GetMargin(*hrPtmPairEnt));
    Money viableSellPrice = GetSellPrice(ptm1MarketEnt, hrPtmPairEnt->GetOpm());
    LOG4_TRACE("buy price(ptm0)=" << buyingPrice
        << ", expected sell price=" << expSellPrice
        << ", viable sell price(ptm1)=" << viableSellPrice
        << ", (>0) " << (viableSellPrice - expSellPrice));

    if (viableSellPrice > expSellPrice)
    {
        /* Create step0 buy order. */
		auto price = buyingPrice + ptm0MinPriceSize;
        auto hrOrderEnt = make_shared<HrOrderEntity>(InvalidTableId, OrderType::Buy, price, coinNumber);

        /* 创建buy order，hrAccEnt0 必须要有足够的money. 由于实际的价格会在
        * CreateOrder() 时根据platform定义的精度做四舍五入的操作, 所以, 我们通过ceil() 函数取浮点数的
        * 上限以确保当前帐户可用余额和adjustAmount的比较结果不会因为四舍五入而出现偏差.
        */
        if (automaton.hrAccEnt0->GetAvailableMoney() < hrOrderEnt->GetFreezingAmount().Ceil())
        {
            uint32_t msgId = (MsgIdNoOrderHandleMarketBuy << 16) | (automaton.hrAccEnt0->GetId());
            if (logHelper.GetMessageNumber(msgId) == 0)
            {
                logHelper.SetMessageNumber(msgId, 1);

                LOG4_INFO("money is not enough"
                    << ", robot id=" << automaton.hrEnt->GetId()
					<< ", account id=" << automaton.hrAccEnt0->GetId()
					<< ", margin=" << margin
                    << ", required money=" << (price * coinNumber)
                    << ", available money=" << automaton.hrAccEnt0->GetAvailableMoney());
            }
            return;
        }

        /* 在 hrAccEnt0 创建buy order前必须先判断: buy order 完成后是否hrAccEnt1有足够的coin
         * 可以用来创建sell order.
         */
        if (automaton.hrAccEnt1->GetAvailableCoinNumber() < coinNumber)
        {
            uint32_t msgId = (MsgIdNoOrderHandleMarketSell << 16) | (automaton.hrAccEnt1->GetId());
            if (logHelper.GetMessageNumber(msgId) == 0)
            {
                logHelper.SetMessageNumber(msgId, 1);

                LOG4_INFO("coin will be not enough when Step1Selling"
                    << ", robot id=" << automaton.hrEnt->GetId()
					<< ", account id=" << automaton.hrAccEnt1->GetId()
					<< ", margin=" << margin
                    << ", required coin number=" << hrOrderEnt->GetCoinNumber()
                    << ", available coin number=" << automaton.hrAccEnt1->GetAvailableCoinNumber());
            }
            return;
        }

        LOG4_INFO("create order, state=NoOrder, order type=Buy"
            << ", robot id=" << automaton.hrEnt->GetId()
            << ", account id=" << automaton.hrAccEnt0->GetId()
            << ", margin=" << margin
            << ", price=" << price
            << ", coin number=" << coinNumber
            << ", ptm0 ask price=" << ptm0MarketEnt.GetAskPrice()
            << ", ptm0 bid price=" << ptm0MarketEnt.GetBidPrice()
            << ", ptm0 lst price=" << ptm0MarketEnt.GetLastPrice());

        CreateOrder(automaton.hrAccEnt0, hrOrderEnt);
        CastEvent(HrAccGrpEvent::BuyOrderCreated, __func__, __LINE__);
        return;
    }

    /* hrAccEnt0 <- hrAccEnt1 */
    hrPtmPairEnt = automaton.hrEnt->GetPlatformPair(ptm1MarketEnt.GetPlatformId(), ptm0MarketEnt.GetPlatformId());
    margin = GetMargin(*hrPtmPairEnt);

    Money sellingPrice = GetSellPrice(ptm0MarketEnt, hrPtmPairEnt->GetOpm());
    Money expBuyPrice = sellingPrice * (1 - margin);
	//=> expBuyPrice = (hrOrderEnt->GetLimitedPrice() + ptm0MinPriceSize) * (1 - GetMargin(*hrPtmPairEnt));
    Money viableBuyPrice = GetBuyPrice(ptm1MarketEnt, hrPtmPairEnt->GetOpm());
    LOG4_TRACE("sell price(ptm0)" << sellingPrice
        << ", expected buy price=" << expBuyPrice
        << ", viable buy price(ptm1)=" << viableBuyPrice
        << ", (<0) " << (viableBuyPrice - expBuyPrice));

    if (viableBuyPrice < expBuyPrice)
    {
        /* Create step0 sell order. */
		auto price = sellingPrice - ptm0MinPriceSize;
        auto hrOrderEnt = make_shared<HrOrderEntity>(InvalidTableId, OrderType::Sell, price, coinNumber);

        /* 创建sell，hrAccEnt0 必须要有足够的coin */
        if (automaton.hrAccEnt0->GetAvailableCoinNumber() < coinNumber)
        {
            uint32_t msgId = (MsgIdNoOrderHandleMarketSell << 16) | (automaton.hrAccEnt0->GetId());
            if (logHelper.GetMessageNumber(msgId) == 0)
            {
                logHelper.SetMessageNumber(msgId, 1);

                LOG4_INFO("coin is not enough"
                    << ", robot id=" << automaton.hrEnt->GetId()
					<< ", account id=" << automaton.hrAccEnt0->GetId()
					<< ", margin=" << margin
                    << ", required coin number=" << hrOrderEnt->GetCoinNumber()
                    << ", available coin number=" << automaton.hrAccEnt0->GetAvailableCoinNumber());
            }
            return;
        }

        /* 在 hrAccEnt0 创建sell order前必须先判断: sell order 完成后是否hrAccEnt1有足够的money
         * 可以用来创建buy order.由于实际的价格会在CreateOrder() 时根据platform定义的精度做
         * 四舍五入的操作, 所以, 我们通过ceil() 函数取浮点数的上限以确保当前帐户可用余额和
         * adjustAmount的比较结果不会因为四舍五入而出现偏差.
         */
        if (automaton.hrAccEnt1->GetAvailableMoney() < hrOrderEnt->GetFreezingAmount().Ceil())
        {
            uint32_t msgId = (MsgIdNoOrderHandleMarketBuy << 16) | (automaton.hrAccEnt1->GetId());
            if (logHelper.GetMessageNumber(msgId) == 0)
            {
                logHelper.SetMessageNumber(msgId, 1);

                LOG4_INFO("money will be not enough when Step1Buying"
                    << ", robot id=" << automaton.hrEnt->GetId()
					<< ", account id=" << automaton.hrAccEnt1->GetId()
					<< ", margin=" << margin
                    << ", required money=" << hrOrderEnt->GetFreezingAmount()
                    << ", available money=" << automaton.hrAccEnt1->GetAvailableMoney());
            }
            return;
        }

        LOG4_INFO("create order, state=NoOrder, order type=Sell"
            << ", robot id=" << automaton.hrEnt->GetId()
            << ", account id=" << automaton.hrAccEnt0->GetId()
            << ", margin=" << margin
            << ", price=" << price
            << ", coin number=" << coinNumber
            << ", ptm0 ask price=" << ptm0MarketEnt.GetAskPrice()
            << ", ptm0 bid price=" << ptm0MarketEnt.GetBidPrice()
            << ", ptm0 lst price=" << ptm0MarketEnt.GetLastPrice());

        CreateOrder(automaton.hrAccEnt0, hrOrderEnt);
        CastEvent(HrAccGrpEvent::SellOrderCreated, __func__, __LINE__);
        return;
    }
}

void NoOrder::HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
    const MarketEntity& ptm0MarketEnt,
    const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleOrder(hrAccEnt, ptm0MarketEnt, ptm1MarketEnt);

    /* stop loss order 只可能属于account 1 */
    if (hrAccEnt->GetId() == automaton.hrAccEnt0->GetId())
    {
        return;
    }
    assert(hrAccEnt->GetId() == automaton.hrAccEnt1->GetId());

    for (auto iter = automaton.openedOrders.begin(); iter != automaton.openedOrders.end(); ++iter)
    {
        auto hrOrderEnt = *iter;
        /* stop loss order 只可能属于account 1, 并且NoOrder 状态下所有的订单都是stop loss order. */
        assert(hrOrderEnt->GetAccount()->GetId() == hrAccEnt->GetId());

        /* handle stop loss order */
        HandleStopLossOrder(hrOrderEnt);
		if (hrOrderEnt->GetClosingTime() != nullptr)
		{
			automaton.openedOrders.erase(iter);
			break;
		}
    }
}

/**********************class Step0Buying**********************/
Step0Buying::Step0Buying(HrAccGrpAlgorithm& automaton)
    : HrAccGrpAlgorithmBase(automaton)
{}

Step0Buying::~Step0Buying()
{}

void Step0Buying::HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleMarket(ptm0MarketEnt, ptm1MarketEnt);

    list<shared_ptr<HrOrderEntity>>& openedOrders = automaton.openedOrders;
    shared_ptr<HrAccountEntity> hrAccEnt0 = automaton.hrAccEnt0;
	auto ptm0MinPriceSize = automaton.ptm0CoinRuleEnt->GetMinPriceSize();
	auto ptm1MinPriceSize = automaton.ptm1CoinRuleEnt->GetMinPriceSize();

    auto cmp = [](TableId accId, shared_ptr<HrOrderEntity> hrOrderEnt)->bool
    {
        return accId == hrOrderEnt->GetAccount()->GetId();
    };

    auto hrPtmPairEnt = automaton.hrEnt->GetPlatformPair(ptm0MarketEnt.GetPlatformId(), ptm1MarketEnt.GetPlatformId());
    auto i = find_if(openedOrders.begin(), openedOrders.end(), bind(cmp, hrAccEnt0->GetId(), _1));
    assert(i != openedOrders.end());
    auto hrOrderEnt0 = *i;
    assert(hrOrderEnt0->GetOrderType() == OrderType::Buy);

    Money buyingPrice = GetBuyPrice(ptm0MarketEnt, hrPtmPairEnt->GetOpm());
	Money expSellPrice = (hrOrderEnt0->GetLimitedPrice() - ptm0MinPriceSize) * (1 + GetMargin(*hrPtmPairEnt));
	Money viableSellPrice = GetSellPrice(ptm1MarketEnt, hrPtmPairEnt->GetOpm());

    shared_ptr<OrderCanceledReason> orderCanceledReason;

    if (viableSellPrice < expSellPrice)
    {
        /* platform 1 price are falling */
        orderCanceledReason = make_shared<OrderCanceledReason>(OrderCanceledReason::CanceledByDecBidPrice);
    }
    else if (buyingPrice + ptm0MinPriceSize > hrOrderEnt0->GetLimitedPrice())
    {
        /* platform 0 price are raising. 事实上, 我们可能有多个robot同时运行, 为了避免robot之间相互干扰导致
        * 频繁的cancel order并间接的抬高买价(/拉低卖价格), 特在此限制2秒内不因为IncAskPrice 的原因cancel
        * order.
        */
        if (hrOrderEnt0->GetCreatingTime() + seconds(2) > system_clock::now())
            return;

        orderCanceledReason = make_shared<OrderCanceledReason>(OrderCanceledReason::CanceledByIncAskPrice);
    }
    else
    {
        return;
    }

    LOG4_INFO("cancel order, reason=" << GetEnumName(*orderCanceledReason)
        << ", order id=" << hrOrderEnt0->GetId()
        << ", order type=" << GetEnumName(hrOrderEnt0->GetOrderType())
        << ", price=" << hrOrderEnt0->GetLimitedPrice()
        << ", coin number=" << hrOrderEnt0->GetCoinNumber()
        << ", ptm0 ask price=" << ptm0MarketEnt.GetAskPrice()
        << ", ptm0 bid price=" << ptm0MarketEnt.GetBidPrice()
        << ", ptm0 lst price=" << ptm0MarketEnt.GetLastPrice());
    CancelOrder(hrOrderEnt0, *orderCanceledReason);
    if (hrOrderEnt0->GetFilledCoinNumber() >= automaton.ptm1CoinRuleEnt->GetMinTradingUnit())
    {
        CastEvent(HrAccGrpEvent::BuyOrderFilled, __func__, __LINE__);

		auto price = viableSellPrice - ptm1MinPriceSize;
        auto coinNumber = hrOrderEnt0->GetFilledCoinNumber();
        auto hrOrderEnt1 = make_shared<HrOrderEntity>(InvalidTableId, OrderType::Sell, price, coinNumber);
        hrOrderEnt1->SetPriceScheduleTime(hrOrderEnt1->GetCreatingTime() + automaton.hrEnt->GetOrder2ndTimeOut());

        LOG4_INFO("create order"
            << ", order id=" << hrOrderEnt1->GetId()
            << ", order type=" << GetEnumName(hrOrderEnt1->GetOrderType())
            << ", price=" << hrOrderEnt1->GetLimitedPrice()
            << ", coin number=" << hrOrderEnt1->GetCoinNumber()
            << ", ptm1 ask price=" << ptm1MarketEnt.GetAskPrice()
            << ", ptm1 bid price=" << ptm1MarketEnt.GetBidPrice()
            << ", ptm1 lst price=" << ptm1MarketEnt.GetLastPrice());
        /* NoOrder::HandleMarket() 创建Setp0 Buy order时， 已经检查过 hrAccEnt1 的可用coin number > coinNumber.
        */
        assert(automaton.hrAccEnt1->GetAvailableCoinNumber() >= coinNumber);
        CreateOrder(automaton.hrAccEnt1, hrOrderEnt1);
        CastEvent(HrAccGrpEvent::SellOrderCreated, __func__, __LINE__);
    }
    else
    {
        if (hrOrderEnt0->GetFilledCoinNumber() == CoinNumber::Zero())
        {
            /* 为了避免过多的canceled order影响结果分析, 将其从数据库删除 */
            shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
            shared_ptr<odb::session> session = Database::GetInstance().GetSession();

            hrAccEnt0->Unbind(hrOrderEnt0);
            StartTransaction(db, session, MaxTransactionRetryNumber);
            db->erase(hrOrderEnt0);
            EndTransaction();
        }
        CastEvent(HrAccGrpEvent::BuyOrderCancled, __func__, __LINE__);
    }

    openedOrders.erase(i);
}

void Step0Buying::HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
    const MarketEntity& ptm0MarketEnt,
    const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleOrder(hrAccEnt, ptm0MarketEnt, ptm1MarketEnt);

	auto ptm0MinPriceSize = automaton.ptm0CoinRuleEnt->GetMinPriceSize();
    auto ptm1MinPriceSize = automaton.ptm1CoinRuleEnt->GetMinPriceSize();
    for (auto iter = automaton.openedOrders.begin(); iter != automaton.openedOrders.end(); ++iter)
    {
        auto hrOrderEnt0 = *iter;
        if (hrOrderEnt0->GetAccount()->GetId() != hrAccEnt->GetId())
            continue;

        if (hrAccEnt->GetId() == automaton.hrAccEnt1->GetId())
        {
            /* handle stop loss order */
            assert(hrOrderEnt0->GetParentOrderId() != nullptr);
            HandleStopLossOrder(hrOrderEnt0);
            if (hrOrderEnt0->GetClosingTime() != nullptr)
			{
				/*
				* 为了简化代码, 我们在此直接退出循环, 待下一次HandleOrder时再继续处理stop loss order 的检查.
				*/
				automaton.openedOrders.erase(iter);
				break;
			}
			continue;
        }

        assert(hrAccEnt->GetId() == automaton.hrAccEnt0->GetId());
        assert(hrOrderEnt0->GetPriceScheduleTime() == nullptr);
        FetchOrder(hrOrderEnt0);
        auto hrPtmPairEnt = automaton.hrEnt->GetPlatformPair(ptm0MarketEnt.GetPlatformId(), ptm1MarketEnt.GetPlatformId());
        if (hrOrderEnt0->GetClosingTime() != nullptr)
        {
            CastEvent(HrAccGrpEvent::BuyOrderFilled, __func__, __LINE__);
            Money expSellPrice = (hrOrderEnt0->GetLimitedPrice() - ptm0MinPriceSize) * (1 + GetMargin(*hrPtmPairEnt));
            Money viableSellPrice = GetSellPrice(ptm1MarketEnt, hrPtmPairEnt->GetOpm());
            if (viableSellPrice < expSellPrice)
            {
                /* 正常情况下, 如果在此之前价格已经产生变化并导致负利润, 此buy order之前就应该已经被cancel掉了.
                 * 但是, GetMargin()会根据币的余额动态变化， 所以viableSellPrice < expSellPrice也是有可能的。
                 * 因此不能引发异常, 所以我们仅仅在日志中记录warning.
                 */
                LOG4_WARN("viableSellPrice(" << viableSellPrice << ") < expSellPrice(" << expSellPrice << ")");
            }

            auto price = viableSellPrice - ptm1MinPriceSize;
            auto coinNumber = automaton.hrEnt->GetMinTradingUnit();
            auto hrOrderEnt1 = make_shared<HrOrderEntity>(InvalidTableId, OrderType::Sell, price, coinNumber);
            hrOrderEnt1->SetPriceScheduleTime(hrOrderEnt1->GetCreatingTime() + automaton.hrEnt->GetOrder2ndTimeOut());

            LOG4_INFO("create order"
                << ", order id=" << hrOrderEnt1->GetId()
                << ", order type=" << GetEnumName(hrOrderEnt1->GetOrderType())
                << ", price=" << hrOrderEnt1->GetLimitedPrice()
                << ", coin number=" << hrOrderEnt1->GetCoinNumber()
                << ", ptm1 ask price=" << ptm1MarketEnt.GetAskPrice()
                << ", ptm1 bid price=" << ptm1MarketEnt.GetBidPrice()
                << ", ptm1 lst price=" << ptm1MarketEnt.GetLastPrice());
            /* NoOrder::HandleMarket() 创建Setp0 Sell order时， 已经检查过 hrAccEnt1 的可用coin number > coinNumber.
            */
            assert(automaton.hrAccEnt1->GetAvailableCoinNumber() >= coinNumber);
            CreateOrder(automaton.hrAccEnt1, hrOrderEnt1);
            CastEvent(HrAccGrpEvent::SellOrderCreated, __func__, __LINE__);

            /* 因为产生了新的order, 并且新的order刚刚被插入到openedOrders, 为了避免不必要的fetch 操作, 同时也
                * 为了简化代码, 我们在此直接退出循环, 待下一次HandleOrder时再继续处理stop loss order 的检查. \
                */
            automaton.openedOrders.erase(iter);
            break;
        }
    }
}

/**********************class Step1Buying**********************/
Step1Buying::Step1Buying(HrAccGrpAlgorithm& automaton)
    : HrAccGrpAlgorithmBase(automaton)
{}

Step1Buying::~Step1Buying()
{}

void Step1Buying::HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleMarket(ptm0MarketEnt, ptm1MarketEnt);
}

void Step1Buying::HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
    const MarketEntity& ptm0MarketEnt,
    const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleOrder(hrAccEnt, ptm0MarketEnt, ptm1MarketEnt);

    /* step1 buy order, stop loss order 只可能属于account 1 */
    if (hrAccEnt->GetId() == automaton.hrAccEnt0->GetId())
    {
        return;
    }
    assert(hrAccEnt->GetId() == automaton.hrAccEnt1->GetId());

    for (auto iter = automaton.openedOrders.begin(); iter != automaton.openedOrders.end(); ++iter)
    {
        auto hrOrderEnt = *iter;
        /* step1 buy order, stop loss order 只可能属于account 1 */
        assert(hrOrderEnt->GetAccount()->GetId() == hrAccEnt->GetId());

        if (hrOrderEnt->GetParentOrderId() == nullptr)
        {
            /* step 1 buy order */
            if (*hrOrderEnt->GetPriceScheduleTime() < system_clock::now())
            {
                /* step 1 buy order => stop loss order */
                CastEvent(HrAccGrpEvent::Order2ndTimeOut, __func__, __LINE__);
                HandleStopLossOrder(hrOrderEnt);
				assert(hrOrderEnt->GetClosingTime() != nullptr);
                /*
                * 为了简化代码, 我们在此直接退出循环, 待下一次HandleOrder时再继续处理stop loss order 的检查.
                */
				automaton.openedOrders.erase(iter);
                break;
            }

            FetchOrder(hrOrderEnt);
            if (hrOrderEnt->GetClosingTime() != nullptr)
            {
                /* step 1 buy order filled */
                LOG4_INFO("order closed"
                    << ", order id=" << hrOrderEnt->GetId()
                    << ", order type=" << GetEnumName(hrOrderEnt->GetOrderType())
                    << ", price=" << hrOrderEnt->GetLimitedPrice()
                    << ", coin number=" << hrOrderEnt->GetCoinNumber());
                PrintAvailable(automaton.hrAccEnt0);
                PrintAvailable(automaton.hrAccEnt1);
                automaton.openedOrders.erase(iter);
                CastEvent(HrAccGrpEvent::BuyOrderFilled, __func__, __LINE__);
                break;
            }
        }
        else
        {
            /* stop loss order */
            HandleStopLossOrder(hrOrderEnt);
			if (hrOrderEnt->GetClosingTime() != nullptr)
			{
				/*
				* 为了简化代码, 我们在此直接退出循环, 待下一次HandleOrder时再继续处理stop loss order 的检查.
				*/
				automaton.openedOrders.erase(iter);
				break;
			}
        }
    }
}

/**********************class Step0Selling**********************/
Step0Selling::Step0Selling(HrAccGrpAlgorithm& automaton)
    : HrAccGrpAlgorithmBase(automaton)
{}

Step0Selling::~Step0Selling()
{}

void Step0Selling::HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleMarket(ptm0MarketEnt, ptm1MarketEnt);

    list<shared_ptr<HrOrderEntity>>& openedOrders = automaton.openedOrders;
    shared_ptr<HrAccountEntity> hrAccEnt0 = automaton.hrAccEnt0;
    auto ptm0MinPriceSize = automaton.ptm0CoinRuleEnt->GetMinPriceSize();
	auto ptm1MinPriceSize = automaton.ptm1CoinRuleEnt->GetMinPriceSize();

    auto cmp = [](TableId accId, shared_ptr<HrOrderEntity> hrOrderEnt)->bool
    {
        return accId == hrOrderEnt->GetAccount()->GetId();
    };

    auto hrPtmPairEnt = automaton.hrEnt->GetPlatformPair(ptm1MarketEnt.GetPlatformId(), ptm0MarketEnt.GetPlatformId());
    auto i = find_if(openedOrders.begin(), openedOrders.end(), bind(cmp, hrAccEnt0->GetId(), _1));
    assert(i != openedOrders.end());
    auto hrOrderEnt0 = *i;
    assert(hrOrderEnt0->GetOrderType() == OrderType::Sell);

    Money sellingPrice = GetSellPrice(ptm0MarketEnt, hrPtmPairEnt->GetOpm());
	Money expBuyPrice = (hrOrderEnt0->GetLimitedPrice() + ptm0MinPriceSize) * (1 - GetMargin(*hrPtmPairEnt));
    Money viableBuyPrice = GetBuyPrice(ptm1MarketEnt, hrPtmPairEnt->GetOpm());

    shared_ptr<OrderCanceledReason> orderCanceledReason;
    if (viableBuyPrice > expBuyPrice)
    {
        /* platform 1 price are raising */
        orderCanceledReason = make_shared<OrderCanceledReason>(OrderCanceledReason::CanceledByIncAskPrice);
    }
    else if (sellingPrice - ptm0MinPriceSize < (*i)->GetLimitedPrice())
    {
        /* platform 0 price are falling. 事实上, 我们可能有多个robot同时运行, 为了避免robot之间相互干扰导致
        * 频繁的cancel order并间接的抬高买价(/拉低卖价格), 特在此限制2秒内不因为IncAskPrice 的原因cancel
        * order.
        */
        if (hrOrderEnt0->GetCreatingTime() + seconds(2) > system_clock::now())
            return;

        orderCanceledReason = make_shared<OrderCanceledReason>(OrderCanceledReason::CanceledByDecAskPrice);
    }
    else
    {
        return;
    }

    LOG4_INFO("cancel order, reason=" << GetEnumName(*orderCanceledReason)
        << ", order id=" << hrOrderEnt0->GetId()
        << ", order type=" << GetEnumName(hrOrderEnt0->GetOrderType())
        << ", price=" << hrOrderEnt0->GetLimitedPrice()
        << ", coin number=" << hrOrderEnt0->GetCoinNumber()
        << ", ptm0 ask price=" << ptm0MarketEnt.GetAskPrice()
        << ", ptm0 bid price=" << ptm0MarketEnt.GetBidPrice()
        << ", ptm0 lst price=" << ptm0MarketEnt.GetLastPrice());
    CancelOrder(hrOrderEnt0, *orderCanceledReason);
    if (hrOrderEnt0->GetFilledCoinNumber() >= automaton.ptm1CoinRuleEnt->GetMinTradingUnit())
    {
        CastEvent(HrAccGrpEvent::SellOrderFilled, __func__, __LINE__);

        auto price = viableBuyPrice + ptm1MinPriceSize;
        auto coinNumber = hrOrderEnt0->GetFilledCoinNumber();
        auto hrAcc1AvlMoney = automaton.hrAccEnt1->GetAvailableMoney();
        if (price * coinNumber > hrAcc1AvlMoney)
        {
            /* NoOrder::HandleMarket() 创建Setp0 Sell order时， 已经检查过 hrAccEnt1 的可用money 大于等于
            * (expBuyPrice + ptmMinPriceSize) * coinNumber. 但是， sell order 创建到完成这期间有可能有stop loss order
            * 调整价格重新挂单， 并导致余额不足， 所以我们必须根据余额的情况调整价格
            */
            /* 金额不可能相差太多， 否则说明程序已经出现了异常 */
            assert((price * coinNumber - hrAcc1AvlMoney) < hrAcc1AvlMoney * 0.01);
            price = (hrAcc1AvlMoney * 0.999) / coinNumber;

            LOG4_WARN("money is not enough, adjust buying price"
                << ", expected price=" << (viableBuyPrice + ptm1MinPriceSize)
                << ", real price=" << price);
        }
        auto hrOrderEnt1 = make_shared<HrOrderEntity>(InvalidTableId, OrderType::Buy, price, coinNumber);
        hrOrderEnt1->SetPriceScheduleTime(hrOrderEnt1->GetCreatingTime() + automaton.hrEnt->GetOrder2ndTimeOut());

        LOG4_INFO("create order"
            << ", order id=" << hrOrderEnt1->GetId()
            << ", order type=" << GetEnumName(hrOrderEnt1->GetOrderType())
            << ", price=" << hrOrderEnt1->GetLimitedPrice()
            << ", coin number=" << hrOrderEnt1->GetCoinNumber()
            << ", ptm1 ask price=" << ptm1MarketEnt.GetAskPrice()
            << ", ptm1 bid price=" << ptm1MarketEnt.GetBidPrice()
            << ", ptm1 lst price=" << ptm1MarketEnt.GetLastPrice());
        CreateOrder(automaton.hrAccEnt1, hrOrderEnt1);
        CastEvent(HrAccGrpEvent::BuyOrderCreated, __func__, __LINE__);
    }
    else
    {
        if (hrOrderEnt0->GetFilledCoinNumber() == CoinNumber::Zero())
        {
            /* 为了避免过多的canceled order影响结果分析, 将其从数据库删除 */
            shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
            shared_ptr<odb::session> session = Database::GetInstance().GetSession();

            hrAccEnt0->Unbind(hrOrderEnt0);
            StartTransaction(db, session, MaxTransactionRetryNumber);
            db->erase(hrOrderEnt0);
            EndTransaction();
        }
        CastEvent(HrAccGrpEvent::SellOrderCancled, __func__, __LINE__);
    }

    openedOrders.erase(i);
}

void Step0Selling::HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
    const MarketEntity& ptm0MarketEnt,
    const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleOrder(hrAccEnt, ptm0MarketEnt, ptm1MarketEnt);

    auto ptm0MinPriceSize = automaton.ptm0CoinRuleEnt->GetMinPriceSize();
	auto ptm1MinPriceSize = automaton.ptm1CoinRuleEnt->GetMinPriceSize();
    for (auto iter = automaton.openedOrders.begin(); iter != automaton.openedOrders.end(); ++iter)
    {
        auto hrOrderEnt0 = *iter;
        if (hrOrderEnt0->GetAccount()->GetId() != hrAccEnt->GetId())
            continue;

        if (hrAccEnt->GetId() == automaton.hrAccEnt1->GetId())
        {
            /* handle stop loss order */
            assert(hrOrderEnt0->GetParentOrderId() != nullptr);
            HandleStopLossOrder(hrOrderEnt0);
            if (hrOrderEnt0->GetClosingTime() != nullptr)
			{
				/*
				* 为了简化代码, 我们在此直接退出循环, 待下一次HandleOrder时再继续处理stop loss order 的检查.
				*/
				automaton.openedOrders.erase(iter);
				break;
			}
			continue;
        }

        assert(hrAccEnt->GetId() == automaton.hrAccEnt0->GetId());
        assert(hrOrderEnt0->GetPriceScheduleTime() == nullptr);
        FetchOrder(hrOrderEnt0);
        auto hrPtmPairEnt = automaton.hrEnt->GetPlatformPair(ptm1MarketEnt.GetPlatformId(), ptm0MarketEnt.GetPlatformId());
        if (hrOrderEnt0->GetClosingTime() != nullptr)
        {
            CastEvent(HrAccGrpEvent::SellOrderFilled, __func__, __LINE__);
            Money expBuyPrice = (hrOrderEnt0->GetLimitedPrice() + ptm0MinPriceSize) * (1 - GetMargin(*hrPtmPairEnt));
            Money viableBuyPrice = GetBuyPrice(ptm1MarketEnt, hrPtmPairEnt->GetOpm());
            if (viableBuyPrice > expBuyPrice)
            {
                /* 正常情况下, 如果在此之前价格已经产生变化并导致负利润, 此buy order之前就应该已经被cancel掉了.
                 * 但是, GetMargin()会根据币的余额动态变化， 所以viableBuyPrice > expBuyPrice也是有可能的。
                 * 因此不能引发异常, 所以我们仅仅在日志中记录warning.
                 */
                LOG4_WARN("viableBuyPrice(" << viableBuyPrice << ") > expBuyPrice(" << expBuyPrice << ")");
            }

            auto price = viableBuyPrice + ptm1MinPriceSize;
            auto coinNumber = automaton.hrEnt->GetMinTradingUnit();
            auto hrAcc1AvlMoney = automaton.hrAccEnt1->GetAvailableMoney();
            if (price * coinNumber > hrAcc1AvlMoney)
            {
                /* NoOrder::HandleMarket() 创建Setp0 Sell order时， 已经检查过 hrAccEnt1 的可用money 大于等于
                * (expBuyPrice + ptmMinPriceSize) * coinNumber. 但是， sell order 创建到完成这期间有可能有stop loss order
                * 调整价格重新挂单， 并导致余额不足， 所以我们必须根据余额的情况调整价格
                */
                /* 金额不可能相差太多， 否则说明程序已经出现了异常 */
                assert((price * coinNumber - hrAcc1AvlMoney) < hrAcc1AvlMoney * 0.01);
                price = (hrAcc1AvlMoney * 0.999) / coinNumber;

                LOG4_WARN("money is not enough, adjust buying price"
                    << ", expected price=" << (viableBuyPrice + ptm1MinPriceSize)
                    << ", real price=" << price);
            }
            auto hrOrderEnt1 = make_shared<HrOrderEntity>(InvalidTableId, OrderType::Buy, price, coinNumber);
            hrOrderEnt1->SetPriceScheduleTime(hrOrderEnt1->GetCreatingTime() + automaton.hrEnt->GetOrder2ndTimeOut());

            LOG4_INFO("create order"
                << ", order id=" << hrOrderEnt1->GetId()
                << ", order type=" << GetEnumName(hrOrderEnt1->GetOrderType())
                << ", price=" << hrOrderEnt1->GetLimitedPrice()
                << ", coin number=" << hrOrderEnt1->GetCoinNumber()
                << ", ptm1 ask price" << ptm1MarketEnt.GetAskPrice()
                << ", ptm1 bid price" << ptm1MarketEnt.GetBidPrice()
                << ", ptm1 lst price" << ptm1MarketEnt.GetLastPrice());
            CreateOrder(automaton.hrAccEnt1, hrOrderEnt1);
            CastEvent(HrAccGrpEvent::BuyOrderCreated, __func__, __LINE__);

            /* 因为产生了新的order, 并且新的order刚刚被插入到openedOrders, 为了避免不必要的fetch 操作, 同时也
            * 为了简化代码, 我们在此直接退出循环, 待下一次HandleOrder时再继续处理stop loss order 的检查. \
            */
            automaton.openedOrders.erase(iter);
            break;
        }
    }
}

/**********************class Step1Selling**********************/
Step1Selling::Step1Selling(HrAccGrpAlgorithm& automaton)
    : HrAccGrpAlgorithmBase(automaton)
{}

Step1Selling::~Step1Selling()
{}

void Step1Selling::HandleMarket(const MarketEntity& ptm0MarketEnt, const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleMarket(ptm0MarketEnt, ptm1MarketEnt);
}

void Step1Selling::HandleOrder(shared_ptr<HrAccountEntity> hrAccEnt,
    const MarketEntity& ptm0MarketEnt,
    const MarketEntity& ptm1MarketEnt)
{
    HrAccGrpAlgorithmBase::HandleOrder(hrAccEnt, ptm0MarketEnt, ptm1MarketEnt);

    /* Step1 Sell order, stop loss order 只可能属于account 1 */
    if (hrAccEnt->GetId() == automaton.hrAccEnt0->GetId())
    {
        return;
    }
    assert(hrAccEnt->GetId() == automaton.hrAccEnt1->GetId());

    for (auto iter = automaton.openedOrders.begin(); iter != automaton.openedOrders.end(); ++iter)
    {
        auto hrOrderEnt = *iter;
        /* Step1 Sell order, stop loss order 只可能属于account 1 */
        assert (hrOrderEnt->GetAccount()->GetId() == hrAccEnt->GetId());

        if (hrOrderEnt->GetParentOrderId() == nullptr)
        {
            if (*hrOrderEnt->GetPriceScheduleTime() < system_clock::now())
            {
                /* step 1 sell order 转换为 stop loss order */
                CastEvent(HrAccGrpEvent::Order2ndTimeOut, __func__, __LINE__);
                HandleStopLossOrder(hrOrderEnt);
				assert(hrOrderEnt->GetClosingTime() != nullptr);
                /*
                * 为了简化代码, 我们在此直接退出循环, 待下一次HandleOrder时再继续处理stop loss order 的检查.
                */
                automaton.openedOrders.erase(iter);
                break;
            }

            /* step 1 sell order */
            FetchOrder(hrOrderEnt);
            if (hrOrderEnt->GetClosingTime() != nullptr)
            {
                /* step 1 sell order filled */
                LOG4_INFO("order closed"
                    << ", order id=" << hrOrderEnt->GetId()
                    << ", order type=" << GetEnumName(hrOrderEnt->GetOrderType())
                    << ", price=" << hrOrderEnt->GetLimitedPrice()
                    << ", coin number=" << hrOrderEnt->GetCoinNumber());

                PrintAvailable(automaton.hrAccEnt0);
                PrintAvailable(automaton.hrAccEnt1);
                automaton.openedOrders.erase(iter);
                CastEvent(HrAccGrpEvent::SellOrderFilled, __func__, __LINE__);
                break;
            }
        }
        else
        {
            /* stop loss order */
            HandleStopLossOrder(hrOrderEnt);
			if (hrOrderEnt->GetClosingTime() != nullptr)
			{
				/*
				* 为了简化代码, 我们在此直接退出循环, 待下一次HandleOrder时再继续处理stop loss order 的检查.
				*/
				automaton.openedOrders.erase(iter);
				break;
			}
        }
    }
}

CxxEndNameSpace