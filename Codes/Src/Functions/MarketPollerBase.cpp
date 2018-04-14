#include "MarketPollerBase.h"

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/Md5HmacSha256.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Functions */
#include "Include/Functions/EnumHelperInterface.h"
#include "Include/Functions/TableIndexHelperInterface.h"

/* local header files */
#include "CurlOutputHandler.h"

using namespace std;
using namespace std::placeholders;

/**********************class MarketPoller**********************/
/* public function */
MarketPollerBase::MarketPollerBase(milliseconds interval)
{
    isOnGoing = false;
    this->interval = milliseconds(interval);
}

MarketPollerBase::~MarketPollerBase()
{}

void MarketPollerBase::Start()
{
    std::lock_guard<std::mutex> lock(mtx);
    isOnGoing = true;
    threadMain = std::thread(bind(&MarketPollerBase::ThreadMain, this));
}

void MarketPollerBase::Stop()
{
    LOG4_TRACE("Start");
    {
        std::lock_guard<std::mutex> lock(mtx);
        isOnGoing = false;
        cv.notify_one();
    }
    threadMain.join();
    LOG4_TRACE("Succeeded");
}

bool MarketPollerBase::IsRunning()
{
    lock_guard<mutex> lock(mtx);
    return isOnGoing;
}

/* protected functions */
bool MarketPollerBase::IsIgnorableCurlError(CURLcode curlCode)
{
	/* 因为我们所有的RPC调用都经过了基本的单元测试, 所以理论上来说, 所有的curl错误都可以视为可被忽略的错误.
	* 在此记录可忽略的curl 错误列表是为了将来碰到某些特别的不能被忽略的curl错误时可以同可忽略的错误区别对待.
	*/
    static list<CURLcode> ignorableCurlErrores =
    {
		CURLE_COULDNT_CONNECT,
		CURLE_COULDNT_RESOLVE_HOST,
		CURLE_OPERATION_TIMEDOUT,
		CURLE_SSL_CONNECT_ERROR,
		CURLE_GOT_NOTHING,
		CURLE_SEND_ERROR,
		CURLE_RECV_ERROR
    };

    list<CURLcode>::iterator iter = find(ignorableCurlErrores.begin(), ignorableCurlErrores.end(), curlCode);
    return !(iter == ignorableCurlErrores.end());
}

ResultCode MarketPollerBase::ExecuteHttpCommand(CURL *&curl, HttpStringHandlerX handlerX)
{
    CURLcode curlCode;
    ResultCode resultCode = ResultCodeOk;
    HttpStringHandler handler = bind(handlerX, _1, _2, _3, ref(resultCode));
    curlCode = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&handler);
    assert((curlCode == CURLE_OK));

    curlCode = curl_easy_perform(curl);
    if (curlCode != CURLE_OK)
    {
        if (IsIgnorableCurlError(curlCode))
            resultCode = MakeResultCode(SBitNotOk, FBitTrue, 1000 + curlCode);
        else
            resultCode = MakeResultCode(SBitNotOk, FBitFalse, 1000 + curlCode);

        curl_easy_cleanup(curl);
        curl = CreateCurlInstance();
    }

    return resultCode;
}

/* private functions */
void MarketPollerBase::ThreadMain()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, interval, [this]{return !isOnGoing; }))
            break;

        QueryMarketInfoImpl();
    }
}