#ifndef _UtController_h_
#define _UtController_h_

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

#include "UtModule.h"
#ifdef HasUtController

CxxBeginNameSpace(UnitTest)

/**********************UtController**********************/
class UtController : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtController);
    //CPPUNIT_TEST(TestStart1);
    //CPPUNIT_TEST(TestStart2);
    //CPPUNIT_TEST(TestStart3);
    CPPUNIT_TEST(TestPrintRuntimeStatus);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    //step0 buy order will be created. HrNs::OderPricingMode::AskBidPrice
    void TestStart1();

    //step0 sell order will be created. HrNs::OderPricingMode::AskBidPrice
    void TestStart2();

    //step0 buy order filled. HrNs::OderPricingMode::AskBidPrice
    void TestStart3();

    void TestPrintRuntimeStatus();

private:
    void ResetSession();
    void SendMarket(Money ptm0BidPrice, Money ptm0AskPrice, Money ptm0LastPrice,
        Money ptm1BidPrice, Money ptm1AskPrice, Money ptm1LastPrice);

private:
    std::shared_ptr<odb::database> db;
    std::shared_ptr<odb::session> session;
    
    std::shared_ptr<HrEntity> hrEnt;
    std::shared_ptr<HrAccountGroupEntity> hrAccGrpEnt;
};

CxxEndNameSpace
#endif //#ifdef HasUtController
#endif