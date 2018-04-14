#include "UtEntity.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"
#include "Include/Entity/EntityQueryType.h"

/* Functions */
#include "Include/Functions/TableIndexHelperInterface.h"
#include "Include/Functions/XmlWrapperInterface.h"
#include "Include/Functions/LogHelperInterface.h"

/* local header files */

#ifdef HasUtEntity
using namespace std;

CxxBeginNameSpace(UnitTest);

/**********************UtEntity**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtEntity);

/* public function */
void UtEntity::setUp()
{
    Database& instance = Database::GetInstance();
    instance.ResetSession();
    db = instance.GetDatabase();
    session = instance.GetSession();

    XmlWrapperInterface& xmlWrapper = XmlWrapperInterface::GetInstance();
    xmlWrapper.Read("UtEntity.xml");
}

/* protected function */
void UtEntity::TestHrAccountGroup()
{
    StartTransaction(db, session, MaxTransactionRetryNumber);
    HrAccGrpResult hrAccGrpResult(db->query<HrAccountGroupEntity>());
    for (auto iter = hrAccGrpResult.begin(); iter != hrAccGrpResult.end(); ++iter)
    {
        auto hrAccEnt0 = iter->GetHrAccount0();
        auto hrAccEnt1 = iter->GetHrAccount1();

        milliseconds timeA = hrAccEnt0->GetPlatformApiAverageResponseTime();
        milliseconds timeB = hrAccEnt1->GetPlatformApiAverageResponseTime();
        CPPUNIT_ASSERT(timeA <= timeB);

        CPPUNIT_ASSERT(hrAccEnt0->GetInvestedMoney() == Money(100.0));
        CPPUNIT_ASSERT(hrAccEnt0->GetInvestedCoinNumber() == CoinNumber(1.0));

        CPPUNIT_ASSERT(hrAccEnt1->GetInvestedMoney() == Money(100.0));
        CPPUNIT_ASSERT(hrAccEnt1->GetInvestedCoinNumber() == CoinNumber(1.0));
    }
	EndTransaction();
}

void UtEntity::TestHrOrder()
{
    TableId id;
    PlatformOrderId platformOrderId = 1001;
    Money price(100.0);
    CoinNumber coinNumber(0.1);

    shared_ptr<HrAccountEntity> hrAccEnt;
    shared_ptr<HrOrderEntity> hrOrderEnt;
    StartTransaction(db, session, MaxTransactionRetryNumber);
    HrAccResult hrAccResult(db->query<HrAccountEntity>());
    assert(!hrAccResult.empty());
    hrAccEnt = hrAccResult.begin().load();

    id = TableIndexHelperInterface::GetInstance().GetUseableTableIndex("Order");
    hrOrderEnt = make_shared<HrOrderEntity>(id, OrderType::Buy, price, coinNumber);
    hrOrderEnt->SetAccount(hrAccEnt);
    hrOrderEnt->SetPlatformOrderId(platformOrderId);
    db->persist(hrOrderEnt);

    db->reload(hrOrderEnt);
    CPPUNIT_ASSERT(hrOrderEnt->GetId() == id);
    CPPUNIT_ASSERT(hrOrderEnt->GetPlatformOrderId() == platformOrderId);
    CPPUNIT_ASSERT(hrOrderEnt->GetLimitedPrice() == price);
    CPPUNIT_ASSERT(hrOrderEnt->GetCoinNumber() == coinNumber);
    EndTransaction();
}

CxxEndNameSpace;
#endif //#ifdef HasUtEntity
