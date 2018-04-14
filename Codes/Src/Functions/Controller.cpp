#include "Controller.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* log4cplus */
#include <log4cplus/logger.h>           //class Logger
#include <log4cplus/loggingmacros.h>    //LOG4CPLUS_TRACE()

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
#include "Include/Functions/RobotInterface.h"
#include "Include/Functions/PlatformInterface.h"
#include "Include/Functions/TableIndexHelperInterface.h"

using namespace std;
using namespace placeholders;

typedef ClassFactoriesRegistor<ControllerInterface, int> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, 0, Controller::GetInstance);

/**********************class Controller**********************/
/* public function */
Controller::Controller()
{
}

Controller::~Controller()
{}

/* static function */
ControllerInterface* Controller::GetInstance()
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation. */
    static Controller instance;
    return &instance;
}

void Controller::Start(std::shared_ptr<RobotEntity> robotEnt)
{
    shared_ptr<RobotInterface> robot = Find(robotEnt->GetId());
    if (robot == nullptr)
    {
        robot = make_shared(robotEnt);;
        robots.push_back((robot));

        robot->Start();
    }
}

void Controller::Stop()
{
    LOG4_TRACE("Start, robots.size()=" << robots.size());
    for (list<shared_ptr<RobotInterface>>::iterator iter = robots.begin(); iter != robots.end(); ++iter)
    {
        (*iter)->Stop();
    }
    robots.clear();
    LOG4_TRACE("Succeeded, robots.size()=" << robots.size());
}

void Controller::PrintRuntimeStatus(std::ostream& os) const
{
    os << "total robot number: " << robots.size() << endl;
    for (list<shared_ptr<RobotInterface>>::const_iterator iter = robots.cbegin(); iter != robots.cend(); ++iter)
    {
        (*iter)->PrintRuntimeStatus(os);
    }
}

/* private functions */
shared_ptr<RobotInterface> Controller::Find(TableId id)
{
    list<shared_ptr<RobotInterface>>::iterator iter;
    for (iter = robots.begin(); iter != robots.end(); ++iter)
    {
        if (id == (*iter)->GetRobotId())
            break;
    }

    if (iter == robots.end())
        return nullptr;

    return *iter;
}