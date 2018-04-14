#include "UtController.h"

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
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"
#include "Include/Entity/EntityQueryType.h"

/* Functions */
#include "Include/Functions/XmlWrapperInterface.h"
#include "Include/Functions/TableIndexHelperInterface.h"
#include "Include/Functions/PlatformInterface.h"
#include "Include/Functions/ControllerInterface.h"

/* local header files */

#ifdef HasUtController
using namespace std;

CxxBeginNameSpace(UnitTest);

/**********************UtController**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtController);

/* public function */
void UtController::setUp()
{
    db = Database::GetInstance().GetDatabase();
    ResetSession();

    XmlWrapperInterface& xmlWrapper = XmlWrapperInterface::GetInstance();
    xmlWrapper.Read("UtController.xml");

    StartTransaction(db, session, MaxTransactionRetryNumber);
    hrEnt = db->load<HrEntity>(1);
    hrAccGrpEnt = *hrEnt->GetAccountGroups().begin();
    EndTransaction();

    GetPlatformInstance(hrAccGrpEnt->GetHrAccount0()->GetPlatform()).ClearOrders();
    GetPlatformInstance(hrAccGrpEnt->GetHrAccount1()->GetPlatform()).ClearOrders();
}

/* protected function */
void UtController::TestStart1()
{
    ControllerInterface& controller = GetControllerInterface();
    controller.Start(hrEnt);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);

    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);
    controller.Stop();

    CPPUNIT_ASSERT(!hrAccGrpEnt->GetHrAccount0()->GetOrders().empty());
    auto orderEnts = hrAccGrpEnt->GetHrAccount0()->GetOrders();
    auto orderEnt = orderEnts.begin();
    auto hrOrderEnt = dynamic_pointer_cast<HrOrderEntity>(orderEnt->get_eager());
    CPPUNIT_ASSERT(hrOrderEnt->GetOrderType() == OrderType::Buy);

    auto ptmEnt0 = hrAccGrpEnt->GetHrAccount0()->GetPlatform();
    auto ptmMinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt->GetLimitedPrice() == ptm0AskPrice + ptmMinPriceSize);
    auto hrAccEnt0 = hrAccGrpEnt->GetHrAccount0();
    CPPUNIT_ASSERT(hrAccEnt0->GetAvailableMoney() == hrAccEnt0->GetInvestedMoney() - hrOrderEnt->GetFreezingAmount());
}

void UtController::TestStart2()
{
    ControllerInterface& controller = GetControllerInterface();
    controller.Start(hrEnt);

    Money ptm0BidPrice(103.0), ptm0AskPrice(104.0), ptm0LastPrice(103.0);
    Money ptm1BidPrice(100.0), ptm1AskPrice(101.0), ptm1LastPrice(100.0);

    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);
    controller.Stop();

    auto hrAccEnt0 = hrAccGrpEnt->GetHrAccount0();
    auto hrAccEnt1 = hrAccGrpEnt->GetHrAccount1();

    CPPUNIT_ASSERT(!hrAccEnt0->GetOrders().empty());
    auto orderEnts = hrAccEnt0->GetOrders();
    auto orderEnt = orderEnts.begin();
    auto hrOrderEnt = dynamic_pointer_cast<HrOrderEntity>(orderEnt->get_eager());
    CPPUNIT_ASSERT(hrOrderEnt->GetOrderType() == OrderType::Sell);

    CPPUNIT_ASSERT(hrAccEnt1->GetOrders().empty());
    HrPlatformPairEntity& hrPtmPairEnt = *hrEnt->GetPlatformPair(2, 1);
    CPPUNIT_ASSERT(hrPtmPairEnt.GetOpm() == HrNs::OderPricingMode::AskBidPrice);

    auto ptmEnt0 = hrAccGrpEnt->GetHrAccount0()->GetPlatform();
    auto ptmMinPriceSize = ptmEnt0->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt->GetLimitedPrice() == ptm0BidPrice - ptmMinPriceSize);

    CoinNumber coinNumber = hrEnt->GetMinTradingUnit();
    CPPUNIT_ASSERT(hrOrderEnt->GetCoinNumber() == coinNumber);
    CPPUNIT_ASSERT(hrAccEnt0->GetAvailableCoinNumber() == hrAccEnt0->GetInvestedCoinNumber() - coinNumber);
}

void UtController::TestStart3()
{
    ControllerInterface& controller = GetControllerInterface();
    controller.Start(hrEnt);

    Money ptm0BidPrice(100.0), ptm0AskPrice(102.0), ptm0LastPrice(100.0);
    Money ptm1BidPrice(103.0), ptm1AskPrice(104.0), ptm1LastPrice(103.0);

    SendMarket(ptm0BidPrice, ptm0AskPrice, ptm0LastPrice, ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);
    controller.Stop();

    /* close step 0 buy order, and create step 1 sell order */
    auto hrAccEnt0 = hrAccGrpEnt->GetHrAccount0();
    auto hrOrderEnt0 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt0->GetOrders().begin()->load());

    controller.Start(hrEnt);
    GetPlatformInstance(hrAccEnt0->GetPlatform()).SetOrder(hrOrderEnt0->GetId(), hrOrderEnt0->GetLimitedPrice());
    controller.Stop();

    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("Step1Selling") == 0);

    /* check hrAcc0Ent state */
    CPPUNIT_ASSERT(hrOrderEnt0->GetClosingTime() != nullptr);
    CPPUNIT_ASSERT(hrAccEnt0->GetAvailableCoinNumber() == hrAccEnt0->GetInvestedCoinNumber() + hrOrderEnt0->GetFilledCoinNumber());
    CPPUNIT_ASSERT(hrAccEnt0->GetAvailableMoney() == hrAccEnt0->GetInvestedMoney() - hrOrderEnt0->GetFreezingAmount());

    /* check hrAcc1Ent state */
    CoinNumber coinNumber = hrEnt->GetMinTradingUnit();

    auto hrAccEnt1 = hrAccGrpEnt->GetHrAccount1();
    CPPUNIT_ASSERT(!hrAccEnt1->GetOrders().empty());
    auto hrOrderEnt1 = dynamic_pointer_cast<HrOrderEntity>(hrAccEnt1->GetOrders().begin()->get_eager());
    CPPUNIT_ASSERT(hrOrderEnt1->GetOrderType() == OrderType::Sell);
    CPPUNIT_ASSERT(hrOrderEnt1->GetClosingTime() == nullptr);

    auto ptmEnt1 = hrAccGrpEnt->GetHrAccount1()->GetPlatform();
    auto ptmMinPriceSize = ptmEnt1->GetCoinRule(hrEnt->GetCoinType())->GetMinPriceSize();
    CPPUNIT_ASSERT(hrOrderEnt1->GetLimitedPrice() == ptm1BidPrice - ptmMinPriceSize);
    CPPUNIT_ASSERT(hrOrderEnt1->GetCoinNumber() == coinNumber);
    CPPUNIT_ASSERT(hrAccEnt1->GetAvailableCoinNumber() == hrAccEnt1->GetInvestedCoinNumber() - coinNumber);

    controller.Start(hrEnt);
    GetPlatformInstance(hrAccEnt1->GetPlatform()).SetOrder(hrOrderEnt1->GetId(), hrOrderEnt1->GetLimitedPrice());
    controller.Stop();
    CPPUNIT_ASSERT(hrOrderEnt1->GetClosingTime() != nullptr);
    CPPUNIT_ASSERT(hrAccGrpEnt->GetState().compare("NoOrder") == 0);
}

void UtController::TestPrintRuntimeStatus()
{
    ControllerInterface& controller = GetControllerInterface();
    cout << endl;

    controller.Start(hrEnt);
    controller.PrintRuntimeStatus(cout);
    controller.Stop();
}

/* private functions */
void UtController::ResetSession()
{
    Database::GetInstance().ResetSession();
    session = Database::GetInstance().GetSession();
}

void UtController::SendMarket(Money ptm0BidPrice, Money ptm0AskPrice, Money ptm0LastPrice,
    Money ptm1BidPrice, Money ptm1AskPrice, Money ptm1LastPrice)
{
    PlatformInterface& ptm0 = GetPlatformInstance(hrAccGrpEnt->GetHrAccount0()->GetPlatform());
    ptm0.SetMarket(hrEnt->GetCoinType(), ptm0BidPrice, ptm0AskPrice, ptm0LastPrice);

    PlatformInterface& ptm1 = GetPlatformInstance(hrAccGrpEnt->GetHrAccount1()->GetPlatform());
    ptm1.SetMarket(hrEnt->GetCoinType(), ptm1BidPrice, ptm1AskPrice, ptm1LastPrice);
}

CxxEndNameSpace;
#endif //#ifdef HasUtEntity
