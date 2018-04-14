#ifndef _RobotInterface_h_
#define _RobotInterface_h_

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/ClassFactories.h"

/* Entity */
#include "Include/Entity/Entities.h"

/**********************class RobotInterface**********************/
class RobotInterface
{
public:
    RobotInterface() {};
    virtual ~RobotInterface() {};

    TableId GetRobotId() const
    {
        return GetRobotEntity()->GetId();
    }

    const char* GetName() const
    {
        return GetRobotEntity()->GetName();
    }

    RobotCfgState GetRobotCfgState() const
    {
        return GetRobotEntity()->GetRobotCfgState();
    }
    virtual void SetRobotCfgState(RobotCfgState robotCfgState) = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void PrintRuntimeStatus(std::ostream& os) const = 0;

protected:
    virtual std::shared_ptr<RobotEntity> GetRobotEntity() const = 0;
};

/**********************class RobotInterface Factory**********************/
inline RobotInterface* CreateRobotInstance(std::shared_ptr<RobotEntity> robotEnt)
{
    typedef ClassFactories<RobotInterface, std::string, std::shared_ptr<RobotEntity>> ClassFactoriesType;
    ClassFactoriesType& instance = ClassFactoriesType::GetInstance();

    return instance.CreateInstance(robotEnt->GetName(), robotEnt);
}

namespace std
{
    inline shared_ptr<RobotInterface> make_shared(shared_ptr<RobotEntity> robotEnt)
    {
        return shared_ptr<RobotInterface>(CreateRobotInstance(robotEnt));
    }
};

#endif