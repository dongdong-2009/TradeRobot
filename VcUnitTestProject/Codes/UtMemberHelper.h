#ifndef _UtMemberHelper_h_
#define _UtMemberHelper_h_

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
#ifdef HasUtMemberHelper

CxxBeginNameSpace(UnitTest)

/**********************UtEntity**********************/
class UtMemberHelper : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtMemberHelper);
	CPPUNIT_TEST(Test1v1SharedPktAndWeakFkt);
	CPPUNIT_TEST(Test1v1LazySharedPktAndWeakFkt1);
	CPPUNIT_TEST(Test1v1LazySharedPktAndWeakFkt2);
	CPPUNIT_TEST(Test1v1SharedFktAndWeakPkt);

	CPPUNIT_TEST(Test1vNSharedPktAndWeakFkt);
	CPPUNIT_TEST(Test1vNLazySharedPktAndWeakFkt);

	CPPUNIT_TEST(Test1vNSharedFktAndWeakPkt);
	CPPUNIT_TEST(Test1vNLazySharedFktAndWeakPkt);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
	void Test1v1SharedPktAndWeakFkt();
	void Test1v1LazySharedPktAndWeakFkt1();
	void Test1v1LazySharedPktAndWeakFkt2();
    void Test1v1SharedFktAndWeakPkt();

    void Test1vNSharedPktAndWeakFkt();
    void Test1vNLazySharedPktAndWeakFkt();

    void Test1vNSharedFktAndWeakPkt();
    void Test1vNLazySharedFktAndWeakPkt();

private:
    void ResetSession();

private:
    std::shared_ptr<odb::database> db;
    std::shared_ptr<odb::session> session;
};

CxxEndNameSpace
#endif //#ifdef HasUtPlatform
#endif