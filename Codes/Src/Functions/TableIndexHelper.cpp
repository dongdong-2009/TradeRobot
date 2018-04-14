#include "TableIndexHelper.h"

/* Foundation */
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Debug.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

using namespace std;

/**********************class TableIndexHelperInterface**********************/
TableIndexHelperInterface& TableIndexHelperInterface::GetInstance()
{
	/* In C++11, the following is guaranteed to perform thread-safe initialisation: */
	static TableIndexHelper instance;
	return instance;
}

/**********************class TableIndexHelper**********************/
TableIndexHelper::TableIndexHelper()
{}

TableIndexHelper::~TableIndexHelper()
{}

TableId TableIndexHelper::GetUseableTableIndex(const char *tableName)
{
    TableId tableId;
	std::shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
	std::shared_ptr<odb::session> session = Database::GetInstance().GetSession();

    if (odb::transaction::has_current())
    {
        tableId = GetUseableTableIndexImpl(db, tableName);
    }
    else
    {
		StartTransaction(db, session, MaxTransactionRetryNumber);
        tableId = GetUseableTableIndexImpl(db, tableName);
        EndTransaction();
    }

    return tableId;
}

/* private function */
TableId TableIndexHelper::GetUseableTableIndexImpl(shared_ptr<odb::database> db, const char *tableName)
{
    shared_ptr<TableIdEntity> tableIdEntity = db->load<TableIdEntity>(tableName);
    TableId tableId = tableIdEntity->GetTableId();
    tableIdEntity->SetTableId(tableId + 1);

    db->update(tableIdEntity);
    return tableId;
}