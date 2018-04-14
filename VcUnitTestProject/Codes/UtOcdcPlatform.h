#ifndef _UtOkCoinDotCnPlatform_h_
#define _UtOkCoinDotCnPlatform_h_

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"
#include "Include/Entity/EntityQueryType.h"

#include "UtModule.h"
#ifdef HasUtOcdcPlatform

CxxBeginNameSpace(UnitTest)

/**********************UtOcdcPlatform**********************/
class UtOcdcPlatform : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtOcdcPlatform);
    CPPUNIT_TEST(TestCreateBuyOrder);
    CPPUNIT_TEST(TestCreateSellOrder);
    CPPUNIT_TEST(TestCancelOrder);
    CPPUNIT_TEST(TestFetchOrder1);
    CPPUNIT_TEST(TestFetchOrder2);
    CPPUNIT_TEST(TestFetchUnknownOrder);
    CPPUNIT_TEST(TestFetchBalance);
    CPPUNIT_TEST(TestMarketPoller);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestCreateBuyOrder();
    void TestCreateSellOrder();
    void TestCancelOrder();
    /* fetch 正常的 order, 刚刚canceled order */
    void TestFetchOrder1();
    /* 不存在的order （okcoin 会在2天后删除order history）*/
    void TestFetchOrder2();
    void TestFetchUnknownOrder();
    void TestFetchBalance();
    void TestMarketPoller();

private:
    void ResetSession();

private:
    std::shared_ptr<odb::database> db;
    std::shared_ptr<odb::session> session;

    std::shared_ptr<HrEntity> hrEnt;
    std::shared_ptr<HrAccountGroupEntity> hrAccGrpEnt;
    std::shared_ptr<HrAccountEntity> hrAccEnt; /* btctrade.com account */
    std::shared_ptr<PlatformEntity>  ptmEnt;   /* btctrade.com */

    Money lowestPrice, highestPrice;
    CoinNumber minTradingUnit;
};

CxxEndNameSpace
#endif //#ifdef HasUtPlatform
#endif