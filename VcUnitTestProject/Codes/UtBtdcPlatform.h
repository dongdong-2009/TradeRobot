#ifndef _UtBtdcPlatform_h_
#define _UtBtdcPlatform_h_

#include <cppunit/extensions/HelperMacros.h>

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"
#include "Include/Entity/EntityQueryType.h"

#include "UtModule.h"
#ifdef HasUtBtdcPlatform

CxxBeginNameSpace(UnitTest)

/**********************UtBtdcPlatform**********************/
class UtBtdcPlatform : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtBtdcPlatform);
    CPPUNIT_TEST(TestCreateBuyOrder);
    CPPUNIT_TEST(TestCreateSellOrder);
    CPPUNIT_TEST(TestCancelOrder);
    CPPUNIT_TEST(TestFetchOrder);
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
    void TestFetchOrder();
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