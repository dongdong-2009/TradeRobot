#ifndef _OcdcCmdString_h_
#define _OcdcCmdString_h_

#include "Include/Module/Module.h"
#ifdef HasOkCoinDotCnPlatform

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/**********************class OcdcCmdString**********************/
class OcdcCmdString
{
public:
    OcdcCmdString();
    ~OcdcCmdString();


    void AddParameter(const char *para, uint32_t value);
    void AddParameter(const char *para, uint64_t value);
    void AddParameter(const char *para, const std::string& value);
    void AddParameter(const char *para, const Rate& value);
    void AddParameter(const char *para, const Money& value);
    void AddParameter(const char *para, const CoinNumber& value);
    void AddParameter(const char *para, const CoinType& value);
    void AddParameter(const char *para, const OrderType& value);

    std::string GetCmdString();
    std::string GetSign(const std::string& privateKey);

private:
    /* disable default constructor */
    OcdcCmdString(const OcdcCmdString& right);

private:
    std::map<std::string, std::string> pairs;
};

#endif //#ifdef HasOkCoinPlatform
#endif