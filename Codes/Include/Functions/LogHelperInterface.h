#ifndef _LogHelperInterface_h_
#define _LogHelperInterface_h_

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Base Type */
#include "Include/Type/BaseType.h"

/**********************class LogHelperInterface**********************/
class LogHelperInterface
{
public:
    LogHelperInterface() {}
    virtual ~LogHelperInterface() {}
    static LogHelperInterface& GetInstance();

    /* 为了避免同一个log被反复的打印/写数据库, caller 可以为每一个需要打印的代码点分配一个 Msg id, 然后通过判断此msg id
     * 已经被打印的次数来决定是否要再次打印同样的信息.
    */
    virtual uint32_t GetMessageNumber(uint32_t messageId) = 0;
    virtual void SetMessageNumber(uint32_t messageId, uint32_t messageNumber) = 0;
};

#endif