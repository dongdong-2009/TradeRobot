#include "UtOcdcPlatform.h"

/* C++ */
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"
#include "Include/Entity/EntityQueryType.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Debug.h"

/* Functions */
#include "Include/Functions/TableIndexHelperInterface.h"
#include "Include/Functions/XmlWrapperInterface.h"
#include "Include/Functions/MarketPollerInterface.h"
#include "Include/Functions/PlatformInterface.h"

/* local header files */
#include "PriceHelperInterface.h"

#ifdef HasUtOcdcPlatform
using namespace std;

CxxBeginNameSpace(UnitTest)

/**********************UtOcdcPlatform**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtOcdcPlatform);

/* public function */
void UtOcdcPlatform::setUp()
{
    db = Database::GetInstance().GetDatabase();
    ResetSession();

    XmlWrapperInterface& xmlWrapper = XmlWrapperInterface::GetInstance();
    xmlWrapper.Read("UtPlatform.xml");

    StartTransaction(db, session, MaxTransactionRetryNumber);
    hrEnt = db->load<HrEntity>(1);
    hrAccGrpEnt = *hrEnt->GetAccountGroups().begin();

    hrAccEnt = hrAccGrpEnt->GetHrAccount0();
    ptmEnt = hrAccEnt->GetPlatform();
    EndTransaction();

    PriceHelperInterface& priceHelper = GetPriceHelperInterface();
    lowestPrice = priceHelper.GetLowestPrice(hrEnt->GetCoinType());
    highestPrice = priceHelper.GetHighestPrice(hrEnt->GetCoinType());

    minTradingUnit = hrEnt->GetMinTradingUnit();
}

/* protected function */
void UtOcdcPlatform::TestCreateBuyOrder()
{
    auto hrOrderEnt = make_shared<HrOrderEntity>(1, OrderType::Buy, lowestPrice, minTradingUnit);
    hrAccEnt->Bind(hrOrderEnt);

    CPPUNIT_ASSERT(GetPlatformInstance(ptmEnt).CreateOrder(hrOrderEnt));

    /* cancel order */
    GetPlatformInstance(ptmEnt).CancelOrder(hrOrderEnt);
}

void UtOcdcPlatform::TestCreateSellOrder()
{
    auto hrOrderEnt = make_shared<HrOrderEntity>(1, OrderType::Sell, highestPrice, CoinNumber(99999.0));
    hrAccEnt->Bind(hrOrderEnt);

    /* coin is not enought */
    CPPUNIT_ASSERT(!GetPlatformInstance(ptmEnt).CreateOrder(hrOrderEnt));
}

void UtOcdcPlatform::TestCancelOrder()
{
    /* id 8875894430 为之前cancel掉了的order, okcoin 不会这些order的历史信息。 fetch操作会
     * 返回“订单不存在”的错误。 我们的CancelOrder() 会忽略掉此错误， 认为此order已经closed。
     */
    auto hrOrderEnt = make_shared<HrOrderEntity>(1, OrderType::Buy, lowestPrice, minTradingUnit);
    hrOrderEnt->SetPlatformOrderId(8875894430ULL);
    hrAccEnt->Bind(hrOrderEnt);

    GetPlatformInstance(ptmEnt).CancelOrder(hrOrderEnt);
}

void UtOcdcPlatform::TestFetchOrder1()
{
    TableId id = TableIndexHelperInterface::GetInstance().GetUseableTableIndex("Order");
    auto hrOrderEnt = make_shared<HrOrderEntity>(id, OrderType::Buy, lowestPrice, minTradingUnit);
    hrAccEnt->Bind(hrOrderEnt);

    CPPUNIT_ASSERT(GetPlatformInstance(ptmEnt).CreateOrder(hrOrderEnt));

    GetPlatformInstance(ptmEnt).FetchOrder(hrOrderEnt);
    CPPUNIT_ASSERT(hrOrderEnt->GetClosingTime() == nullptr);
    CPPUNIT_ASSERT(hrOrderEnt->GetFilledCoinNumber() == CoinNumber::Zero());

    /* cancel order */
    GetPlatformInstance(ptmEnt).CancelOrder(hrOrderEnt);

    GetPlatformInstance(ptmEnt).FetchOrder(hrOrderEnt);
    CPPUNIT_ASSERT(hrOrderEnt->GetClosingTime() != nullptr);
}

void UtOcdcPlatform::TestFetchOrder2()
{
    /* 假设id 1111111111 为之前cancel掉了的order, okcoin 不会保存这些order的历史信息。 我们的程序
    * 不可能fetch 2天前cancel/closed的order， 因此okcoin platfrom fetch操作引发断言失败异常。
    */
    auto hrOrderEnt = make_shared<HrOrderEntity>(1, OrderType::Buy, lowestPrice, minTradingUnit);
    hrOrderEnt->SetPlatformOrderId(1111111111ULL);
    hrAccEnt->Bind(hrOrderEnt);

    //GetPlatformInstance(ptmEnt).FetchOrder(hrOrderEnt);
}

void UtOcdcPlatform::TestFetchUnknownOrder()
{
#define InvalidePlatformOrderId 0

    auto hrOrderEnt1 = make_shared<HrOrderEntity>(1, OrderType::Buy, lowestPrice, minTradingUnit);
    hrAccEnt->Bind(hrOrderEnt1);
    CPPUNIT_ASSERT(GetPlatformInstance(ptmEnt).CreateOrder(hrOrderEnt1));

    PlatformOrderId ptmOrderId = hrOrderEnt1->GetPlatformOrderId();
    hrOrderEnt1->SetPlatformOrderId(InvalidePlatformOrderId);
    GetPlatformInstance(ptmEnt).FetchUnknownOrder(hrOrderEnt1);
    CPPUNIT_ASSERT(hrOrderEnt1->GetPlatformOrderId() == ptmOrderId);

    auto hrOrderEnt2 = make_shared<HrOrderEntity>(2, OrderType::Buy, lowestPrice, minTradingUnit);
    hrAccEnt->Bind(hrOrderEnt2);
    CPPUNIT_ASSERT(!GetPlatformInstance(ptmEnt).FetchUnknownOrder(hrOrderEnt2));

    /* cancel order */
    GetPlatformInstance(ptmEnt).CancelOrder(hrOrderEnt1);
}

void UtOcdcPlatform::TestFetchBalance()
{
    Money cnyBalance;
    GetPlatformInstance(ptmEnt).FetchBalance(hrAccEnt, cnyBalance);
    CPPUNIT_ASSERT(cnyBalance != Money::Zero());

    CoinNumber btcBalance;
    GetPlatformInstance(ptmEnt).FetchBalance(hrAccEnt, hrEnt->GetCoinType(), btcBalance);
    CPPUNIT_ASSERT(btcBalance != CoinNumber::Zero());
}

void UtOcdcPlatform::TestMarketPoller()
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
void UtOcdcPlatform::ResetSession()
{
    Database::GetInstance().ResetSession();
    session = Database::GetInstance().GetSession();
}

CxxEndNameSpace
#endif //#ifdef HasUtPlatform
