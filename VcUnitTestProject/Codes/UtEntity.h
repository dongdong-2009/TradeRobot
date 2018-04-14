#ifndef _UtEntity_h_
#define _UtEntity_h_

#include <cppunit/extensions/HelperMacros.h>

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

#include "UtModule.h"
#ifdef HasUtEntity

CxxBeginNameSpace(UnitTest)

/**********************UtEntity**********************/
class UtEntity : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtEntity);
    CPPUNIT_TEST(TestHrAccountGroup);
    CPPUNIT_TEST(TestHrOrder);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestHrAccountGroup();
    void TestHrOrder();

private:
    std::shared_ptr<odb::database> db;
    std::shared_ptr<odb::session> session;
};

CxxEndNameSpace
#endif //#ifdef HasUtPlatform
#endif