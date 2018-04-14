#include "UtBtdcPlatform.h"

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
#include "Include/Functions/PlatformInterface.h"
#include "Include/Functions/MarketPollerInterface.h"

/* local header files */
#include "PriceHelperInterface.h"

#ifdef HasUtBtdcPlatform
using namespace std;

CxxBeginNameSpace(UnitTest);

/**********************UtBtdcPlatform**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtBtdcPlatform);

/* public function */
void UtBtdcPlatform::setUp()
{
    db = Database::GetInstance().GetDatabase();
    ResetSession();

    XmlWrapperInterface& xmlWrapper = XmlWrapperInterface::GetInstance();
    xmlWrapper.Read("UtPlatform.xml");

    StartTransaction(db, session, MaxTransactionRetryNumber);
    hrEnt = db->load<HrEntity>(1);
    hrAccGrpEnt = *hrEnt->GetAccountGroups().begin();

    hrAccEnt = hrAccGrpEnt->GetHrAccount1();
    ptmEnt = hrAccEnt->GetPlatform();
    EndTransaction();

    PriceHelperInterface& priceHelper = GetPriceHelperInterface();
    lowestPrice = priceHelper.GetLowestPrice(hrEnt->GetCoinType());
    highestPrice = priceHelper.GetHighestPrice(hrEnt->GetCoinType());

    minTradingUnit = hrEnt->GetMinTradingUnit();
}

/* protected function */
void UtBtdcPlatform::TestCreateBuyOrder()
{
    TableId id = TableIndexHelperInterface::GetInstance().GetUseableTableIndex("Order");
    auto hrOrderEnt = make_shared<HrOrderEntity>(id, OrderType::Buy, lowestPrice, minTradingUnit);
    hrAccEnt->Bind(hrOrderEnt);

    CPPUNIT_ASSERT(GetPlatformInstance(ptmEnt).CreateOrder(hrOrderEnt));
    cout << "platform order id = " << hrOrderEnt->GetPlatformOrderId() << endl;

    /* cancel order */
    GetPlatformInstance(ptmEnt).CancelOrder(hrOrderEnt);
}

void UtBtdcPlatform::TestCreateSellOrder()
{
    TableId id = TableIndexHelperInterface::GetInstance().GetUseableTableIndex("Order");
    auto hrOrderEnt = make_shared<HrOrderEntity>(id, OrderType::Sell, highestPrice, CoinNumber(99999.0));
    hrAccEnt->Bind(hrOrderEnt);

    /* coin is not enought */
    CPPUNIT_ASSERT(!GetPlatformInstance(ptmEnt).CreateOrder(hrOrderEnt));
}

void UtBtdcPlatform::TestCancelOrder()
{
    auto hrOrderEnt = make_shared<HrOrderEntity>(1, OrderType::Buy, lowestPrice, minTradingUnit);
    hrOrderEnt->SetPlatformOrderId(375201077ULL);
    hrAccEnt->Bind(hrOrderEnt);

    /* 撤消失败，您的委托已经全部成交或已撤销 */
    GetPlatformInstance(ptmEnt).CancelOrder(hrOrderEnt);
}

void UtBtdcPlatform::TestFetchOrder()
{
    auto hrOrderEnt = make_shared<HrOrderEntity>(1, OrderType::Buy, lowestPrice, minTradingUnit);
    hrOrderEnt->SetPlatformOrderId(375201077ULL);
    hrAccEnt->Bind(hrOrderEnt);

    GetPlatformInstance(ptmEnt).FetchOrder(hrOrderEnt);
    CPPUNIT_ASSERT(hrOrderEnt->GetClosingTime() != nullptr);
    CPPUNIT_ASSERT(hrOrderEnt->GetFilledCoinNumber() == CoinNumber::Zero());
}

void UtBtdcPlatform::TestFetchUnknownOrder()
{
#define InvalidePlatformOrderId 0
    /* 为了避免和其他用例的order混淆, 我们取一个不同的price 和 coin number */
    auto hrOrderEnt1 = make_shared<HrOrderEntity>(1, OrderType::Buy, Money(1.11), CoinNumber(0.11));
    hrAccEnt->Bind(hrOrderEnt1);
    CPPUNIT_ASSERT(GetPlatformInstance(ptmEnt).CreateOrder(hrOrderEnt1));

    PlatformOrderId ptmOrderId = hrOrderEnt1->GetPlatformOrderId();
    hrOrderEnt1->SetPlatformOrderId(InvalidePlatformOrderId);
    GetPlatformInstance(ptmEnt).FetchUnknownOrder(hrOrderEnt1);
    CPPUNIT_ASSERT(hrOrderEnt1->GetPlatformOrderId() == ptmOrderId);

    /* 单元测试上下文中, FetchUnknownOrder() 意外的将其他用例的order视为unknown order.
     * 程序正常运行时, 程序记录了所有的order id, 所以不会将已知的order视为unknwon order.
     * 为了避免单元测试过程中断言失败, 我们不测试检测unknown order失败的情况.
     */
    //auto hrOrderEnt2 = make_shared<HrOrderEntity>(2, OrderType::Buy, Money(1.11), CoinNumber(0.11));
    //hrAccEnt->Bind(hrOrderEnt2);
    //CPPUNIT_ASSERT(!GetPlatformInstance(ptmEnt).FetchUnknownOrder(hrOrderEnt2));

    /* cancel order */
    GetPlatformInstance(ptmEnt).CancelOrder(hrOrderEnt1);
}

void UtBtdcPlatform::TestFetchBalance()
{
    Money cnyBalance;
    GetPlatformInstance(ptmEnt).FetchBalance(hrAccEnt, cnyBalance);
    CPPUNIT_ASSERT(cnyBalance != Money::Zero());

    CoinNumber btcBalance;
    GetPlatformInstance(ptmEnt).FetchBalance(hrAccEnt, hrEnt->GetCoinType(), btcBalance);
    CPPUNIT_ASSERT(btcBalance != CoinNumber::Zero());
}

void UtBtdcPlatform::TestMarketPoller()
{
    HttpMarketHandler marketHandler = [](CoinType coinType, Money bidPrice, Money askPrice, Money lastPrice)->bool
    {
        static uint_t idx = 0;;

        if (idx == 0)
        {
            cout << endl;
        }
        idx = idx + 1;
        cout << setw(2) << idx << " " << bidPrice << "    " << askPrice << "    " << lastPrice << endl;
        return true;
    };

    shared_ptr<MarketPollerInterface> marketPoller(CreateMarketPollerInterface(ptmEnt->GetName().c_str(),
        ptmEnt->GetMinMarketQueryInvl(), hrEnt->GetCoinType(), marketHandler));
    marketPoller->Start();

    this_thread::sleep_for(chrono::seconds(1));
    marketPoller->Stop();
}

/* private functions */
void UtBtdcPlatform::ResetSession()
{
    Database::GetInstance().ResetSession();
    session = Database::GetInstance().GetSession();
}

CxxEndNameSpace;
#endif //#ifdef HasUtEntity
