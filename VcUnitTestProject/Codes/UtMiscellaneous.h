#ifndef _UtMiscellaneous_h_
#define _UtMiscellaneous_h_

#include <cppunit/extensions/HelperMacros.h>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

#include "UtModule.h"
#ifdef HasUtMiscellaneous

CxxBeginNameSpace(UnitTest)

/**********************UtMiscellaneous**********************/
class UtMiscellaneous : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtMiscellaneous);
    CPPUNIT_TEST(TestMiscellaneous1);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestMiscellaneous1();
};

CxxEndNameSpace
#endif //#ifdef HasUtMiscellaneous
#endif