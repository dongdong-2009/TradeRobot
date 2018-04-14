#ifndef _ResultCode_h_
#define _ResultCode_h_

#include <cstdint>

/************************************** ResultCode **************************************/
//  ResultCode are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+---------------------+-------------------------------+
//  |S|F|                      Error Code                           |
//  +-+-+-+-+-+---------------------+-------------------------------+
//      S - Severity - indicates success/fail
//
//          0 - Success
//          1 - Fail (COERROR)
//
//      F - Fixable - indicates if this error can be fixed by retry
//
//          0 - false
//          1 - true

typedef uint32_t ResultCode;

#define ResultCodeOk     ((uint32_t)0L)

#define SBitOk           ((uint32_t)0L)
#define SBitNotOk        ((uint32_t)1L)
#define FBitFalse        ((uint32_t)0L)
#define FBitTrue         ((uint32_t)1L)

inline ResultCode MakeResultCode(uint32_t s, uint32_t f, uint32_t errCode)
{
    return (((s & 0x1) << 31) | ((f & 0x1) << 30) | (errCode & 0x3FFFFFFF));
}

inline uint32_t GetResultSBit(ResultCode resultCode){ return (resultCode >> 31); }

inline uint32_t GetResultFBit(ResultCode resultCode){ return ((resultCode >> 30) & 0x1); }
inline void SetResultFBit(ResultCode& resultCode, uint32_t f)
{
    resultCode = (resultCode & (~(1 << 30))) | (f << 30);
}

inline uint32_t GetResultErrCode(ResultCode resultCode){ return (resultCode & 0x3FFFFFFF); }

#define OptResultUnknownCurlError         1001
#define OptResultCurlCouldntResolveHost   (1000 + 6)  //CURLE_COULDNT_RESOLVE_HOST
#define OptResultCurlCouldntConnect       (1000 + 7)  //CURLE_COULDNT_CONNECT
#define OptResultCurlOperationTimedout    (1000 + 28) /* 28 - the timeout time was reached */
#define OptResultCurlGotNothing           (1000 + 52) /* 52 - when this is a specific error */
#define OptResultCurlSendError            (1000 + 55) /* 55 - failed sending network data */
#define OptResultCurlRecvError            (1000 + 56) /* 56 - failure in receiving network data */

#define OptResultUnknownPtmError          2001
#define OptResultNonceHasExpired          2002
#define OptResultOptTooFrequent           2003
#define OptResultResourceNotEnough        2004
#define OptResultMoneyNotEnough           2005
#define OptResultCoinNotEnough            2006
#define OptResultNoMatchedOrderId         2007
#define OptResultParameterError           2008
#define OptResultOrderIsClosed            2009
#define OptResultCancelOptRepeated        2010
//failed to parse json string
#define OptResultJsonFailed               2011
#define OptResulOcdcSystemError           2012  //okcoin.cn 返回10002错误码，"系统错误"

const char* ConvertErrCodeToString(uint32_t errCode);

#endif