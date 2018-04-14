#ifndef _OcdcPlatform_h_
#define _OcdcPlatform_h_

#include "Include/Module/Module.h"
#ifdef HasOkCoinDotCnPlatform

/* json */
#include "picojson/picojson.h"

/* Curl */
#ifdef __CYGWIN__
#   include <sys/select.h>
#endif
#include <curl/curl.h>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Fundation */
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/ResultCode.h"

/* Functions */
#include "Include/Functions/PlatformInterface.h"

/* local header files */
#include "PlatformBase.h"
#include "CurlOutputHandler.h"

/* https://curl.haxx.se/libcurl/c/threadsafe.html
Handles. You must never share the same handle in multiple threads. You can pass the handles
around among threads, but you must never use a single handle from more than one thread at
any given time.
这里将官方文档的链接记录下来, 如果解决不了 fetch order 时碰到的unknown error, 就为每一个线程分配单独的
curl句柄.
*/
struct AccountCurlInfo
{
    CURL *buyCurl;
    CURL *sellCurl;
    CURL *cancelCurl;
    CURL *fetchOrderCurl;
    CURL *fetchOrdersCurl;
    CURL *fetchBalanceCurl;
};

/**********************class OcdcPlatform**********************/
/*
please refer to <The.ACE.Programmers.Guide.chm>, chapter 16.2 Half-Sync/Half-Async Model
for more information about design model of class OcdcPlatform.
*/
class OcdcPlatform : public PlatformBase
{
public:
    OcdcPlatform();
    ~OcdcPlatform();
    /* Static function */
    static PlatformInterface* CreateInstance(std::shared_ptr<PlatformEntity> ptmEnt);

    Money GetRoundedPrice(Money price, CoinType coinType) const;

private:
    CURL* CreateCurlInstance(const char *url, std::chrono::seconds timeout);
    std::shared_ptr<AccountCurlInfo> GetAccountCurlInfo(TableId accountId);

    bool TryToCreateBuyOrder(std::shared_ptr<OrderEntity> orderEnt, time_point deadline);
    bool CreateBuyOrderImpl(std::shared_ptr<OrderEntity> orderEnt);

    bool TryToCreateSellOrder(std::shared_ptr<OrderEntity> orderEnt, time_point deadline);
    bool CreateSellOrderImpl(std::shared_ptr<OrderEntity> orderEnt);

    bool TryToCancelOrder(std::shared_ptr<OrderEntity> orderEnt, time_point deadline);
    bool CancelOrderImpl(std::shared_ptr<OrderEntity> orderEnt);

    bool TryToFetchOrder(std::shared_ptr<OrderEntity> orderEnt, time_point deadline);
    bool FetchOrderImpl(std::shared_ptr<OrderEntity> orderEnt);

    bool TryToFetchUnknownOrder(std::shared_ptr<OrderEntity> orderEnt, time_point deadline);
    bool FetchUnknownOrderImpl(std::shared_ptr<OrderEntity> orderEnt);

    bool TryToFetchBalance1(std::shared_ptr<AccountEntity> accEnt, Money& balance, time_point deadline);
    bool FetchBalanceImpl1(std::shared_ptr<AccountEntity> accEnt, Money& balance);

    bool TryToFetchBalance2(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance,
        time_point deadline);
    bool FetchBalanceImpl2(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance);

    /* functions defined by this class */
    size_t HandleHttpOrderCreateString(void *ptr, size_t size, size_t nmemb,
        ResultCode& resultCode, std::shared_ptr<OrderEntity> orderEnt);

    size_t HandleHttpOrderCancelString(void *ptr, size_t size, size_t nmemb,
        ResultCode& resultCode, std::shared_ptr<OrderEntity> orderEnt);

    size_t HandleHttpOrderFetchString(void *ptr, size_t size, size_t nmemb,
        ResultCode& resultCode, std::shared_ptr<OrderEntity> orderEnt);

    size_t HandleHttpUnknownOrderFetchString(void *ptr, size_t size, size_t nmemb,
        ResultCode& resultCode, std::shared_ptr<OrderEntity> orderEnt);

    size_t HandleHttpBalanceFetchString1(void *ptr, size_t size, size_t nmemb,
        ResultCode& resultCode, Money& balance);

    size_t HandleHttpBalanceFetchString2(void *ptr, size_t size, size_t nmemb,
        ResultCode& resultCode, CoinType coinType, CoinNumber& balance);

    uint32_t ConvertToErrCode(uint32_t ocdcCode);
};

#endif //#ifdef HasOkCoinPlatform
#endif