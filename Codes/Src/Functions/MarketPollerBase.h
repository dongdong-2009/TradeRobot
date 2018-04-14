#ifndef _MarketPollerBase_h_
#define _MarketPollerBase_h_

/* Curl*/
#ifdef __CYGWIN__
#   include <sys/select.h>
#endif
#include <curl/curl.h>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/ResultCode.h"

/* Functions */
#include "Include/Functions/MarketPollerInterface.h"
#include "Include/Functions/LogHelperInterface.h"

#define CheckJsonResult(err) \
	if (!err.empty()) \
    	{ \
        LOG4_WARN("failed, err=" << err); \
		resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed); \
		return (nmemb * size); \
    	}

/**********************class MarketPollerBase**********************/
class MarketPollerBase : public MarketPollerInterface
{
public:
    MarketPollerBase(milliseconds interval);
    ~MarketPollerBase();

    void Start();
    void Stop();
    bool IsRunning();

protected:
    bool IsIgnorableCurlError(CURLcode curlCode);

    typedef std::function<size_t(void *, size_t, size_t, ResultCode&)> HttpStringHandlerX;
    ResultCode ExecuteHttpCommand(CURL *&curl, HttpStringHandlerX handlerX);
	
    virtual void QueryMarketInfoImpl() = 0;
    virtual CURL* CreateCurlInstance() = 0;

private:
    /* New function added by this class */
    void ThreadMain();

private:
    /* member variable initialized by constructor */
    std::mutex mtx;
    std::condition_variable cv;
    bool isOnGoing;
    std::chrono::system_clock::duration interval;

    std::thread threadMain;              /* initialize by Start() */
};

#endif