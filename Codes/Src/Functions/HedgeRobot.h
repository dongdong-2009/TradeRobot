#ifndef _HedgeRobot_h_
#define _HedgeRobot_h_

#include "Include/Functions/RobotInterface.h"

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

class RobotEntity;
class MarketEntity;
class HrEntity;

class HrAccGrpInterface;

/**********************class HedgeRobot**********************/
class HedgeRobot : public RobotInterface
{
public:
    HedgeRobot(std::shared_ptr<RobotEntity> robotEnt);
    virtual ~HedgeRobot();
    static RobotInterface* CreateInstance(std::shared_ptr<RobotEntity> robotEnt);

    void SetRobotCfgState(RobotCfgState robotCfgState);

    void Start();
    void Stop();
    void PrintRuntimeStatus(std::ostream& os) const;

protected:
    std::shared_ptr<RobotEntity> GetRobotEntity() const;

private:
    void HandleMarket(std::shared_ptr<MarketEntity> marketEnt);
    void StartImpl();
    void StopImpl();

private:
    std::mutex mtx;

    ServiceState serviceState;
    std::shared_ptr<HrEntity> hrEnt;
    std::list<std::shared_ptr<HrAccGrpInterface>> hrAccGrps;
};

#endif