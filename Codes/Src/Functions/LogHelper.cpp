#include "LogHelper.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Debug.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Functions */
#include "Include/Functions/TableIndexHelperInterface.h"


using namespace std;

/**********************class LogHelperInterface**********************/
LogHelperInterface& LogHelperInterface::GetInstance()
{
	/* In C++11, the following is guaranteed to perform thread-safe initialisation: */
	static LogHelper instance;
	return instance;
}

/**********************class LogHelper**********************/
LogHelper::LogHelper()
{}

LogHelper::~LogHelper()
{}

uint32_t LogHelper::GetMessageNumber(uint32_t messageId)
{
    lock_guard<mutex> lock(mtx);
    auto iter = msgIds.find(messageId);
    if (iter == msgIds.end())
        return 0;

    return iter->second;
}

void LogHelper::SetMessageNumber(uint32_t messageId, uint32_t messageNumber)
{
    lock_guard<mutex> lock(mtx);
    auto iter = msgIds.find(messageId);
    if (iter == msgIds.end())
        msgIds.insert(make_pair(messageId, messageNumber));
    else
        iter->second = messageNumber;
}