#include "UtMemberHelper.h"

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

/* Entity */
#include "Include/Entity/Entities.h"         //for TableIdEntity
#include "Include/Entity/Entities-odb.h"     //for TableIdEntity
#include "Include/Entity/EntityQueryType.h"  //for TableIdEntity

/* Functions */
#include "Include/Functions/TableIndexHelperInterface.h"

/* local header files */
#include "UtEntities.h"
#include "UtEntities-odb.h"
#include "UtEntityQueryType.h"

#ifdef HasUtMemberHelper
using namespace std;

CxxBeginNameSpace(UnitTest);

/**********************UtMemberHelper**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtMemberHelper);

/* public function */
void UtMemberHelper::setUp()
{
    db = Database::GetInstance().GetDatabase();
    ResetSession();

    StartTransaction(db, session, MaxTransactionRetryNumber);
	db->execute("delete from `TableId`;");
	db->execute("delete from `Ut1v1WeakFkt`;");
	db->execute("delete from `Ut1v1SharedPkt`;");
	db->execute("delete from `Ut1v1LazySharedPkt`;");

    db->execute("delete from `Ut1v1SharedFkt`;");
    db->execute("delete from `Ut1v1WeakPkt`;");

    db->execute("delete from `Ut1vNSharedPkt`;");
    db->execute("delete from `Ut1vNWeakFkt1`;");

    db->execute("delete from `Ut1vNLazySharedPkt`;");
    db->execute("delete from `Ut1vNWeakFkt2`;");

    db->execute("delete from `Ut1vNWeakPkt1`;");
    db->execute("delete from `Ut1vNSharedFkt`;");

    db->execute("delete from `Ut1vNWeakPkt2`;");
    db->execute("delete from `Ut1vNLazySharedFkt`;");

    /* TableIdxEntity */
    const char *tableNames[] =
    {
        "Ut1v1SharedPkt", "Ut1v1LazySharedPkt", "Ut1v1WeakFkt",
        "Ut1v1SharedFkt", "Ut1v1WeakPkt",
        "Ut1vNSharedPkt", "Ut1vNWeakFkt1",
        "Ut1vNLazySharedPkt", "Ut1vNWeakFkt2",
        "Ut1vNWeakPkt1", "Ut1vNSharedFkt",
        "Ut1vNWeakPkt2", "Ut1vNLazySharedFkt",
    };
    for (size_t i = 0; i < sizeof(tableNames) / sizeof(const char*); ++i)
    {
        shared_ptr<TableIdEntity> tableIdxEntity = make_shared<TableIdEntity>(tableNames[i], 1);
        db->persist(tableIdxEntity);
    }
    EndTransaction();
}

/* protected function */
void UtMemberHelper::Test1v1SharedPktAndWeakFkt()
{
    list<const char*> functions =
    {
        "Ut1v1SharedPktEntity::GetUt1v1WeakFkt",
        "Ut1v1SharedPktEntity::Bind",
        "Ut1v1SharedPktEntity::Unbind",

        "Ut1v1WeakFktEntity::SetUt1v1SharedPkt",
        "Ut1v1WeakFktEntity::Bind",
        "Ut1v1WeakFktEntity::Unbind",
    };

    TableId ut1v1SharedPktId;
    TableId ut1v1WeakIdFktId1, ut1v1WeakIdFktId2;
    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();

        ut1v1SharedPktId = tableIndexHelper.GetUseableTableIndex("Ut1v1SharedPkt");
        auto ut1v1SharedPktEnt = make_shared<Ut1v1SharedPktEntity>(ut1v1SharedPktId, 0, 0);

        ut1v1WeakIdFktId1 = tableIndexHelper.GetUseableTableIndex("Ut1v1WeakFkt");
        auto ut1v1WeakFktEnt1 = make_shared<Ut1v1WeakFktEntity>(ut1v1WeakIdFktId1, 0, 0);

        ut1v1WeakIdFktId2 = tableIndexHelper.GetUseableTableIndex("Ut1v1WeakFkt");
        auto ut1v1WeakFktEnt2 = make_shared<Ut1v1WeakFktEntity>(ut1v1WeakIdFktId2, 0, 0);

        CPPUNIT_ASSERT(ut1v1SharedPktEnt->GetBindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1SharedPktEnt->GetUnbindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetBindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetUnbindNumber() == 0);
        ut1v1SharedPktEnt->SetUt1v1WeakFkt(ut1v1WeakFktEnt1);
        cout << "..." << endl;

        CPPUNIT_ASSERT(ut1v1SharedPktEnt->GetBindNumber() == 1);
        CPPUNIT_ASSERT(ut1v1SharedPktEnt->GetUnbindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetBindNumber() == 1);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetUnbindNumber() == 0);
        //CPPUNIT_ASSERT_THROW(ut1v1SharedPktEnt->SetUt1v1WeakFkt(ut1v1WeakFktEnt2), runtime_error);
        ut1v1SharedPktEnt->SetUt1v1WeakFkt(nullptr);
        ut1v1SharedPktEnt->SetUt1v1WeakFkt(ut1v1WeakFktEnt1);
        cout << "..." << endl;

        CPPUNIT_ASSERT(ut1v1SharedPktEnt->GetBindNumber() == 2);
        CPPUNIT_ASSERT(ut1v1SharedPktEnt->GetUnbindNumber() == 1);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetBindNumber() == 2);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetUnbindNumber() == 1);

        TableId ut1v1LazySharedPktId = tableIndexHelper.GetUseableTableIndex("Ut1v1LazySharedPkt");
        shared_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt = make_shared<Ut1v1LazySharedPktEntity>(ut1v1LazySharedPktId, 0, 0);
        ut1v1LazySharedPktEnt->SetUt1v1WeakFkt(ut1v1WeakFktEnt1);

        ut1v1SharedPktEnt->SetBindNumber(0);
        ut1v1SharedPktEnt->SetUnbindNumber(0);
        ut1v1WeakFktEnt1->SetBindNumber(0);
        ut1v1WeakFktEnt1->SetUnbindNumber(0);

        db->persist(ut1v1SharedPktEnt);
        db->persist(ut1v1LazySharedPktEnt);
        db->persist(ut1v1WeakFktEnt1);
        EndTransaction();
    }

    {
        ResetSession();
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt = db->load<Ut1v1SharedPktEntity>(ut1v1SharedPktId);
        shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt = ut1v1SharedPktEnt->GetUt1v1WeakFkt();

        CPPUNIT_ASSERT(ut1v1SharedPktEnt->GetBindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1SharedPktEnt->GetUnbindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt->GetBindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt->GetUnbindNumber() == 0);

        EndTransaction();
    }
}

//transient object for lazy_shared_ptr.
void UtMemberHelper::Test1v1LazySharedPktAndWeakFkt1()
{
    TableId ut1v1LazySharedPktId;
    TableId ut1v1WeakIdFktId;

    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt;
        shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt;

        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt == nullptr);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt == nullptr);

        TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();
        ut1v1LazySharedPktId = tableIndexHelper.GetUseableTableIndex("Ut1v1LazySharedPkt");
        ut1v1LazySharedPktEnt = make_shared<Ut1v1LazySharedPktEntity>(ut1v1LazySharedPktId, 0, 0);

        ut1v1WeakIdFktId = tableIndexHelper.GetUseableTableIndex("Ut1v1WeakFkt");
        ut1v1WeakFktEnt = make_shared<Ut1v1WeakFktEntity>(ut1v1WeakIdFktId, 0, 0);

        ut1v1LazySharedPktEnt->SetUt1v1WeakFkt(ut1v1WeakFktEnt);
        //transient object, loaded() == false, get_eager() != nullptr
        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt->GetBindNumber() == 1);
        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt->GetUnbindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt->GetBindNumber() == 1);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt->GetUnbindNumber() == 0);

        ut1v1WeakFktEnt->SetUt1v1LazySharedPkt(nullptr);
        ut1v1WeakFktEnt->SetUt1v1LazySharedPkt(ut1v1LazySharedPktEnt);

        TableId ut1v1SharedPktId = tableIndexHelper.GetUseableTableIndex("Ut1v1SharedPkt");
        shared_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt = make_shared<Ut1v1SharedPktEntity>(ut1v1SharedPktId, 0, 0);
        ut1v1SharedPktEnt->SetUt1v1WeakFkt(ut1v1WeakFktEnt);

        db->persist(ut1v1SharedPktEnt);
        db->persist(ut1v1LazySharedPktEnt);
        db->persist(ut1v1WeakFktEnt);

        EndTransaction();
    }

    {
        ResetSession();
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt = db->load<Ut1v1LazySharedPktEntity>(ut1v1LazySharedPktId);
        shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt = ut1v1LazySharedPktEnt->GetUt1v1WeakFkt(true);

        EndTransaction();
    }
}

//persistent object for lazy_shared_ptr.
void UtMemberHelper::Test1v1LazySharedPktAndWeakFkt2()
{
    TableId ut1v1LazySharedPktId1, ut1v1LazySharedPktId2;
    TableId ut1v1WeakIdFktId1, ut1v1WeakIdFktId2;
    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();

        ut1v1LazySharedPktId1 = tableIndexHelper.GetUseableTableIndex("Ut1v1LazySharedPkt");
        auto ut1v1LazySharedPktEnt1 = make_shared<Ut1v1LazySharedPktEntity>(ut1v1LazySharedPktId1, 0, 0);
        ut1v1LazySharedPktId2 = tableIndexHelper.GetUseableTableIndex("Ut1v1LazySharedPkt");
        auto ut1v1LazySharedPktEnt2 = make_shared<Ut1v1LazySharedPktEntity>(ut1v1LazySharedPktId2, 0, 0);

        ut1v1WeakIdFktId1 = tableIndexHelper.GetUseableTableIndex("Ut1v1WeakFkt");
        auto ut1v1WeakFktEnt1 = make_shared<Ut1v1WeakFktEntity>(ut1v1WeakIdFktId1, 0, 0);
        ut1v1WeakIdFktId2 = tableIndexHelper.GetUseableTableIndex("Ut1v1WeakFkt");
        auto ut1v1WeakFktEnt2 = make_shared<Ut1v1WeakFktEntity>(ut1v1WeakIdFktId2, 0, 0);

        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt1->GetBindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt1->GetUnbindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetBindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetUnbindNumber() == 0);
        //ut1v1LazySharedPktEnt1->bindNumber += 2;
        ut1v1LazySharedPktEnt1->SetUt1v1WeakFkt(ut1v1WeakFktEnt1);
        cout << "..." << endl;

        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt1->GetBindNumber() == 1);
        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt1->GetUnbindNumber() == 0);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetBindNumber() == 1);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetUnbindNumber() == 0);
        /* can't set new value when old value is not null. */
        //CPPUNIT_ASSERT_THROW(ut1v1LazySharedPktEnt1->SetUt1v1WeakFkt(ut1v1WeakFktEnt1), runtime_error);
        /* 实际应用: ut1v1LazySharedPktEnt1->SetUt1v1WeakFkt(nullptr) 之前保存 ut1v1WeakFktEnt1 */
        //ut1v1LazySharedPktEnt1->unbindNumber += 2;
        ut1v1LazySharedPktEnt1->SetUt1v1WeakFkt(nullptr);
        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt1->GetUt1v1WeakFkt(false) == nullptr);
        //ut1v1LazySharedPktEnt1->bindNumber += 2;
        ut1v1LazySharedPktEnt1->SetUt1v1WeakFkt(ut1v1WeakFktEnt1);
        cout << "..." << endl;

        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt1->GetBindNumber() == 2);
        CPPUNIT_ASSERT(ut1v1LazySharedPktEnt1->GetUnbindNumber() == 1);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetBindNumber() == 2);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1->GetUnbindNumber() == 1);

        ut1v1LazySharedPktEnt2->SetUt1v1WeakFkt(ut1v1WeakFktEnt2);

        TableId ut1v1SharedPktId1 = tableIndexHelper.GetUseableTableIndex("Ut1v1SharedPkt");
        shared_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt1 = make_shared<Ut1v1SharedPktEntity>(ut1v1SharedPktId1, 0, 0);
        ut1v1SharedPktEnt1->SetUt1v1WeakFkt(ut1v1WeakFktEnt1);

        TableId ut1v1SharedPktId2 = tableIndexHelper.GetUseableTableIndex("Ut1v1SharedPkt");
        shared_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt2 = make_shared<Ut1v1SharedPktEntity>(ut1v1SharedPktId2, 0, 0);
        ut1v1SharedPktEnt2->SetUt1v1WeakFkt(ut1v1WeakFktEnt2);

        db->persist(ut1v1SharedPktEnt1);
        db->persist(ut1v1LazySharedPktEnt1);
        db->persist(ut1v1WeakFktEnt1);

        db->persist(ut1v1SharedPktEnt2);
        db->persist(ut1v1LazySharedPktEnt2);
        db->persist(ut1v1WeakFktEnt2);
        EndTransaction();
    }

    {
        ResetSession();
        StartTransaction(db, session, MaxTransactionRetryNumber);

        weak_ptr<Ut1v1LazySharedPktEntity> w;
        bool e = w.expired();

        shared_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt1 = db->load<Ut1v1LazySharedPktEntity>(ut1v1LazySharedPktId1);
        shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt1 = ut1v1LazySharedPktEnt1->GetUt1v1WeakFkt(false);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1 == nullptr);

        cout << "..." << endl;
        ut1v1LazySharedPktEnt1->GetUt1v1WeakFkt(false);
        //CPPUNIT_ASSERT_THROW(ut1v1LazySharedPktEnt1->SetUt1v1WeakFkt(nullptr), runtime_error);

        ut1v1WeakFktEnt1 = ut1v1LazySharedPktEnt1->GetUt1v1WeakFkt(true);
        CPPUNIT_ASSERT(ut1v1WeakFktEnt1 != nullptr);

        EndTransaction();
    }
}

void UtMemberHelper::Test1v1SharedFktAndWeakPkt()
{
    TableId ut1v1SharedFktId;
    TableId ut1v1WeakIdPktId1, ut1v1WeakIdPktId2;
    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();

        ut1v1SharedFktId = tableIndexHelper.GetUseableTableIndex("Ut1v1SharedFkt");
        auto ut1v1SharedFktEnt = make_shared<Ut1v1SharedFktEntity>(ut1v1SharedFktId);

        ut1v1WeakIdPktId1 = tableIndexHelper.GetUseableTableIndex("Ut1v1WeakPkt");
        auto ut1v1WeakPktEnt1 = make_shared<Ut1v1WeakPktEntity>(ut1v1WeakIdPktId1);

        ut1v1WeakIdPktId2 = tableIndexHelper.GetUseableTableIndex("Ut1v1WeakPkt");
        auto ut1v1WeakPktEnt2 = make_shared<Ut1v1WeakPktEntity>(ut1v1WeakIdPktId2);

        ut1v1SharedFktEnt->SetUt1v1WeakPkt(ut1v1WeakPktEnt1);
        cout << "..." << endl;

        //CPPUNIT_ASSERT_THROW(ut1v1SharedFktEnt->SetUt1v1WeakPkt(ut1v1WeakPktEnt2), runtime_error);
        ut1v1SharedFktEnt->SetUt1v1WeakPkt(nullptr);
        ut1v1SharedFktEnt->SetUt1v1WeakPkt(ut1v1WeakPktEnt1);

        ut1v1WeakPktEnt1->SetUt1v1SharedFkt(nullptr);
        ut1v1WeakPktEnt1->SetUt1v1SharedFkt(ut1v1SharedFktEnt);
        cout << "..." << endl;

        db->persist(ut1v1SharedFktEnt);
        db->persist(ut1v1WeakPktEnt1);
        EndTransaction();
    }

    {
        ResetSession();
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1v1SharedFktEntity> ut1v1SharedFktEnt = db->load<Ut1v1SharedFktEntity>(ut1v1SharedFktId);
        shared_ptr<Ut1v1WeakPktEntity> ut1v1WeakFktEnt = ut1v1SharedFktEnt->GetUt1v1WeakPkt();

        EndTransaction();
    }
}

void UtMemberHelper::Test1vNSharedPktAndWeakFkt()
{

    TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();
    TableId sharedPktId = tableIndexHelper.GetUseableTableIndex("Ut1vNSharedPkt");
    TableId weakFktId1 = tableIndexHelper.GetUseableTableIndex("Ut1vNWeakFkt1");
    TableId weakFktId2 = tableIndexHelper.GetUseableTableIndex("Ut1vNWeakFkt1");
    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1vNSharedPktEntity> sharedPkt = make_shared<Ut1vNSharedPktEntity>(sharedPktId);
        shared_ptr<Ut1vNWeakFkt1Entity> weakFkt1 = make_shared<Ut1vNWeakFkt1Entity>(weakFktId1);
        shared_ptr<Ut1vNWeakFkt1Entity> weakFkt2 = make_shared<Ut1vNWeakFkt1Entity>(weakFktId2);

        sharedPkt->Bind(weakFkt1);
        //CPPUNIT_ASSERT_THROW(sharedPkt->Bind(weakFkt1), runtime_error);
        weakFkt2->SetUt1vNSharedPkt(sharedPkt);

        auto iter = sharedPkt->GetUt1vNWeakFkt1s().begin();
        CPPUNIT_ASSERT(sharedPkt->GetUt1vNWeakFkt1s().size() == 2);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId1);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId2);
        CPPUNIT_ASSERT(weakFkt1->GetUt1vNSharedPkt()->GetId() == sharedPktId);
        CPPUNIT_ASSERT(weakFkt2->GetUt1vNSharedPkt()->GetId() == sharedPktId);

        sharedPkt->Unbind(weakFkt1);
        iter = sharedPkt->GetUt1vNWeakFkt1s().begin();
        CPPUNIT_ASSERT(sharedPkt->GetUt1vNWeakFkt1s().size() == 1);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId2);
        CPPUNIT_ASSERT(weakFkt2->GetUt1vNSharedPkt()->GetId() == sharedPktId);
        CPPUNIT_ASSERT(weakFkt1->GetUt1vNSharedPkt() == nullptr);
        sharedPkt->Bind(weakFkt1);

        db->persist(sharedPkt);
        db->persist(weakFkt1);
        db->persist(weakFkt2);
        EndTransaction();
    }

    {
        ResetSession();
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1vNSharedPktEntity> sharedPkt = db->load<Ut1vNSharedPktEntity>(sharedPktId);
        auto iter = sharedPkt->GetUt1vNWeakFkt1s().begin();
        CPPUNIT_ASSERT(sharedPkt->GetUt1vNWeakFkt1s().size() == 2);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId1);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId2);

        EndTransaction();
    }

    {
        ResetSession();
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1vNWeakFkt1Entity> weakFkt1 = db->load<Ut1vNWeakFkt1Entity>(weakFktId1);
        shared_ptr<Ut1vNSharedPktEntity> sharedPkt = weakFkt1->GetUt1vNSharedPkt();
        auto iter = sharedPkt->GetUt1vNWeakFkt1s().begin();
        CPPUNIT_ASSERT(sharedPkt->GetUt1vNWeakFkt1s().size() == 2);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId1);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId2);

        EndTransaction();
    }
}

void UtMemberHelper::Test1vNLazySharedPktAndWeakFkt()
{
    TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();
    TableId lazySharedPktId = tableIndexHelper.GetUseableTableIndex("Ut1vNLazySharedPkt");
    TableId weakFktId1 = tableIndexHelper.GetUseableTableIndex("Ut1vNWeakFkt2");
    TableId weakFktId2 = tableIndexHelper.GetUseableTableIndex("Ut1vNWeakFkt2");
    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1vNLazySharedPktEntity> lazySharedPkt = make_shared<Ut1vNLazySharedPktEntity>(lazySharedPktId);
        shared_ptr<Ut1vNWeakFkt2Entity> weakFkt1 = make_shared<Ut1vNWeakFkt2Entity>(weakFktId1);
        shared_ptr<Ut1vNWeakFkt2Entity> weakFkt2 = make_shared<Ut1vNWeakFkt2Entity>(weakFktId2);

        lazySharedPkt->Bind(weakFkt1);
        //CPPUNIT_ASSERT_THROW(lazySharedPkt->Bind(weakFkt1), runtime_error);
        //CPPUNIT_ASSERT_THROW(weakFkt1->SetUt1vNLazySharedPkt(lazySharedPkt), runtime_error);
        weakFkt2->SetUt1vNLazySharedPkt(lazySharedPkt);

        list<lazy_shared_ptr<Ut1vNWeakFkt2Entity>>::iterator iter = lazySharedPkt->GetUt1vNWeakFkt2s().begin();
        CPPUNIT_ASSERT(lazySharedPkt->GetUt1vNWeakFkt2s().size() == 2);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId1);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId2);
        CPPUNIT_ASSERT(weakFkt1->GetUt1vNLazySharedPkt()->GetId() == lazySharedPktId);
        CPPUNIT_ASSERT(weakFkt2->GetUt1vNLazySharedPkt()->GetId() == lazySharedPktId);

        lazySharedPkt->Unbind(weakFkt1);
        iter = lazySharedPkt->GetUt1vNWeakFkt2s().begin();
        CPPUNIT_ASSERT(lazySharedPkt->GetUt1vNWeakFkt2s().size() == 1);
        CPPUNIT_ASSERT((*iter++)->GetId() == weakFktId2);
        CPPUNIT_ASSERT(weakFkt2->GetUt1vNLazySharedPkt()->GetId() == lazySharedPktId);
        CPPUNIT_ASSERT(weakFkt1->GetUt1vNLazySharedPkt() == nullptr);
        lazySharedPkt->Bind(weakFkt1);

        db->persist(lazySharedPkt);
        db->persist(weakFkt1);
        db->persist(weakFkt2);
        EndTransaction();
    }

    {
        ResetSession();
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1vNLazySharedPktEntity> lazySharedPkt = db->load<Ut1vNLazySharedPktEntity>(lazySharedPktId);
        list<lazy_shared_ptr<Ut1vNWeakFkt2Entity>>::iterator iter = lazySharedPkt->GetUt1vNWeakFkt2s().begin();
        CPPUNIT_ASSERT(lazySharedPkt->GetUt1vNWeakFkt2s().size() == 2);
        CPPUNIT_ASSERT((*iter++).object_id() == weakFktId1);
        CPPUNIT_ASSERT((*iter++).object_id() == weakFktId2);

        EndTransaction();
    }

    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        cout << "..." << endl;
        shared_ptr<Ut1vNWeakFkt2Entity> weakFkt1 = db->load<Ut1vNWeakFkt2Entity>(weakFktId1);
        shared_ptr<Ut1vNLazySharedPktEntity> lazySharedPkt = weakFkt1->GetUt1vNLazySharedPkt();
        list<lazy_shared_ptr<Ut1vNWeakFkt2Entity>>::iterator iter = lazySharedPkt->GetUt1vNWeakFkt2s().begin();
        CPPUNIT_ASSERT(lazySharedPkt->GetUt1vNWeakFkt2s().size() == 2);
        CPPUNIT_ASSERT((*iter++).object_id() == weakFktId1);
        CPPUNIT_ASSERT((*iter++).object_id() == weakFktId2);

        EndTransaction();
    }
}

void UtMemberHelper::Test1vNSharedFktAndWeakPkt()
{
    TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();
    TableId weakPkt1Id = tableIndexHelper.GetUseableTableIndex("Ut1vNWeakPkt1");
    TableId sharedFktId1 = tableIndexHelper.GetUseableTableIndex("Ut1vNSharedFkt");
    TableId sharedFktId2 = tableIndexHelper.GetUseableTableIndex("Ut1vNSharedFkt");
    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1vNWeakPkt1Entity> weakPkt1 = make_shared<Ut1vNWeakPkt1Entity>(weakPkt1Id);
        shared_ptr<Ut1vNSharedFktEntity> sharedFkt1 = make_shared<Ut1vNSharedFktEntity>(sharedFktId1);
        shared_ptr<Ut1vNSharedFktEntity> sharedFkt2 = make_shared<Ut1vNSharedFktEntity>(sharedFktId2);

        cout << "..." << endl;
        weakPkt1->Bind(sharedFkt1);
        //CPPUNIT_ASSERT_THROW(weakPkt1->Bind(sharedFkt1), runtime_error);
        //CPPUNIT_ASSERT_THROW(sharedFkt1->SetUt1vNWeakPkt1(weakPkt1), runtime_error);
        sharedFkt2->SetUt1vNWeakPkt1(weakPkt1);

        list<weak_ptr<Ut1vNSharedFktEntity>>::iterator iter = weakPkt1->GetUt1vNSharedFkts().begin();
        CPPUNIT_ASSERT(weakPkt1->GetUt1vNSharedFkts().size() == 2);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == sharedFktId1);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == sharedFktId2);
        CPPUNIT_ASSERT(sharedFkt1->GetUt1vNWeakPkt1()->GetId() == weakPkt1Id);
        CPPUNIT_ASSERT(sharedFkt2->GetUt1vNWeakPkt1()->GetId() == weakPkt1Id);

        cout << "..." << endl;
        weakPkt1->Unbind(sharedFkt1);
        iter = weakPkt1->GetUt1vNSharedFkts().begin();
        CPPUNIT_ASSERT(weakPkt1->GetUt1vNSharedFkts().size() == 1);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == sharedFktId2);
        CPPUNIT_ASSERT(sharedFkt2->GetUt1vNWeakPkt1()->GetId() == weakPkt1Id);
        CPPUNIT_ASSERT(sharedFkt1->GetUt1vNWeakPkt1() == nullptr);
        weakPkt1->Bind(sharedFkt1);

        db->persist(weakPkt1);
        db->persist(sharedFkt1);
        db->persist(sharedFkt2);
        EndTransaction();
    }

    {
        ResetSession();
        StartTransaction(db, session, MaxTransactionRetryNumber);

        cout << "..." << endl;
        shared_ptr<Ut1vNWeakPkt1Entity> weakPkt1 = db->load<Ut1vNWeakPkt1Entity>(weakPkt1Id);
        list<weak_ptr<Ut1vNSharedFktEntity>>::iterator iter = weakPkt1->GetUt1vNSharedFkts().begin();
        CPPUNIT_ASSERT(weakPkt1->GetUt1vNSharedFkts().size() == 2);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == sharedFktId1);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == sharedFktId2);

        EndTransaction();
    }

    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        cout << "..." << endl;
        shared_ptr<Ut1vNSharedFktEntity> sharedFkt1 = db->load<Ut1vNSharedFktEntity>(sharedFktId1);
        shared_ptr<Ut1vNWeakPkt1Entity> weakPkt1 = sharedFkt1->GetUt1vNWeakPkt1();
        list<weak_ptr<Ut1vNSharedFktEntity>>::iterator iter = weakPkt1->GetUt1vNSharedFkts().begin();
        CPPUNIT_ASSERT(weakPkt1->GetUt1vNSharedFkts().size() == 2);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == sharedFktId1);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == sharedFktId2);

        EndTransaction();
    }
}

void UtMemberHelper::Test1vNLazySharedFktAndWeakPkt()
{
    TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();
    TableId weakPkt2Id = tableIndexHelper.GetUseableTableIndex("Ut1vNWeakPkt2");
    TableId lazySharedFktId1 = tableIndexHelper.GetUseableTableIndex("Ut1vNLazySharedFkt");
    TableId lazySharedFktId2 = tableIndexHelper.GetUseableTableIndex("Ut1vNLazySharedFkt");
    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1vNWeakPkt2Entity> weakPkt2 = make_shared<Ut1vNWeakPkt2Entity>(weakPkt2Id);
        shared_ptr<Ut1vNLazySharedFktEntity> lazySharedFkt1 = make_shared<Ut1vNLazySharedFktEntity>(lazySharedFktId1);
        shared_ptr<Ut1vNLazySharedFktEntity> lazySharedFkt2 = make_shared<Ut1vNLazySharedFktEntity>(lazySharedFktId2);

        cout << "..." << endl;
        weakPkt2->BindUt1vNLazySharedFkt(lazySharedFkt1);
        //CPPUNIT_ASSERT_THROW(weakPkt2->BindUt1vNLazySharedFkt(lazySharedFkt1), runtime_error);
        //CPPUNIT_ASSERT_THROW(lazySharedFkt1->SetUt1vNWeakPkt2(weakPkt2), runtime_error);
        lazySharedFkt2->SetUt1vNWeakPkt2(weakPkt2);

        list<weak_ptr<Ut1vNLazySharedFktEntity>>::iterator iter = weakPkt2->GetUt1vNLazySharedFkts().begin();
        CPPUNIT_ASSERT(weakPkt2->GetUt1vNLazySharedFkts().size() == 2);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == lazySharedFktId1);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == lazySharedFktId2);
        CPPUNIT_ASSERT(lazySharedFkt1->GetUt1vNWeakPkt2(false)->GetId() == weakPkt2Id);
        CPPUNIT_ASSERT(lazySharedFkt2->GetUt1vNWeakPkt2(false)->GetId() == weakPkt2Id);

        weakPkt2->UnbindUt1vNLazySharedFkt(lazySharedFkt1);
        iter = weakPkt2->GetUt1vNLazySharedFkts().begin();
        CPPUNIT_ASSERT(weakPkt2->GetUt1vNLazySharedFkts().size() == 1);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == lazySharedFktId2);
        CPPUNIT_ASSERT(lazySharedFkt2->GetUt1vNWeakPkt2(false)->GetId() == weakPkt2Id);
        CPPUNIT_ASSERT(lazySharedFkt1->GetUt1vNWeakPkt2(false) == nullptr);
        weakPkt2->BindUt1vNLazySharedFkt(lazySharedFkt1);

        db->persist(weakPkt2);
        db->persist(lazySharedFkt1);
        db->persist(lazySharedFkt2);
        EndTransaction();
    }

    {
        ResetSession();
        StartTransaction(db, session, MaxTransactionRetryNumber);

        shared_ptr<Ut1vNWeakPkt2Entity> weakPkt2 = db->load<Ut1vNWeakPkt2Entity>(weakPkt2Id);
        list<weak_ptr<Ut1vNLazySharedFktEntity>>::iterator iter = weakPkt2->GetUt1vNLazySharedFkts().begin();
        CPPUNIT_ASSERT(weakPkt2->GetUt1vNLazySharedFkts().size() == 2);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == lazySharedFktId1);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == lazySharedFktId2);

        EndTransaction();
    }

    {
        StartTransaction(db, session, MaxTransactionRetryNumber);

        cout << "..." << endl;
        shared_ptr<Ut1vNLazySharedFktEntity> sharedFkt1 = db->load<Ut1vNLazySharedFktEntity>(lazySharedFktId1);
        shared_ptr<Ut1vNWeakPkt2Entity> weakPkt2 = sharedFkt1->GetUt1vNWeakPkt2(true);
        list<weak_ptr<Ut1vNLazySharedFktEntity>>::iterator iter = weakPkt2->GetUt1vNLazySharedFkts().begin();
        CPPUNIT_ASSERT(weakPkt2->GetUt1vNLazySharedFkts().size() == 2);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == lazySharedFktId1);
        CPPUNIT_ASSERT((*iter++).lock()->GetId() == lazySharedFktId2);

        EndTransaction();
    }
}

void UtMemberHelper::ResetSession()
{
    Database::GetInstance().ResetSession();
    session = Database::GetInstance().GetSession();
}

CxxEndNameSpace;
#endif //#ifdef HasUtEntity
