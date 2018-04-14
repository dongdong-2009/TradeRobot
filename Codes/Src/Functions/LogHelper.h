#ifndef _LogHelper_h_
#define _LogHelper_h_

/* Fundation */
#include "Include/Foundation/SystemInclude.h"

/* Base Type */
#include "Include/Type/BaseType.h"

/* Functions */
#include "Include/Functions/LogHelperInterface.h"

/**********************class LogHelper**********************/
class LogHelper: public LogHelperInterface
{
public:
    LogHelper();
    ~LogHelper();

    uint32_t GetMessageNumber(uint32_t messageId);
    void SetMessageNumber(uint32_t messageId, uint32_t messageNumber);

private:
    std::mutex mtx;
    std::map<uint32_t, uint32_t> msgIds;
};

#endif
