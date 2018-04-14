/* Curl*/
#include <curl/curl.h>

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* log4cplus */
#include <log4cplus/logger.h>           //class Logger
#include <log4cplus/configurator.h>     //class PropertyConfigurator
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
#include "Include/Functions/ControllerInterface.h"

/* local header files */
#include "Version.h"

#if defined(_WIN32) && defined(_MSC_VER)
#    if defined(_DEBUG)
#        pragma comment(lib, "log4cplus.win32.d.lib")
#    else
#        pragma comment(lib, "log4cplus.win32.r.lib")
#    endif
#endif

using namespace std;
using namespace log4cplus;

static bool running = true;
static BOOL CtrlHandler(DWORD fdwCtrlType)
{
    if (fdwCtrlType == CTRL_C_EVENT || fdwCtrlType == CTRL_CLOSE_EVENT)
    {
        running = false;
        return TRUE;
    }

    return FALSE;
}

int main(int argc, char **argv)
{
    /* to keep there is only one instance is running */
    HANDLE mutex;
    mutex = CreateMutex(NULL, TRUE, TEXT("EpgSender"));
    if ((mutex != nullptr) && (GetLastError() == ERROR_ALREADY_EXISTS))
    {
        /* One another instance is running! */
        return 1;
    }

    log4cplus::initialize();
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

    LOG4_INFO("Software Svn Version: " << ExeVersion);

    CURLcode curlCode = curl_global_init(CURL_GLOBAL_ALL);
    assert(curlCode == CURLE_OK);

    /* 注册中断处理函数， 处理Ctrl+C 和 Ctrl+CLOSE 消息 */
    auto result = SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
    assert(result);

    /* 查询robot， 并启动robot */
    std::list<std::shared_ptr<RobotEntity>> robotEnts;
    shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
    shared_ptr<odb::session> session = Database::GetInstance().GetSession();

    StartTransaction(db, session, MaxTransactionRetryNumber);
    RobotResult robotResult(db->query<RobotEntity>());
    for (auto i = robotResult.begin(); i != robotResult.end(); ++i)
    {
        robotEnts.push_back(i.load());
    }
    EndTransaction();
    ControllerInterface& controller = GetControllerInterface();
    for (auto i = robotEnts.begin(); i != robotEnts.end(); ++i)
    {
        controller.Start(*i);
    }

    while (running)
    {
        this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    controller.Stop();

    curl_global_cleanup();
    if (mutex != nullptr)
    {
        ReleaseMutex(mutex);
        CloseHandle(mutex);
    }
    return 0;
}