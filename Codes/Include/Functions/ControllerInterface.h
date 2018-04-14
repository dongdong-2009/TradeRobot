#ifndef _ControllerInterface_h_
#define _ControllerInterface_h_

#include <memory>
#include <ostream>
#include "Include/Foundation/ClassFactories.h"

class RobotEntity;

/**********************class ControllerInterface**********************/
class ControllerInterface
{
public:
    ControllerInterface() {};
    virtual ~ControllerInterface() {};

    virtual void Start(std::shared_ptr<RobotEntity> robotEnt) = 0;
    virtual void Stop() = 0;
    virtual void PrintRuntimeStatus(std::ostream& os) const = 0;
};

/**********************class ControllerInterface Factory**********************/
inline ControllerInterface& GetControllerInterface()
{
    typedef ClassFactories<ControllerInterface, int> ClassFactoriesType;
    ClassFactoriesType& instance = ClassFactoriesType::GetInstance();

    return *instance.CreateInstance(0);
}

#endif