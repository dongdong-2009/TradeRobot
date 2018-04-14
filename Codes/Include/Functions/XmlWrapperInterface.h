#ifndef _XmlWrapperInterface_h_
#define _XmlWrapperInterface_h_

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

class XmlWrapperInterface
{
public:
    XmlWrapperInterface() {}
    virtual ~XmlWrapperInterface() {}
    static XmlWrapperInterface& GetInstance();

    virtual void Read(const char *xmlPath) = 0;
};

#endif