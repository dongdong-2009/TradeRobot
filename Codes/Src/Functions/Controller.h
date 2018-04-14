#ifndef _Controller_h_
#define _Controller_h_

#include "Include/Functions/ControllerInterface.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

class RobotEntity;
class RobotInterface;

/**********************class Controller**********************/
class Controller : public ControllerInterface
{
public:
    Controller();
    ~Controller();
    static ControllerInterface* GetInstance();

    void Start(std::shared_ptr<RobotEntity> robotEnt);
    void Stop();
    void PrintRuntimeStatus(std::ostream& os) const;

private:
    //find by robot id
    std::shared_ptr<RobotInterface> Find(TableId id);

private:
    std::list<std::shared_ptr<RobotInterface>> robots;
};

#endif