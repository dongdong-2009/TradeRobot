#include "Include/Foundation/ResultCode.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

using namespace std;

const char* ConvertErrCodeToString(uint32_t errCode)
{
    static map<uint32_t, const char*> errors =
    {
        { OptResultCurlCouldntResolveHost, "CURLE_COULDNT_RESOLVE_HOST" },
        { OptResultCurlCouldntConnect, "CURLE_COULDNT_CONNECT" },
        { OptResultCurlOperationTimedout, "CURLE_OPERATION_TIMEDOUT" },
        { OptResultCurlGotNothing, "CURLE_GOT_NOTHING" },
		{ OptResultCurlSendError, "CURLE_SEND_ERROR" },
		{ OptResultCurlRecvError, "CURLE_RECV_ERROR" },
        { OptResultUnknownPtmError, "OptResultUnknownPtmError" },
        { OptResultNonceHasExpired, "OptResultNonceHasExpired" },
        { OptResultOptTooFrequent, "OptResultOptTooFrequent" },
        { OptResultResourceNotEnough, "OptResultResourceNotEnough" },
        { OptResultMoneyNotEnough, "OptResultMoneyNotEnough" },
        { OptResultCoinNotEnough, "OptResultCoinNotEnough" },
        { OptResultNoMatchedOrderId, "OptResultNoMatchedOrderId" },
        { OptResultParameterError, "OptResultParameterError" },
        { OptResultOrderIsClosed, "OptResultOrderIsClosed" },
        { OptResultCancelOptRepeated, "OptResultCancelOptRepeated" },
        { OptResultJsonFailed, "OptResultJsonFailed" },
		{ OptResulOcdcSystemError, "OptResulOcdcSystemError" }
    };
    static const char *unknown = "unknown error code";

    auto iter = errors.find(errCode);
    if (iter == errors.end())
        return unknown;
    return iter->second;
}