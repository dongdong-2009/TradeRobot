#include "HrAccGrp.h"

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
#include "Include/Functions/PlatformInterface.h"

/* local header files */
#include "HrAccGrpAlgorithm.h"

using namespace std;
using namespace placeholders;

using namespace HrNs;

HrAccGrpInterface* HrAccGrpInterface::CreateInstance(shared_ptr<HrAccountGroupEntity> hrAccEnt)
{
    return new HrAccGrp(hrAccEnt);
}

/**********************class HrAccGrp**********************/
HrAccGrp::HrAccGrp(shared_ptr<HrAccountGroupEntity> hrAccGrpEnt)
{
    running = false;
    thdEvent = 0;
    this->hrAccGrpEnt = hrAccGrpEnt;
    hrAccEnt0 = hrAccGrpEnt->GetHrAccount0();
    hrAccEnt1 = hrAccGrpEnt->GetHrAccount1();
    ptmEnt0 = hrAccEnt0->GetPlatform();
    ptmEnt1 = hrAccEnt1->GetPlatform();
}

HrAccGrp::~HrAccGrp()
{}

void HrAccGrp::Start()
{
    lock_guard<mutex> lock(mtx);
    if (running)
        return;

    /* 如果有, 分别加载hrAccEnt0 和hrAccEnt1
     * 最后1条Market纪录
     */
	shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
	shared_ptr<odb::session> session = Database::GetInstance().GetSession();
	StartTransaction(db, session, MaxTransactionRetryNumber);
    marketEntPair.first = ptmEnt0->GetLastMarket();
    marketEntPair.second = ptmEnt1->GetLastMarket();

    assert(marketEntPair.first == nullptr  || marketEntPair.first->GetPlatform() == ptmEnt0);
    assert(marketEntPair.second == nullptr || marketEntPair.second->GetPlatform() == ptmEnt1);

    list<shared_ptr<HrAccountEntity>> hrAccEnts{ hrAccEnt0, hrAccEnt1 };
    for (auto i = hrAccEnts.begin(); i != hrAccEnts.end(); ++i)
    {
        if ((*i)->GetInvestedMoney() == Money::Zero())
        {
            Money cnyBalance;
            GetPlatformInstance((*i)->GetPlatform()).FetchBalance((*i), cnyBalance);
            (*i)->SetInvestedMoney(cnyBalance);
            db->update((*i));
        }

        if ((*i)->GetInvestedCoinNumber() == CoinNumber::Zero())
        {
            CoinNumber coinBalance;
            GetPlatformInstance((*i)->GetPlatform()).FetchBalance((*i), hrAccGrpEnt->GetRobot()->GetCoinType(), coinBalance);
            (*i)->SetInvestedCoinNumber(coinBalance);
            db->update((*i));
        }
    }
    EndTransaction();

    running = true;
    bool ready = false;
    theAccGrpThreadMain = std::thread(bind(&HrAccGrp::TheAccGrpThreadMain, this, &ready));
    while (!ready)
    {
        std::this_thread::yield();
    }
}

void HrAccGrp::Stop()
{
    LOG4_TRACE("Start");

    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!running)
        {
            if (theAccGrpThreadMain.joinable())
                theAccGrpThreadMain.join();
            return;
        }
        thdEvent |= ThdEventExit;
        cv.notify_one();
    }
    theAccGrpThreadMain.join();

    LOG4_TRACE("Succeeded");
}

bool HrAccGrp::IsRunning() const
{
    lock_guard<mutex> lock(mtx);
    return running;
}

shared_ptr<HrAccountGroupEntity> HrAccGrp::GetHrAccGrpEnt() const
{
    return hrAccGrpEnt;
}

void HrAccGrp::HandleMarket(shared_ptr<MarketEntity> marketEnt)
{
    assert(marketEnt->GetCoinType() == hrAccGrpEnt->GetRobot()->GetCoinType());
    TableId ptmId = marketEnt->GetPlatform()->GetId();

    assert(marketEnt->GetPlatform() != nullptr);
    lock_guard<mutex> lock(mtx);
    if (ptmId == ptmEnt0->GetId())
    {
        marketEntPair.first = marketEnt;
    }
    else if (ptmId == ptmEnt1->GetId())
    {
        marketEntPair.second = marketEnt;
    }
    else
    {
        /* do nothing */
        return;
    }

    if (marketEntPair.first != nullptr && marketEntPair.second != nullptr)
    {
        assert(marketEntPair.first->GetPlatform() != nullptr);
        assert(marketEntPair.second->GetPlatform() != nullptr);
        thdEvent |= ThdEventMarket;
        cv.notify_one();
    }
    else
    {
        LOG4_TRACE("marketEntPair.first or marketEntPair.second is null");
    }
}

/* private functions */
void HrAccGrp::TheAccGrpThreadMain(bool *ready)
{
    LOG4_TRACE("Start");
    shared_ptr<HrAccGrpAlgorithmInterface> algorithm = make_shared<HrAccGrpAlgorithm>(hrAccGrpEnt);
    time_point until = system_clock::now() + milliseconds(500);
    shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
	shared_ptr<odb::session> session = Database::GetInstance().GetSession();

    *ready = true;
    while (running)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_until(lock, until, [this]{ return thdEvent != 0; });

        if ((thdEvent & ThdEventMarket) == ThdEventMarket)
        {
            /* 先于ThdEventExit事件的检测, 首先检测并处理ThdEventMarket, 其目的是确保线程在退出前处理掉所有的
             * Market Event, 方便单元测试.  因为单元测试的代码通常如下:
             hrAccGrp->Start();
             hrAccGrp->HandleMarket(ptm0MarketEnt);
             hrAccGrp->HandleMarket(ptm1MarketEnt);
             hrAccGrp->Stop();   //Stop() 正常返回, 说明Market信息一定已经被处理过了.
             CPPUNIT_ASSERT(xxxx == yyyy);
             */
            thdEvent &= ~ThdEventMarket;

            /* handle market */
            try
            {
                algorithm->HandleMarket(*marketEntPair.first, *marketEntPair.second);
            }
            catch (std::runtime_error& err)
            {
                running = false;
                LOG4_FATAL("robot is in abnormal state, robot id=" << hrAccGrpEnt->GetRobot()->GetId()
                    << ", group id=" << hrAccGrpEnt->GetId()
                    << "err message=" << err.what());
            }
            continue;
        }

        if ((thdEvent & ThdEventExit) == ThdEventExit)
        {
            thdEvent &= ~ThdEventExit;

            running = false;
            continue;
        }

        /* time out */
        until = until + seconds(1);
        if (marketEntPair.first != nullptr && marketEntPair.second != nullptr)
        {
            try
            {
                algorithm->HandleOrder(hrAccEnt0, *marketEntPair.first, *marketEntPair.second);
                algorithm->HandleOrder(hrAccEnt1, *marketEntPair.first, *marketEntPair.second);
            }
            catch (std::runtime_error& err)
            {
                running = false;
                LOG4_FATAL("robot is in abnormal state, robot id=" << hrAccGrpEnt->GetRobot()->GetId()
                    << ", group id=" << hrAccGrpEnt->GetId()
                    << "err message=" << err.what());
            }
        }
    }

    LOG4_TRACE("Succeeded");
}
