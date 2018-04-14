#ifndef _XmlWrapper_h_
#define _XmlWrapper_h_

#include "Include/Functions/XmlWrapperInterface.h"

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/**********************class XmlWrapper**********************/
class XmlWrapper : public XmlWrapperInterface
{
public:
    XmlWrapper() {}
    ~XmlWrapper() {}

    void Read(const char *xmlPath);

private:
    void ClearAllTables();    
    void InitTableIdTable();
    void InitNonceTable();
};

#endif