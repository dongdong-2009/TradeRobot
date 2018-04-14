#ifndef _UtOkCoinPlatform_h_
#define _UtOkCoinPlatform_h_

#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Type.h"

#include <cppunit/extensions/HelperMacros.h>

#include "UnitTestModule.h"
#ifdef HasUtOkCoinPlatform

class RobotInterface;
class PlatformInterface;
class RobotEntity;
class PlatformEntity;
class AccountEntity;

CxxBeginNameSpace(UnitTest)

/**********************UtOkCoinPlatform**********************/
class UtOkCoinPlatform : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtOkCoinPlatform);
    CPPUNIT_TEST(TestMarketPoller);
    CPPUNIT_TEST(TestCreateOrder);
    CPPUNIT_TEST(TestFetchOrder);
    CPPUNIT_TEST_SUITE_END();

public:
    UtOkCoinPlatform();
    ~UtOkCoinPlatform();
    void setUp();

protected:
    void TestMarketPoller();
    void TestCreateOrder();
    void TestFetchOrder();

private:
    std::shared_ptr<PlatformEntity> platformEntity;
    std::shared_ptr<AccountGroupEntity> accountGroupEntity;
    std::shared_ptr<AccountEntity> accountEntity;
    std::shared_ptr<RobotEntity> robotEntity;
};

CxxEndNameSpace
#endif //#ifdef HasUtOkCoinPlatform
#endif