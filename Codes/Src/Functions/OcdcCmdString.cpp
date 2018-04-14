#include "OcdcCmdString.h"

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Md5HmacSha256.h"

/* Functions */
#include "Include/Functions/EnumHelperInterface.h"

#ifdef HasOkCoinDotCnPlatform
using namespace std;

/**********************class OcdcCmdString**********************/
OcdcCmdString::OcdcCmdString()
{}

OcdcCmdString::~OcdcCmdString()
{}

void OcdcCmdString::AddParameter(const char *para, uint32_t value)
{
    ostringstream os;
    os << value;
    pairs.insert(make_pair(para, os.str()));
}

void OcdcCmdString::AddParameter(const char *para, uint64_t value)
{
    ostringstream os;
    os << value;
    pairs.insert(make_pair(para, os.str()));
}

void OcdcCmdString::AddParameter(const char *para, const std::string& value)
{
    pairs.insert(make_pair(para, value));
}

void OcdcCmdString::AddParameter(const char *para, const Rate& value)
{
    pairs.insert(make_pair(para, value.ToString()));
}

void OcdcCmdString::AddParameter(const char *para, const Money& value)
{
    pairs.insert(make_pair(para, value.ToString()));
}

void OcdcCmdString::AddParameter(const char *para, const CoinNumber& value)
{
    pairs.insert(make_pair(para, value.ToString()));
}

void OcdcCmdString::AddParameter(const char *para, const CoinType& value)
{
    string coinName = EnumHelperInterface::GetInstance().GetName(value) + string("_cny");
    pairs.insert(make_pair(para, coinName));
}

void OcdcCmdString::AddParameter(const char *para, const OrderType& value)
{
    pairs.insert(make_pair(para, EnumHelperInterface::GetInstance().GetName(value)));
}

std::string OcdcCmdString::GetCmdString()
{
    string result;    

    for (auto iter = pairs.begin(); iter != pairs.end(); ++iter)
    {
        if (result.size() != 0)
            result.append("&");

        result.append(iter->first);
        result.append("=");
        result.append(iter->second);
    }

    return result;
}

std::string OcdcCmdString::GetSign(const std::string& privateKey)
{
    string cmd = GetCmdString();
    cmd.append("&secret_key=");
    cmd.append(privateKey);

    return GetMd5(cmd.c_str());
}

#endif