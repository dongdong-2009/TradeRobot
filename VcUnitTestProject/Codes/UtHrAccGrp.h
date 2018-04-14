#ifndef _UtHrAccGrp_h_
#define _UtHrAccGrp_h_

#include <cppunit/extensions/HelperMacros.h>

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"

#include "UtModule.h"
#ifdef HasUtHrAccGrp

CxxBeginNameSpace(UnitTest)

/**********************UtHrAccGrp**********************/
class UtHrAccGrp : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtHrAccGrp);
    //CPPUNIT_TEST(TestStart);
    CPPUNIT_TEST(TestHandleMarket1);
    CPPUNIT_TEST(TestHandleMarket2);
    CPPUNIT_TEST(TestHandleMarket3);
    CPPUNIT_TEST(TestHandleMarket4);
    CPPUNIT_TEST(TestHandleMarket5);
    CPPUNIT_TEST(TestHandleMarket6);
    CPPUNIT_TEST(TestHandleMarket7);

    CPPUNIT_TEST(TestHandleOrder1);
    CPPUNIT_TEST(TestHandleOrder2);
    CPPUNIT_TEST(TestHandleOrder3);
    CPPUNIT_TEST(TestHandleOrder4);

    CPPUNIT_TEST(TestOrderPrice01);
    CPPUNIT_TEST(TestOrderPrice02);
    CPPUNIT_TEST(TestOrderPrice03);
    CPPUNIT_TEST(TestOrderPrice04);
    CPPUNIT_TEST(TestOrderPrice05);
    CPPUNIT_TEST(TestOrderPrice06);
    CPPUNIT_TEST(TestOrderPrice07);
    CPPUNIT_TEST(TestOrderPrice08);
    CPPUNIT_TEST(TestOrderPrice09);

    CPPUNIT_TEST(TestOrderPrice11);
    CPPUNIT_TEST(TestOrderPrice12);
    CPPUNIT_TEST(TestOrderPrice13);
    CPPUNIT_TEST(TestOrderPrice14);
    CPPUNIT_TEST(TestOrderPrice15);
    CPPUNIT_TEST(TestOrderPrice16);
    CPPUNIT_TEST(TestOrderPrice17);
    CPPUNIT_TEST(TestOrderPrice18);
    CPPUNIT_TEST(TestOrderPrice19);

    CPPUNIT_TEST(TestRestartHrAccGrp1);
    CPPUNIT_TEST(TestRestartHrAccGrp2);
    CPPUNIT_TEST(TestRestartHrAccGrp3);
    CPPUNIT_TEST(TestRestartHrAccGrp4);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestStart();
    //now order will be created. HrNs::OderPricingMode::AskBidPrice
    void TestHandleMarket1();

    //step0 buy order will be created. HrNs::OderPricingMode::AskBidPrice
    void TestHandleMarket2();
    //money is not enough
    void TestHandleMarket3();

    //step0 sell order will be created. HrNs::OderPricingMode::AskBidPrice
    void TestHandleMarket4();
    //coin is not enough
    void TestHandleMarket5();

    //cancel step0 buying order
    void TestHandleMarket6();
    //cancel step0 selling order
    void TestHandleMarket7();

    /* no order state, stop loss sell order filled */
    void TestHandleOrder1();
    /* no order state, stop loss buy order filled */
    void TestHandleOrder2();
    /* step0 buy order filled, step 1 sell order filled */
    void TestHandleOrder3();
    /* step0 sell order filled, step 1 buy order filled */
    void TestHandleOrder4();

    /* HrNs::OderPricingMode::AskAskPrice, buying/selling order price */
    void TestOrderPrice01();
    /* HrNs::OderPricingMode::AskBidPrice, buying/selling order price */
    void TestOrderPrice02();
    /* HrNs::OderPricingMode::AskLastPrice, buying/selling order price */
    void TestOrderPrice03();
    /* HrNs::OderPricingMode::BidAskPrice, buying/selling order price */
    void TestOrderPrice04();
    /* HrNs::OderPricingMode::BidBidPrice, buying/selling order price */
    void TestOrderPrice05();
    /* HrNs::OderPricingMode::BidLastPrice, buying/selling order price */
    void TestOrderPrice06();
    /* HrNs::OderPricingMode::LastAskPrice, buying/selling order price */
    void TestOrderPrice07();
    /* HrNs::OderPricingMode::LastBidPrice, buying/selling order price */
    void TestOrderPrice08();
    /* HrNs::OderPricingMode::LastLastPrice, buying/selling order price */
    void TestOrderPrice09();

    /* HrNs::OderPricingMode::AskAskPrice, selling/buying order price */
    void TestOrderPrice11();
    /* HrNs::OderPricingMode::AskBidPrice, selling/buying order price */
    void TestOrderPrice12();
    /* HrNs::OderPricingMode::AskLastPrice, selling/buying order price */
    void TestOrderPrice13();
    /* HrNs::OderPricingMode::BidAskPrice, selling/buying order price */
    void TestOrderPrice14();
    /* HrNs::OderPricingMode::BidBidPrice, selling/buying order price */
    void TestOrderPrice15();
    /* HrNs::OderPricingMode::BidLastPrice, selling/buying order price */
    void TestOrderPrice16();
    /* HrNs::OderPricingMode::LastAskPrice, selling/buying order price */
    void TestOrderPrice17();
    /* HrNs::OderPricingMode::LastBidPrice, selling/buying order price */
    void TestOrderPrice18();
    /* HrNs::OderPricingMode::LastLastPrice, selling/buying order price */
    void TestOrderPrice19();

    /* step0 buy order created, then stop, then start. we expect HrAccGrpAlgorithm
     *  is at Step0Buying state.
     */
    void TestRestartHrAccGrp1();
    /* step0 sell order created, then stop, then start. we expect HrAccGrpAlgorithm
    *  is at Step0Selling state.
    */
    void TestRestartHrAccGrp2();
    /* step1 sell order created, then stop, then start. we expect HrAccGrpAlgorithm
    *  is at Step1Selling state.
    */
    void TestRestartHrAccGrp3();
    /* step1 buy order created, then stop, then start. we expect HrAccGrpAlgorithm
    *  is at Step1Buying state.
    */
    void TestRestartHrAccGrp4();

private:
    void ResetSession();

    typedef std::pair<std::shared_ptr<MarketEntity>, std::shared_ptr<MarketEntity>> MarketEntityPair;
    MarketEntityPair CreateMarket(Money ptm0BidPrice, Money ptm0AskPrice, Money ptm0LastPrice,
        Money ptm1BidPrice, Money ptm1AskPrice, Money ptm1LastPrice);

    void SendMarket(Money ptm0BidPrice, Money ptm0AskPrice, Money ptm0LastPrice,
        Money ptm1BidPrice, Money ptm1AskPrice, Money ptm1LastPrice);

    void SetOrder(shared_ptr<HrOrderEntity> hrOrderEnt);
    void SetOrder(shared_ptr<HrOrderEntity> hrOrderEnt, Money concludedPrice);

    template<typename T>
    void UpdateEntity(std::shared_ptr<T> ent)
    {
        StartTransaction(db, session, MaxTransactionRetryNumber);
        db->update(ent);
        EndTransaction();
    }

    template<typename T>
    std::shared_ptr<T> LoadEntity(TableId id)
    {
        /* 为了避免错误的load到了之前的单元测试用例的数据(相同的table id, 如2次测试使用了相同id 的MarkEtentity),
         * 我们使用了临时遍历 session。
         */
        std::shared_ptr<T> ent;
        StartTransaction(db, session, MaxTransactionRetryNumber);
        ent = db->load<T>(id);
        EndTransaction();
        return ent;
    }

private:
    std::shared_ptr<odb::database> db;
    std::shared_ptr<odb::session> session;

    std::shared_ptr<HrEntity> hrEnt;
    std::shared_ptr<HrAccountGroupEntity> hrAccGrpEnt;
    std::shared_ptr<HrAccountEntity> hrAccEnt0;
    std::shared_ptr<HrAccountEntity> hrAccEnt1;
    std::shared_ptr<PlatformEntity> ptmEnt0;
    std::shared_ptr<PlatformEntity> ptmEnt1;
    std::shared_ptr<PlatformCoinRuleEntity> ptm0CoinRuleEnt;
    std::shared_ptr<PlatformCoinRuleEntity> ptm1CoinRuleEnt;
};

CxxEndNameSpace
#endif //#ifdef HasUtPlatform
#endif