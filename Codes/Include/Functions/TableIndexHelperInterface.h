#ifndef _TableIndexHelperInterface_h_
#define _TableIndexHelperInterface_h_

#include <memory>  // std::std::shared_ptr, std::weak_ptr

/* Base Type */
#include "Include/Type/BaseType.h"

namespace odb
{
    class database;
};

/**********************class TableIndexHelperInterface**********************/
class TableIndexHelperInterface
{
protected:
	TableIndexHelperInterface() {}
	virtual ~TableIndexHelperInterface() {}

public:
    static TableIndexHelperInterface& GetInstance();

    virtual TableId GetUseableTableIndex(const char *tableName) = 0;
};

#endif