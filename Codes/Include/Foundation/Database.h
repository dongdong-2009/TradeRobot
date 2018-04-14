#ifndef _Database_h_
#define _Database_h_

#include <memory>

/* ODB */
#include <odb/database.hxx>
#include <odb/session.hxx>
#include <odb/transaction.hxx>
#include <odb/mysql/database.hxx>

#define MaxTransactionRetryNumber 10

/**********************class Database**********************/
/* with multi-thread context, you must set database isolate level, for example (mysql):
use BtcTradeRobot;
SET GLOBAL TRANSACTION ISOLATION LEVEL Serializable;
//SET session TRANSACTION ISOLATION LEVEL Serializable;
SELECT @@GLOBAL.tx_isolation, @@tx_isolation;

with MySql, the isolate levle could be: Read uncommitted,Read committed,Repeatable,Serializable
*/
template<typename T>
class DatabaseTemplate
{
public:
    static DatabaseTemplate<T>& GetInstance()
    {
        /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
        static DatabaseTemplate<T> instance;
        return instance;
    }

	std::shared_ptr<odb::database> GetDatabase() const
	{
		return db;
	}

    std::shared_ptr<odb::session> GetSession() const
    {
        return session;
    }

#ifdef UnitTestMode
    /* 仅限单元测试使用， 几乎每一个单元测试的用例都需要单独的session */
    void ResetSession()
    {
        session = std::make_shared<odb::session>(false);
    }
#endif

private:
    DatabaseTemplate()
	{
#ifdef UnitTestMode
		db = std::make_shared<T>("root", "alyIuN[robot].511025", "BtcTradeRobotUnitTest", "127.0.0.1");
#else
        db = std::make_shared<T>("root", "alyIuN[robot].511025", "BtcTradeRobot", "127.0.0.1");
#endif

        session = std::make_shared<odb::session>(false);
	}

    ~DatabaseTemplate()
	{}

private:
    std::shared_ptr<odb::database> db;
    std::shared_ptr<odb::session> session;
};

typedef DatabaseTemplate<odb::mysql::database> Database;

#define StartTransaction(db, session, retryNumber) \
    for (int iii = 0; iii < retryNumber; ++iii) \
    { \
	    try \
        { \
	        odb::transaction trans(db->begin()); \
			session->current(*session);

#define EndTransaction() \
            session->reset_current(); \
	        trans.commit(); \
	        break; \
        } \
	    catch (const odb::recoverable&) \
        { \
            std::this_thread::sleep_for(std::chrono::microseconds(100)); \
	        continue; \
        } \
    }

#endif
