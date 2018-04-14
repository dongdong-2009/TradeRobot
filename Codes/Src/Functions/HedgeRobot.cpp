#include "HedgeRobot.h"

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

/* Functions */
#include "Include/Functions/TableIndexHelperInterface.h"
#include "Include/Functions/PlatformInterface.h"
#include "Include/Functions/HrAccGrpInterface.h"
#include "Include/Functions/EnumHelperInterface.h"

/* local header files */
#include "HrAccGrpAlgorithm.h"

using namespace std;
using namespace placeholders;

using namespace HrNs;

typedef ClassFactoriesRegistor<RobotInterface, std::string, shared_ptr<RobotEntity>> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, "HedgeRobot", HedgeRobot::CreateInstance);

/**********************class HedgeRobot**********************/
HedgeRobot::HedgeRobot(std::shared_ptr<RobotEntity> robotEnt)
    : serviceState(ServiceState::ServiceStopped), hrEnt(dynamic_pointer_cast<HrEntity>(robotEnt))
{}

HedgeRobot::~HedgeRobot()
{}

RobotInterface* HedgeRobot::CreateInstance(shared_ptr<RobotEntity> robotEnt)
{
    return new HedgeRobot(robotEnt);
}

void HedgeRobot::SetRobotCfgState(RobotCfgState robotCfgState)
{
    std::lock_guard<std::mutex> lock(mtx);

    if (hrEnt->GetRobotCfgState() == robotCfgState)
        return;

    if (serviceState == ServiceState::ServiceRunning)
    {
        if (robotCfgState == RobotCfgState::Enabled)
            StartImpl();
        else
            StopImpl();
    }
    hrEnt->SetRobotCfgState(robotCfgState);
}

void HedgeRobot::Start()
{
    LOG4_TRACE("Start, robot id=" << GetRobotId());

    /* refresh parameter */
    shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
    shared_ptr<odb::session> session = Database::GetInstance().GetSession();
    StartTransaction(db, session, MaxTransactionRetryNumber);
    for (auto i = hrEnt->GetPlatformPairs().begin(); i != hrEnt->GetPlatformPairs().end(); ++i)
    {
        db->reload(*i);
    }
    EndTransaction();

    std::lock_guard<std::mutex> lock(mtx);

    if (this->serviceState == ServiceState::ServiceRunning)
    {
        LOG4_TRACE("this->serviceState == ServiceStatus::ServiceRunning");
        return;
    }

    if (hrEnt->GetRobotCfgState() == RobotCfgState::Enabled)
    {
        StartImpl();
    }

    this->serviceState = ServiceState::ServiceRunning;
    LOG4_TRACE("Succeeded");
}

void HedgeRobot::Stop()
{
    LOG4_TRACE("Start");

    std::lock_guard<std::mutex> lock(mtx);
    if (this->serviceState == ServiceState::ServiceStopped)
    {
        LOG4_INFO("this->serviceState == ServiceStatus::ServiceStopped");
        return;
    }

    if (hrEnt->GetRobotCfgState() == RobotCfgState::Enabled)
    {
        StopImpl();
    }

    this->serviceState = ServiceState::ServiceStopped;
    LOG4_TRACE("Succeeded");
}

void HedgeRobot::PrintRuntimeStatus(std::ostream& os) const
{
    string indent2(2, ' '), indent4(4, ' '), indent6(6, ' '), indent8(8, ' ');
    os << "robot id: " << hrEnt->GetId() << endl;
    os << indent2 << "svc status : " << GetEnumName(serviceState) << endl;
    os << indent2 << "cfg status : " << GetEnumName(hrEnt->GetRobotCfgState()) << endl;
    os << indent2 << "grp number : " << hrAccGrps.size() << endl;
    os << indent2 << "acc grps   : ... ..." << endl;
    for (auto iter = hrAccGrps.cbegin(); iter != hrAccGrps.cend(); ++iter)
    {
        auto hrAccGrpEnt = (*iter)->GetHrAccGrpEnt();
        os << indent4 << "grp id    : " << hrAccGrpEnt->GetId() << endl;
        os << indent6 << "grp state : " << ((*iter)->IsRunning() ? "running" : "stopped") << endl;

        list<shared_ptr<HrAccountEntity>> hrAccEnts{ hrAccGrpEnt->GetHrAccount0(), hrAccGrpEnt->GetHrAccount1() };
        for (auto i = hrAccEnts.begin(); i != hrAccEnts.end(); ++i)
        {
            os << indent6 << "account id: " << (*i)->GetId() << endl;
            os << indent8 << "invested money : " << (*i)->GetInvestedMoney() << endl;
            os << indent8 << "available money: " << (*i)->GetAvailableMoney() << endl;
            os << indent8 << "invested coin  : " << (*i)->GetInvestedCoinNumber() << endl;
            os << indent8 << "available coin : " << (*i)->GetAvailableCoinNumber() << endl;
        }
    }
}

/* protected functions */
shared_ptr<RobotEntity> HedgeRobot::GetRobotEntity() const
{
    return hrEnt;
}

/* private functions */
void HedgeRobot::HandleMarket(shared_ptr<MarketEntity> marketEnt)
{
    /* When stop robot, the functions calling relationship and mutex are:
     * HedgeRobot::Stop(),  HedgeRobot::mtx
     * -> HedgeRobot::StopImpl()
     * -> DeregisterMarketHandler(), PlatformBase::handlersMtx
     *
     * when Market received, the functions calling relationship and mutex are:
     * void PlatformBase::HandleMarketInfor(),  PlatformBase::handlersMtx
     * -> void HedgeRobot::HandleMarket(),  HedgeRobot::mtx
     *
     * to avoid mutex dead lock, we use try_lock()
     */
    unique_lock<mutex> lock(mtx, std::defer_lock);
    if (lock.try_lock())
    {
        for (auto i = hrAccGrps.begin(); i != hrAccGrps.end(); ++i)
        {
            (*i)->HandleMarket(marketEnt);
        }
    }
}

void HedgeRobot::StartImpl()
{
    LOG4_TRACE("Start");
    auto cmp = [](shared_ptr<PlatformEntity> l, shared_ptr<PlatformEntity> r)
    {
        return l->GetId() < r->GetId();
    };
    set<shared_ptr<PlatformEntity>, bool(*)(shared_ptr<PlatformEntity>, shared_ptr<PlatformEntity>)> ptmEnts(cmp);

    std::list<shared_ptr<HrAccountGroupEntity>>& accGrpEnts = hrEnt->GetAccountGroups();
    for (auto i = accGrpEnts.begin(); i != accGrpEnts.end(); ++i)
    {
        shared_ptr<HrAccountGroupEntity> hrAccGrpEnt = *i;
        shared_ptr<HrAccGrpInterface> hrAccGrp = make_shared(hrAccGrpEnt);
        hrAccGrps.push_back(hrAccGrp);

        if (ptmEnts.find(hrAccGrpEnt->GetHrAccount0()->GetPlatform()) == ptmEnts.end())
        {
            ptmEnts.insert(hrAccGrpEnt->GetHrAccount0()->GetPlatform());
        }

        if (ptmEnts.find(hrAccGrpEnt->GetHrAccount1()->GetPlatform()) == ptmEnts.end())
        {
            ptmEnts.insert(hrAccGrpEnt->GetHrAccount1()->GetPlatform());
        }

        LOG4_DEBUG("Start hr account group, id=" << hrAccGrpEnt->GetId());
        hrAccGrp->Start();
    }

    auto handler = bind(&HedgeRobot::HandleMarket, this, _1);
    for (auto i = ptmEnts.begin(); i != ptmEnts.end(); ++i)
    {
        PlatformInterface& ptm = GetPlatformInstance(*i);
        ptm.RegisterMarketHandler(hrEnt->GetId(), hrEnt->GetCoinType(), handler);
    }

    LOG4_TRACE("Succeeded");
}

void HedgeRobot::StopImpl()
{
    LOG4_TRACE("Start");
    auto cmp = [](shared_ptr<PlatformEntity> l, shared_ptr<PlatformEntity> r)
    {
        return l->GetId() < r->GetId();
    };
    set<shared_ptr<PlatformEntity>, bool(*)(shared_ptr<PlatformEntity>, shared_ptr<PlatformEntity>)> ptmEnts(cmp);

    std::list<shared_ptr<HrAccountGroupEntity>>& accGrpEnts = hrEnt->GetAccountGroups();
    for (auto i = accGrpEnts.begin(); i != accGrpEnts.end(); ++i)
    {
        if (ptmEnts.find((*i)->GetHrAccount0()->GetPlatform()) == ptmEnts.end())
        {
            ptmEnts.insert((*i)->GetHrAccount0()->GetPlatform());
        }

        if (ptmEnts.find((*i)->GetHrAccount1()->GetPlatform()) == ptmEnts.end())
        {
            ptmEnts.insert((*i)->GetHrAccount1()->GetPlatform());
        }
    }

    for (auto i = ptmEnts.begin(); i != ptmEnts.end(); ++i)
    {
        PlatformInterface& ptm = GetPlatformInstance(*i);
        ptm.DeregisterMarketHandler(hrEnt->GetId(), hrEnt->GetCoinType());
    }

    for (auto i = hrAccGrps.begin(); i != hrAccGrps.end(); ++i)
    {
        (*i)->Stop();
    }
    hrAccGrps.clear();

    LOG4_TRACE("Succeeded");
}

