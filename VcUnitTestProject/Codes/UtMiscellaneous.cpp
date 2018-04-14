#include "UtMiscellaneous.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Debug.h"
#include "Include/Foundation/ResultCode.h"

/* local header files */

#ifdef HasUtMiscellaneous
using namespace std;

CxxBeginNameSpace(UnitTest);

/**********************UtMiscellaneous**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtMiscellaneous);

/* public function */
void UtMiscellaneous::setUp()
{}

/* protected function */
void UtMiscellaneous::TestMiscellaneous1()
{}

CxxEndNameSpace;
#endif //#ifdef HasUtEntity
