#include "UtHrAccGrp.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"
#include "Include/Entity/EntityQueryType.h"

/* Functions */
#include "Include/Functions/TableIndexHelperInterface.h"
#include "Include/Functions/XmlWrapperInterface.h"
#include "Include/Functions/HrAccGrpInterface.h"
#include "Include/Functions/PlatformInterface.h"

/* local header files */

#ifdef HasUtHrAccGrp
using namespace std;

CxxBeginNameSpace(UnitTest);

/**********************UtHrAccGrp**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtHrAccGrp);
/* public function */
void UtHrAccGrp::setUp()
{
    db = Database::GetInstance().GetDatabase();
    ResetSession();

    XmlWrapperInterface& xmlWrapper = XmlWrapperInterface::GetInstance();
    xmlWrapper.Read("UtHrAccGrp.xml");

	StartTransaction(db, session, MaxTransactionRetryNumber);
    hrEnt = db->load<HrEntity>(1);
    hrAccGrpEnt = *hrEnt->GetAccountGroups().begin();
    hrAccEnt0 = hrAccGrpEnt->GetHrAccount0();
    hrAccEnt1 = hrAccGrpEnt->GetHrAccount1();
    ptmEnt0 = hrAccEnt0->GetPlatform();
    ptmEnt1 = hrAccEnt1->GetPlatform();
    ptm0CoinRuleEnt = ptmEnt0->GetCoinRule(hrEnt->GetCoinType());
    ptm1CoinRuleEnt = ptmEnt1->GetCoinRule(hrEnt->GetCoinType());
    EndTransaction();

    GetPlatformInstance(ptmEnt0).ClearOrders();
    GetPlatformInstance(ptmEnt1).ClearOrders();
}

/* protected function */
void UtHrAccGrp::TestStart()
{
    shared_ptr<HrAccGrpInterface> hrAccGrp(HrAccGrpInterface::CreateInstance(hrAccGrpEnt));
    CPPUNIT_ASSERT(!hrAccGrp->IsRunning());
    hrAccGrp->Start();
    CPPUNIT_ASSERT(hrAccGrp->IsRunning());
    hrAccGrp->Stop();
    CPPUNIT_ASSERT(!hrAccGrp->IsRunning());

    hrAccEnt0->SetInvestedMoney(Money::Zero());
    hrAccEnt0->SetInvestedCoinNumber(CoinNumber::Zero());
    hrAccEnt1->SetInvestedMoney(Money::Zero());
    hrAccEnt1->SetInvestedCoinNumber(CoinNumber::Zero());

	StartTransaction(db, session, MaxTransactionRetryNumber);
    db->update(hrAccEnt0);
    db->update(hrAccEnt1);
    EndTransaction();

    hrAccGrp->Start();

    CPPUNIT_ASSERT(hrAccEnt0->GetInvestedMoney() != Money::Zero());
    CPPUNIT_ASSERT(hrAccEnt0->GetInvestedCoinNumber() != CoinNumber::Zero());
    CPPUNIT_ASSERT(hrAccEnt1->GetInvestedMoney() != Money::Zero());
    CPPUNIT_ASSERT(hrAccEnt1->GetInvestedCoinNumber() != CoinNumber::Zero());
    hrAccGrp->Stop();
}

void UtHrAccGrp::TestHandleMarket1()
{
    Money bidPrice(100.0), askPrice(102.0), lastPrice(100.0);
    SendMarket(bidPrice, askPrice, lastPrice, bidPrice, askPrice, lastPrice);

    CPPUNIT_ASSERT(hrAccEnt0->GetOrders().empty());
    CPPUNIT_ASSERT(hrAccEnt1->GetOrders().empty());
}

void UtHrAccGrp::TestHandleMarket2()
{
    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

	CPPUNIT_ASSERT(!hrAccEnt0->GetOrders().empty());
    auto orderEnts = hrAccEnt0->GetOrders();
    auto orderEnt = orderEnts.begin();
    auto hrOrderEnt = dynamic_pointer_cast<HrOrderEntity>(orderEnt->get_eager());
    CPPUNIT_ASSERT(hrOrderEnt->GetOrderType() == OrderType::Buy);

	CPPUNIT_ASSERT(hrAccEnt1->GetOrders().empty());
    HrPlatformPairEntity& hrPtmPairEnt = *hrEnt->GetPlatformPair(2, 1);
    CPPUNIT_ASSERT(hrPtmPairEnt.GetOpm() == HrNs::OderPricingMode::AskBidPrice);

    auto ptmMinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt->GetLimitedPrice() == ptm0AskPrice + ptmMinPriceSize);
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt0->GetAvailableMoney() == hrAccEnt0->GetInvestedMoney() - hrOrderEnt->GetFreezingAmount() - Money(1));
    CPPUNIT_ASSERT(hrAccEnt0->GetAvailableCoinNumber() == hrAccEnt0->GetInvestedCoinNumber() - CoinNumber(0.001));
}

void UtHrAccGrp::TestHandleMarket3()
{
	/* update available money */
	hrAccEnt0->SetInvestedMoney(Money(0.01));
	UpdateEntity(hrAccEnt0);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    CPPUNIT_ASSERT(hrAccEnt0->GetOrders().empty());
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt0->GetAvailableMoney() == Money(0.01) - Money(1.0));
}

void UtHrAccGrp::TestHandleMarket4()
{
    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(101.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    CPPUNIT_ASSERT(!hrAccEnt0->GetOrders().empty());
    auto orderEnts = hrAccEnt0->GetOrders();
    auto orderEnt = orderEnts.begin();
    auto hrOrderEnt = dynamic_pointer_cast<HrOrderEntity>(orderEnt->get_eager());
    CPPUNIT_ASSERT(hrOrderEnt->GetOrderType() == OrderType::Sell);

    CPPUNIT_ASSERT(hrAccEnt1->GetOrders().empty());
    HrPlatformPairEntity& hrPtmPairEnt = *hrEnt->GetPlatformPair(2, 1);
    CPPUNIT_ASSERT(hrPtmPairEnt.GetOpm() == HrNs::OderPricingMode::AskBidPrice);

    auto ptmMinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt->GetLimitedPrice() == ptm0BidPrice - ptmMinPriceSize);

    CoinNumber coinNumber = hrEnt->GetMinTradingUnit();
    CPPUNIT_ASSERT(hrOrderEnt->GetCoinNumber() == coinNumber);
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
	CPPUNIT_ASSERT(hrAccEnt0->GetAvailableMoney() == hrAccEnt0->GetInvestedMoney() - 1.0);
	CPPUNIT_ASSERT(hrAccEnt0->GetAvailableCoinNumber() == hrAccEnt0->GetInvestedCoinNumber() - coinNumber - 0.001);
}

void UtHrAccGrp::TestHandleMarket5()
{
	/* update available coin */
	hrAccEnt0->SetInvestedCoinNumber(CoinNumber(0.001));
	UpdateEntity(hrAccEnt0);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(101.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    CPPUNIT_ASSERT(hrAccEnt0->GetOrders().empty());
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt0->GetAvailableCoinNumber() == CoinNumber(0.001) - CoinNumber(0.001));
}

void UtHrAccGrp::TestHandleMarket6()
{
    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    /* SendMarket to cancel step 0 buying order. */
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm0BidPrice, ptm0AskPrice, ptm0LastPrice);
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("NoOrder") == 0);

    auto hrAccEnt = hrAccEnt0;
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt->GetAvailableMoney() == hrAccEnt->GetInvestedMoney() - 1.0);
    CPPUNIT_ASSERT(hrAccEnt->GetAvailableCoinNumber() == hrAccEnt->GetInvestedCoinNumber() - 0.001);
}

void UtHrAccGrp::TestHandleMarket7()
{
    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(101.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    /* SendMarket to cancel step 0 buying order. */
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm0BidPrice, ptm0AskPrice, ptm0LastPrice);
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("NoOrder") == 0);

    auto hrAccEnt = hrAccEnt0;
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt->GetAvailableMoney() == hrAccEnt->GetInvestedMoney() - 1.0);
    CPPUNIT_ASSERT(hrAccEnt->GetAvailableCoinNumber() == hrAccEnt->GetInvestedCoinNumber() - 0.001);
}

void UtHrAccGrp::TestHandleOrder1()
{
    /* Create step 0 buy order. */
    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    /* close step 0 buy order, and create step 1 sell order */
    auto hrOrderEnt = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->load());
    SetOrder(hrOrderEnt, hrOrderEnt->GetLimitedPrice());

    /* step 1 sell order timeout, conver to "NoOrder" state */
    auto secs = hrEnt->GetOrder2ndTimeOut();
    list<lazy_shared_ptr<OrderEntity>> orderEnts;
    copy(hrAccEnt0->GetOrders().begin(), hrAccEnt0->GetOrders().end(), back_inserter(orderEnts));
    copy(hrAccEnt1->GetOrders().begin(), hrAccEnt1->GetOrders().end(), back_inserter(orderEnts));
    StartTransaction(db, session, MaxTransactionRetryNumber);
    for (auto i = orderEnts.begin(); i != orderEnts.end(); ++i)
    {
        hrOrderEnt = dynamic_pointer_cast<HrOrderEntity>(i->load());
        hrOrderEnt->SetCreatingTime(hrOrderEnt->GetCreatingTime() - secs);

        if (hrOrderEnt->GetClosingTime() != nullptr)
            hrOrderEnt->SetClosingTime(*hrOrderEnt->GetClosingTime() - secs);

        if (hrOrderEnt->GetPriceScheduleTime() != nullptr)
            hrOrderEnt->SetPriceScheduleTime(*hrOrderEnt->GetPriceScheduleTime() - secs);
        db->update(hrOrderEnt);
    }
    EndTransaction();
    hrOrderEnt = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->load());
    SetOrder(hrOrderEnt);
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("NoOrder") == 0);

    /* close stop loss order */
    auto iter = hrAccEnt1->GetOrders().begin();
    auto hrOrderEnt2 = dynamic_pointer_cast<HrOrderEntity>((iter++)->load());
    auto hrOrderEnt3 = dynamic_pointer_cast<HrOrderEntity>((iter++)->load());
    SetOrder(hrOrderEnt3, hrOrderEnt3->GetLimitedPrice());

    auto transFeeRate = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetTransactionFeeRate();
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("NoOrder") == 0);
    auto money = (hrOrderEnt2->GetFilledCoinAmount() + hrOrderEnt3->GetFilledCoinAmount()) * (1 - transFeeRate);
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableMoney() == hrAccEnt1->GetInvestedMoney() + money - 1.0);
    auto coinNumber =  hrOrderEnt2->GetFilledCoinNumber() + hrOrderEnt3->GetFilledCoinNumber();
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableCoinNumber() == hrAccEnt1->GetInvestedCoinNumber() - coinNumber - 0.001);
}

void UtHrAccGrp::TestHandleOrder2()
{
    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(101.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    /* close step 0 sell order, and create step 1 buy order */
    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->load());
    SetOrder(hrOrderEnt1, hrOrderEnt1->GetLimitedPrice());

    /* step 1 buy order timeout, conver to "NoOrder" state */
    auto secs = hrEnt->GetOrder2ndTimeOut();
    list<lazy_shared_ptr<OrderEntity>> orderEnts;
    copy(hrAccEnt0->GetOrders().begin(), hrAccEnt0->GetOrders().end(), back_inserter(orderEnts));
    copy(hrAccEnt1->GetOrders().begin(), hrAccEnt1->GetOrders().end(), back_inserter(orderEnts));
	StartTransaction(db, session, MaxTransactionRetryNumber);
    for (auto i = orderEnts.begin(); i != orderEnts.end(); ++i)
    {
        auto hrOrderEnt = dynamic_pointer_cast<HrOrderEntity>(i->load());
        hrOrderEnt->SetCreatingTime(hrOrderEnt->GetCreatingTime() - secs);

        if (hrOrderEnt->GetClosingTime() != nullptr)
            hrOrderEnt->SetClosingTime(*hrOrderEnt->GetClosingTime() - secs);

        if (hrOrderEnt->GetPriceScheduleTime() != nullptr)
            hrOrderEnt->SetPriceScheduleTime(*hrOrderEnt->GetPriceScheduleTime() - secs);
        db->update(hrOrderEnt);
    }
    EndTransaction();
    auto hrOrderEnt2 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->load());
    SetOrder(hrOrderEnt2);
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("NoOrder") == 0);

    /* close stop loss order */
    auto iter = hrAccEnt1->GetOrders().begin();
    hrOrderEnt2 = dynamic_pointer_cast<HrOrderEntity>((iter++)->load());
    auto hrOrderEnt3 = dynamic_pointer_cast<HrOrderEntity>((iter++)->load());
    SetOrder(hrOrderEnt3, hrOrderEnt3->GetLimitedPrice());

    auto transFeeRate = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetTransactionFeeRate();
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("NoOrder") == 0);
    auto money = hrOrderEnt2->GetFilledCoinAmount() + hrOrderEnt3->GetFilledCoinAmount();
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableMoney() == hrAccEnt1->GetInvestedMoney() - money - 1.0);
    auto coinNumber = (hrOrderEnt2->GetFilledCoinNumber() + hrOrderEnt3->GetFilledCoinNumber()) * (1 - transFeeRate);
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableCoinNumber() == hrAccEnt1->GetInvestedCoinNumber() + coinNumber - 0.001);
}

void UtHrAccGrp::TestHandleOrder3()
{
    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("Step1Selling") == 0);

    /* check hrAcc0Ent state */
    CPPUNIT_ASSERT(hrOrderEnt0->GetClosingTime() != nullptr);
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
	CPPUNIT_ASSERT(hrAccEnt0->GetAvailableMoney() == hrAccEnt0->GetInvestedMoney() - hrOrderEnt0->GetFreezingAmount() - Money(1.0));
    CoinNumber takehomeCoinNumber = hrOrderEnt0->GetFilledCoinNumber() * (1 - ptm0CoinRuleEnt->GetTransactionFeeRate());
    CPPUNIT_ASSERT(hrAccEnt0->GetAvailableCoinNumber() == hrAccEnt0->GetInvestedCoinNumber() + takehomeCoinNumber - CoinNumber(0.001));

    /* check hrAcc1Ent state */
    CoinNumber coinNumber = hrEnt->GetMinTradingUnit();

    CPPUNIT_ASSERT(!hrAccEnt1->GetOrders().empty());
    auto hrOrderEnt10 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    CPPUNIT_ASSERT(hrOrderEnt10->GetOrderType() == OrderType::Sell);
    CPPUNIT_ASSERT(hrOrderEnt10->GetClosingTime() == nullptr);

    auto ptmMinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt10->GetLimitedPrice() == ptm1BidPrice - ptmMinPriceSize);
    CPPUNIT_ASSERT(hrOrderEnt10->GetCoinNumber() == coinNumber);
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableCoinNumber() == hrAccEnt1->GetInvestedCoinNumber() - coinNumber - CoinNumber(0.001));

    /* step 1 sell order filled */
    SetOrder(hrOrderEnt10, hrOrderEnt10->GetLimitedPrice());
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("NoOrder") == 0);

    CPPUNIT_ASSERT(hrOrderEnt10->GetClosingTime() != nullptr);
    CPPUNIT_ASSERT(hrOrderEnt10->GetCanceledReason() == nullptr);
    CPPUNIT_ASSERT(hrOrderEnt10->GetLimitedPrice() == hrOrderEnt10->GetConcludedPrice());
    CPPUNIT_ASSERT(hrOrderEnt10->GetCoinNumber() == hrOrderEnt10->GetFilledCoinNumber());

    auto transFeeRate = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetTransactionFeeRate();
    auto money = hrOrderEnt10->GetFilledCoinAmount() * (1 - transFeeRate);
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableMoney() == hrAccEnt1->GetInvestedMoney() + money - 1.0);
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableCoinNumber() == hrAccEnt1->GetInvestedCoinNumber() - hrOrderEnt10->GetFilledCoinNumber() - 0.001);
}

void UtHrAccGrp::TestHandleOrder4()
{
    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(101.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("Step1Buying") == 0);

    /* check hrAcc0Ent state */
    CPPUNIT_ASSERT(hrOrderEnt0->GetClosingTime() != nullptr);
    auto transFeeRate0 = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetTransactionFeeRate();
    auto money = hrOrderEnt0->GetFilledCoinAmount() * (1 - transFeeRate0);
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
	CPPUNIT_ASSERT(hrAccEnt0->GetAvailableMoney() == hrAccEnt0->GetInvestedMoney() + money - Money(1.0));
	CPPUNIT_ASSERT(hrAccEnt0->GetAvailableCoinNumber() == hrAccEnt0->GetInvestedCoinNumber() - hrOrderEnt0->GetFilledCoinNumber() - CoinNumber(0.001));

    /* check hrAcc1Ent state */
    CPPUNIT_ASSERT(!hrAccEnt1->GetOrders().empty());
    auto hrOrderEnt10 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    CPPUNIT_ASSERT(hrOrderEnt10->GetOrderType() == OrderType::Buy);
    CPPUNIT_ASSERT(hrOrderEnt10->GetClosingTime() == nullptr);

    auto ptmMinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt10->GetLimitedPrice() == ptm1AskPrice + ptmMinPriceSize);
    CPPUNIT_ASSERT(hrOrderEnt10->GetCoinNumber() == hrEnt->GetMinTradingUnit());

    /* step 1 buy order filled */
    SetOrder(hrOrderEnt10, hrOrderEnt10->GetLimitedPrice());
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("NoOrder") == 0);

    CPPUNIT_ASSERT(hrOrderEnt10->GetClosingTime() != nullptr);
    CPPUNIT_ASSERT(hrOrderEnt10->GetCanceledReason() == nullptr);
    CPPUNIT_ASSERT(hrOrderEnt10->GetLimitedPrice() == hrOrderEnt10->GetConcludedPrice());
    CPPUNIT_ASSERT(hrOrderEnt10->GetCoinNumber() == hrOrderEnt10->GetFilledCoinNumber());

    auto transFeeRate1 = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetTransactionFeeRate();
    auto coinNumber = hrOrderEnt10->GetFilledCoinNumber() * (1 - transFeeRate1);
	//Money(1), CoinNumber(0.001) 是HrGrpAlgorithm 引入的误差冗余.
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableMoney() == hrAccEnt1->GetInvestedMoney() - hrOrderEnt10->GetFilledCoinAmount() - Money(1.0));
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableCoinNumber() == hrAccEnt1->GetInvestedCoinNumber() + coinNumber - CoinNumber(0.001));
}

void UtHrAccGrp::TestOrderPrice01()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt0->GetId(), ptmEnt1->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::AskAskPrice);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0AskPrice + ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1AskPrice - ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice02()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt0->GetId(), ptmEnt1->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::AskBidPrice);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0AskPrice + ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1BidPrice - ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice03()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt0->GetId(), ptmEnt1->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::AskLastPrice);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0AskPrice + ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1LastPrice - ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice04()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt0->GetId(), ptmEnt1->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::BidAskPrice);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0BidPrice + ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1AskPrice - ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice05()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt0->GetId(), ptmEnt1->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::BidBidPrice);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0BidPrice + ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1BidPrice - ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice06()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt0->GetId(), ptmEnt1->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::BidLastPrice);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0BidPrice + ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1LastPrice - ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice07()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt0->GetId(), ptmEnt1->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::LastAskPrice);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0LastPrice + ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1AskPrice - ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice08()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt0->GetId(), ptmEnt1->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::LastBidPrice);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0LastPrice + ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1BidPrice - ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice09()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt0->GetId(), ptmEnt1->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::LastLastPrice);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0LastPrice + ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1LastPrice - ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice11()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt1->GetId(), ptmEnt0->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::AskAskPrice);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(102.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0AskPrice - ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1AskPrice + ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice12()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt1->GetId(), ptmEnt0->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::AskBidPrice);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(102.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0BidPrice - ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1AskPrice + ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice13()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt1->GetId(), ptmEnt0->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::AskBidPrice);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(102.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0LastPrice - ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1AskPrice + ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice14()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt1->GetId(), ptmEnt0->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::BidAskPrice);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(102.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0AskPrice - ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1BidPrice + ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice15()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt1->GetId(), ptmEnt0->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::BidBidPrice);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(102.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0BidPrice - ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1BidPrice + ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice16()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt1->GetId(), ptmEnt0->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::BidLastPrice);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(102.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0LastPrice - ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1BidPrice + ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice17()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt1->GetId(), ptmEnt0->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::LastAskPrice);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(102.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0AskPrice - ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1LastPrice + ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice18()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt1->GetId(), ptmEnt0->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::LastBidPrice);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(102.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0BidPrice - ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1LastPrice + ptm1MinPriceSize);
}

void UtHrAccGrp::TestOrderPrice19()
{
    auto hrPtmPairEnt = hrEnt->GetPlatformPair(ptmEnt1->GetId(), ptmEnt0->GetId());
    hrPtmPairEnt->SetOpm(HrNs::OderPricingMode::LastLastPrice);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(102.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());

    auto ptm0MinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt0->GetLimitedPrice() == ptm0LastPrice - ptm0MinPriceSize);

    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    auto ptm1MinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1LastPrice + ptm1MinPriceSize);
}

void UtHrAccGrp::TestRestartHrAccGrp1()
{
    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    ResetSession();
    StartTransaction(db, session, MaxTransactionRetryNumber);
    hrEnt = db->load<HrEntity>(1);
    hrAccGrpEnt = *hrEnt->GetAccountGroups().begin();
    hrAccEnt0 = hrAccGrpEnt->GetHrAccount0();
    hrAccEnt1 = hrAccGrpEnt->GetHrAccount1();
    ptmEnt0 = hrAccEnt0->GetPlatform();
    ptmEnt1 = hrAccEnt1->GetPlatform();
    EndTransaction();

    shared_ptr<HrAccGrpInterface> hrAccGrp(HrAccGrpInterface::CreateInstance(hrAccGrpEnt));
    hrAccGrp->Start();
    hrAccGrp->Stop();
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("Step0Buying") == 0);
}

void UtHrAccGrp::TestRestartHrAccGrp2()
{
    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(101.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    shared_ptr<HrAccGrpInterface> hrAccGrp(HrAccGrpInterface::CreateInstance(hrAccGrpEnt));
    hrAccGrp->Start();
    hrAccGrp->Stop();
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("Step0Selling") == 0);
}

void UtHrAccGrp::TestRestartHrAccGrp3()
{
    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("Step1Selling") == 0);

    shared_ptr<HrAccGrpInterface> hrAccGrp(HrAccGrpInterface::CreateInstance(hrAccGrpEnt));
    hrAccGrp->Start();
    hrAccGrp->Stop();
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("Step1Selling") == 0);
}

void UtHrAccGrp::TestRestartHrAccGrp4()
{
    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(101.0), ptm1LastPrice(100.0);
    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->get_eager());

    /* close order, and waiting for order to be handled */
    SetOrder(hrOrderEnt0, hrOrderEnt0->GetLimitedPrice());
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("Step1Buying") == 0);

    shared_ptr<HrAccGrpInterface> hrAccGrp(HrAccGrpInterface::CreateInstance(hrAccGrpEnt));
    hrAccGrp->Start();
    hrAccGrp->Stop();
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("Step1Buying") == 0);
}

/* private functions */
void UtHrAccGrp::ResetSession()
{
    Database::GetInstance().ResetSession();
    session = Database::GetInstance().GetSession();
}

UtHrAccGrp::MarketEntityPair UtHrAccGrp::CreateMarket(Money ptm0BidPrice, Money ptm0AskPrice, Money ptm0LastPrice,
    Money ptm1BidPrice, Money ptm1AskPrice, Money ptm1LastPrice)
{
    shared_ptr<MarketEntity> ptm0MarketEnt, ptm1MarketEnt;

    TableId id = TableIndexHelperInterface::GetInstance().GetUseableTableIndex("Market");
    ptm0MarketEnt = make_shared<MarketEntity>(id, CoinType::Btc, ptm0BidPrice, ptm0AskPrice, ptm0LastPrice);
    ptm0MarketEnt->SetPlatform(ptmEnt0);

    id = TableIndexHelperInterface::GetInstance().GetUseableTableIndex("Market");
    ptm1MarketEnt = make_shared<MarketEntity>(id, CoinType::Btc, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);
    ptm1MarketEnt->SetPlatform(ptmEnt1);

    StartTransaction(db, session, MaxTransactionRetryNumber);
    db->persist(ptm0MarketEnt);
    db->persist(ptm1MarketEnt);
    EndTransaction();

    return make_pair(ptm0MarketEnt, ptm1MarketEnt);
}

void UtHrAccGrp::SendMarket(Money ptm0BidPrice, Money ptm0AskPrice, Money ptm0LastPrice,
    Money ptm1BidPrice, Money ptm1AskPrice, Money ptm1LastPrice)
{
    auto marketEnts = CreateMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice,
        ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);

    shared_ptr<HrAccGrpInterface> hrAccGrp(HrAccGrpInterface::CreateInstance(hrAccGrpEnt));
    hrAccGrp->Start();
    hrAccGrp->HandleMarket(marketEnts.first);
    hrAccGrp->HandleMarket(marketEnts.second);
    hrAccGrp->Stop();
}

void UtHrAccGrp::SetOrder(shared_ptr<HrOrderEntity> hrOrderEnt)
{
    shared_ptr<PlatformEntity> ptmEnt = hrOrderEnt->GetPlatform();

    shared_ptr<HrAccGrpInterface> hrAccGrp(HrAccGrpInterface::CreateInstance(hrAccGrpEnt));
    hrAccGrp->Start();
    GetPlatformInstance(ptmEnt).SetOrder(hrOrderEnt->GetId());
    hrAccGrp->Stop();
}

void UtHrAccGrp::SetOrder(shared_ptr<HrOrderEntity> hrOrderEnt, Money concludedPrice)
{
    shared_ptr<PlatformEntity> ptmEnt = hrOrderEnt->GetPlatform();

    shared_ptr<HrAccGrpInterface> hrAccGrp(HrAccGrpInterface::CreateInstance(hrAccGrpEnt));
    hrAccGrp->Start();
    GetPlatformInstance(ptmEnt).SetOrder(hrOrderEnt->GetId(), concludedPrice);
    hrAccGrp->Stop();
}

CxxEndNameSpace
#endif