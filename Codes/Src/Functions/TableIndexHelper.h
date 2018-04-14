#ifndef _TableIndexHelper_h_
#define _TableIndexHelper_h_

#include "Include/Functions/TableIndexHelperInterface.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"  /* std::shared_ptr */

using std::shared_ptr;
namespace odb
{
    class database;
};

/**********************class OrderEntityIndexHelper**********************/
class TableIndexHelper : public TableIndexHelperInterface
{
public:
    TableIndexHelper();
    ~TableIndexHelper();

    TableId GetUseableTableIndex(const char *tableName);

private:
    TableId GetUseableTableIndexImpl(shared_ptr<odb::database> db, const char *tableName);
};

#endif