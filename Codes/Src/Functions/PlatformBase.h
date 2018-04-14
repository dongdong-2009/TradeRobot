#ifndef _PlatformBase_h_
#define _PlatformBase_h_

/* Curl*/
#ifdef __CYGWIN__
#   include <sys/select.h>
#endif
#include <curl/curl.h>

/* Base Type */
#include "Include/Type/BaseType.h"

/* Fundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/ResultCode.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* Functions */
#include "Include/Functions/MarketPollerInterface.h"
#include "Include/Functions/LogHelperInterface.h"
#include "Include/Functions/PlatformInterface.h"

/* local header files */

struct AccountInfo
{
    std::mutex tradeMtx;
    std::mutex fetchMtx;
    std::chrono::system_clock::time_point lastTradeTimePoint;
    std::chrono::system_clock::time_point lastFetchTimePoint;
};

#define CheckDeadlineAndFBit(deadline, resultCode, str) \
	if (deadline < system_clock::now() || GetResultFBit(resultCode) == FBitFalse) \
	{ \
		LOG4_FATAL(str); \
		return false; \
	}

#define CheckJsonResult(err) \
	if (!err.empty()) \
	{ \
        LOG4_WARN("failed, err=" << err); \
		resultCode = MakeResultCode(SBitNotOk, FBitTrue, OptResultJsonFailed); \
		return (nmemb * size); \
	}

class PlatformBase : public PlatformInterface
{
public:
    PlatformBase();
    virtual ~PlatformBase();

    std::shared_ptr<PlatformEntity> GetPlatformEntity();
    void SetPlatformEntity(std::shared_ptr<PlatformEntity> ptmEnt);

    const std::string& GetPlatformName() const;

    milliseconds GetMinMarketQueryInterval();
    void SetMinMarketQueryInterval(milliseconds minMarketQueryInterval);

    milliseconds GetMinOrderQueryInterval();
    void SetMinOrderQueryInterval(milliseconds minOrderQueryInterval);

    CoinNumber GetCoinMinTradingUnit(CoinType coinType) const;

    /* Function inherited from PlatformInterface class. */
    bool CreateOrder(std::shared_ptr<OrderEntity> orderEnt);
    void CancelOrder(std::shared_ptr<OrderEntity> orderEnt);
    void FetchOrder(std::shared_ptr<OrderEntity> orderEnt);
    bool FetchUnknownOrder(std::shared_ptr<OrderEntity> orderEnt);
    void FetchBalance(std::shared_ptr<AccountEntity> accEnt, Money& balance);
    void FetchBalance(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance);

    void RegisterMarketHandler(TableId robotId, CoinType coinType, MarketHandler handler);
    void DeregisterMarketHandler(TableId robotId, CoinType coinType);

    virtual void UpdateAvailableStep0(shared_ptr<OrderEntity> orderEnt);
    virtual void UpdateAvailableStep1(shared_ptr<OrderEntity> orderEnt);

    void Save();

protected:
    uint32_t ConvertCurlCodeToErrCode(CURLcode curlCode);
    bool IsIgnorableCurlError(CURLcode curlCode);
    bool IsCurlInterruptedError(uint32_t errCode);

    typedef std::function<size_t(void *, size_t, size_t, ResultCode&)> HttpStringHandlerX;
    ResultCode ExecuteHttpCommand(CURL *&curl, std::shared_ptr<PlatformApiRuleEntity> apiRule,
        const char* cmd, std::shared_ptr<AccountEntity> accEnt, HttpStringHandlerX handlerX);

    virtual CURL* CreateCurlInstance(const char *url, std::chrono::seconds timeout) = 0;
    virtual bool CreateBuyOrderImpl(std::shared_ptr<OrderEntity> orderEnt) = 0;
    virtual bool CreateSellOrderImpl(std::shared_ptr<OrderEntity> orderEnt) = 0;
    virtual bool CancelOrderImpl(std::shared_ptr<OrderEntity> orderEnt) = 0;
    virtual bool FetchOrderImpl(std::shared_ptr<OrderEntity> orderEnt) = 0;
    virtual bool FetchUnknownOrderImpl(std::shared_ptr<OrderEntity> orderEnt) = 0;
    virtual bool FetchBalanceImpl1(std::shared_ptr<AccountEntity> accEnt, Money& balance) = 0;
    virtual bool FetchBalanceImpl2(std::shared_ptr<AccountEntity> accEnt, CoinType coinType, CoinNumber& balance) = 0;

    bool HandleMarketInfor(CoinType coinType, Money bidPrice, Money askPrice, Money lastPrice);

private:
    std::shared_ptr<AccountInfo> GetAccountInfo(TableId accountId);

protected:
    std::mutex ptmEntMtx;
    std::shared_ptr<PlatformEntity> ptmEnt;

private:
    std::mutex handlersMtx;
    typedef std::map<std::pair<TableId, CoinType>, MarketHandler> MarketHandlerRepertory;
    MarketHandlerRepertory marketHandlers;

    typedef std::map<CoinType, std::shared_ptr<MarketPollerInterface>> MarketPollerRepertory;
    MarketPollerRepertory  marketPollers;

	time_t marketTimeStamp;
};

#endif