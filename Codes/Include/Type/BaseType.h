#ifndef _BaseType_h_
#define _BaseType_h_

#include <cstdint>  /* uint8_t, uint16_t, uint32_t ... */
#include <cmath>    /* round() */
#include <iomanip>  /* setiosflags(), setprecision() */
#include <cassert>
#include <sstream>  /* std::ostringstream */
#include <chrono>   /* time_point, duration, seconds, ... */

using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::system_clock;
typedef std::chrono::time_point<std::chrono::system_clock> time_point;

typedef unsigned char uchar_t;
typedef unsigned int uint_t;

/***********************class Double***********************
Example:
void RunExampleRateAndDouble()
{
    Rate r1(1.5);

    auto v1 = r1 * 2.0; //ok, rsult type is Rate.
    auto v2 = 2.0 * r1; //ok, rsult type is Rate.
    auto v3 = r1 / 2.0; //ok, rsult type is Rate.
    auto v4 = 2.0 / r1; //ok, rsult type is Rate.
    cout << "ok" << endl;
}

void RunExampleMoneyAndDouble()
{
    Money m1(10.0);

    auto v1 = m1 * 2.0; //ok, rsult type is Money.
    auto v2 = 2.0 * m1; //ok, rsult type is Money.
    auto v3 = m1 / 2.0; //ok, rsult type is Money.
    //auto v4 = 2.0 / m1; //error C2678
    cout << "ok" << endl;
}

void RunExampleRateAndRate()
{
    Rate r1(1.5), r2(1.5);

    assert(r1 == r2);
    auto v1 = r2 / r1; //ok, rsult type is Rate.
    auto v2 = r1 * r2; //ok, rsult type is Rate.

    cout << "ok" << endl;
}

void RunExampleMoneyAndMoney()
{
    Money m1(10.0), m2(20.0);

    //auto v1 = m1 * m2; //error C2678
    auto v2 = double(m1) * double(m2); //ok, rsult type is double.
    auto v3 = (double)m1 * (double)m2; //ok, rsult type is double.
    auto v4 = static_cast<double>(m1)* static_cast<double>(m2); //ok, rsult type is double.

    cout << "ok" << endl;
}

void RunExampleRateAndMoney()
{
    Rate r1(1.5);
    Money m1(10.0);

    //bool equal = (r1 == m1);  //error C2678
    auto v1 = m1 * r1; //ok, rsult type is Money.
    auto v2 = r1 * m1; //ok, rsult type is Money.
    auto v3 = m1 / r1; //ok, rsult type is Money.
    //auto v4 = r1 / m1; //error C2678
    auto v5 = (double)r1 / (double)m1;

    cout << "ok" << endl;
}

void RunExampleMoneyAndCoinNumber()
{
    Money m1(10.0);
    CoinNumber c1(2.0);

    auto v1 = m1 * c1; //ok, rsult type is Money.
    auto v2 = c1 * m1; //ok, rsult type is Money.
    auto v3 = m1 / c1; //ok, rsult type is Money.
    auto v4 = c1 / m1; //error C2678

    cout << "ok" << endl;
}
int main(int argc, _TCHAR* argv[])
{
    RunExampleRateAndDouble();
    RunExampleMoneyAndDouble();
    RunExampleRateAndRate();
    RunExampleMoneyAndMoney();
    RunExampleRateAndMoney();
    RunExampleMoneyAndCoinNumber();
    return 0;
}
**********************************************************/
template<uint32_t Type, uint32_t Ratio>
class Double
{
public:
    explicit Double(double number) : number(number){}
    Double() : Double(0.0){}
    Double(const Double& right) : number(right.number){}

    static uint32_t GetRatio() { return Ratio; }
    explicit operator double() const { return number; }

    Double& operator+=(const Double& right)
    {
        number = number + (double)right;
        return (*this);
    }

    Double& operator-=(const Double& right)
    {
        number = number - (double)right;
        return (*this);
    }

    template<uint32_t T, uint32_t R>
    Double& operator*=(const Double<T, R>& right)
    {
        number = number * (double)right;
        return (*this);
    }

    template<uint32_t T, uint32_t R>
    Double& operator/=(const Double<T, R>& right)
    {
        assert(!IsEqual(0.0, (double)right));
        number = number / (double)right;
        return (*this);
    }

    Double& operator*=(double right)
    {
        number = number * right;
        return (*this);
    }

    Double& operator/=(double right)
    {
        assert(!IsEqual(0.0, right));
        number = number / right;
        return (*this);
    }

    static Double Zero() { return Double(0); }
    Double Round(int precision)
    {
        int multi = static_cast<int>(pow(10, precision));
        return Double(std::round(number * multi) / multi);
    }

    Double Ceil()
    {
        return Double(ceil(number));
    }

    static bool IsEqual(double left, double right)
    {
        double precision = 1.0 / Ratio;
        return (left - precision < right) && ((left + precision) > right);
    }

    std::string ToString() const
    {
        std::ostringstream os;
        os << std::setiosflags(std::ios_base::fixed) << std::setprecision(4) << number;
        return os.str();
    }

private:
    double number;
};

typedef Double<1, 1000000> Rate;
typedef Double<2, 1000000> Money;
typedef Double<3, 1000000> CoinNumber;

#define DefineOperator(T) \
inline T operator+(const T& left, const T& right) \
{ \
	T v(left); \
	return (v += right); \
} \
inline T operator+(const T& left, double right) \
{ \
	T v(left); \
	return (v += T(right)); \
} \
inline T operator+(double left, const T& right) \
{ \
	T v(left); \
	return (v += right); \
} \
inline T operator-(const T& left, const T& right) \
{ \
	T v(left); \
	return (v -= right); \
} \
inline T operator-(const T& left, double right) \
{ \
	T v(left); \
	return (v -= T(right)); \
} \
inline T operator-(double left, const T& right) \
{ \
	T v(left); \
	return (v -= right); \
} \
inline T operator*(const T& left, double right) \
{ \
	T v(left); \
	return (v *= right); \
} \
inline T operator*(double left, const T& right) \
{ \
	T v(left); \
	return (v *= right); \
} \
inline bool operator==(const T& left, const T& right) \
{ \
	return T::IsEqual((double)left, (double)right); \
} \
inline bool operator!=(const T& left, const T& right) \
{ \
	return !(left == right); \
} \
inline bool operator>(const T& left, const T& right)\
{ \
	return (double)left > (double)right + (1.0 / T::GetRatio()); \
} \
inline bool operator>=(const T& left, const T& right)\
{ \
	return (left > right) || (left == right); \
} \
inline bool operator<(const T& left, const T& right) \
{ \
	return (double)left < (double)right - (1.0 / T::GetRatio()); \
} \
inline bool operator<=(const T& left, const T& right)\
{ \
	return (left < right) || (left == right); \
} \
inline std::ostream& operator << (std::ostream& os, const T& value) \
{ \
	os << value.ToString(); \
	return os; \
}

DefineOperator(Rate);
DefineOperator(Money);
DefineOperator(CoinNumber);

/* Rate and double */
inline Rate operator/(const Rate& left, double right)
{
    Rate v(left);
    return (v /= right);
}

inline Rate operator/(double left, const Rate& right)
{
    Rate v(left);
    return (v /= right);
}

/* Money and double */
inline Money operator/(const Money& left, double right)
{
    Money v(left);
    return (v /= right);
}

/* CoinNumber and double */
inline CoinNumber operator/(const CoinNumber& left, double right)
{
    CoinNumber v(left);
    return (v /= right);
}

/* CoinNumber and CoinNumber */
inline Rate operator/(const CoinNumber& left, const CoinNumber& right)
{
    Rate v((double)left);
    return (v /= right);
}

/* Money and Money */
inline Rate operator/(const Money& left, const Money& right)
{
    Rate v((double)left);
    return (v /= right);
}

/* Rate and Rate */
inline Rate operator*(const Rate& left, const Rate& right)
{
    Rate v(left);
    return (v *= right);
}

inline Rate operator/(const Rate& left, const Rate& right)
{
    Rate v(left);
    return (v /= right);
}

/* Rate and Money */
inline Money operator*(const Money& left, const Rate& right)
{
    Money v(left);
    return (v *= right);
}

inline Money operator*(const Rate& left, const Money& right)
{
    Money v(right);
    return (v *= left);
}

inline Money operator/(const Money& left, const Rate& right)
{
    Money v(left);
    return (v /= right);
}

/* Rate and CoinNumber */
inline CoinNumber operator*(const CoinNumber& left, const Rate& right)
{
    CoinNumber v(left);
    return (v *= right);
}

inline CoinNumber operator*(const Rate& left, const CoinNumber& right)
{
    CoinNumber v(right);
    return (v *= left);
}

inline CoinNumber operator/(const CoinNumber& left, const Rate& right)
{
    CoinNumber v(left);
    return (v /= right);
}

/* Money and CoinNumber */
inline Money operator*(const Money& left, const CoinNumber& right)
{
    Money v(left);
    return (v *= right);
}

inline Money operator*(const CoinNumber& left, const Money& right)
{
    Money v(right);
    return (v *= left);
}

inline Money operator/(const Money& left, const CoinNumber& right)
{
    Money v(left);
    return (v /= right);
}

/* retuslt means: how much coin do 1 yuan can buy */
inline CoinNumber operator/(const CoinNumber& left, const Money& right)
{
    CoinNumber v(left);
    return (v /= right);
}

//typedef int64_t Money;       /* unit: one in a hundred thousand CNY */
//#define MoneyDenominator      100000LL
//inline CoinNumber DoubleToMoney(double money) { return Money(MoneyDenominator * money); }
//inline double MoneyToDouble(Money money) { return (double)money / MoneyDenominator; }

//#define CoinNumberDenominator 100000LL
//typedef int64_t CoinNumber;  /* unit: one in a hundred thousand btc/ltc/doge ...*/
//inline CoinNumber DoubleToCoinNumber(double coinNumber) { return CoinNumber(CoinNumberDenominator * coinNumber); }
//inline double CoinNumberToDouble(CoinNumber coinNumber) { return (double)coinNumber / CoinNumberDenominator; }

typedef uint32_t TableId;
typedef uint64_t PlatformOrderId;
typedef uint64_t Nonce;

enum class ServiceState
{
    ServiceStopped,
    ServiceRunning,
};

enum class Severity
{
	Success,
	Fail
};

enum class ApiType
{
    Buy,
    Sell,
    CancelOrder,
    FetchOrder,
    FetchOrders,
    FetchBalance,
};

enum class CoinType
{
	Btc,  //0
	Dog,
	Eth,
	Etc,
	Ltc,
	Ybc
};

enum class OrderType
{
	Buy,
	Sell
};

enum class OrderCanceledReason
{
	CanceledByCmd,
	CanceledByIncAskPrice,
	CanceledByDecAskPrice,
	CanceledByIncBidPrice,
	CanceledByDecBidPrice,
	CanceledByFilledSellOrder,
    CanceledByOrder2ndTimeOut
};

enum class RobotCfgState
{
	Enabled,
	Disabled
};

/* charge type when withdrawl/transaction coin/money from trade platform */
enum class ChargeType
{
    FixedFee,
    FloatingRate
};

#define InvalidTableId            0

/* Buy Low Sell High Robot*/
typedef uint32_t BlshrPricePointNumber;

/* //Hedge Robot Name Space */
namespace HrNs
{
    enum class OrderYieldType
    {
        Normal,
        Clearance,
    };

	//Hedge Robot Worker Margin Level Number
    // -1 <= AccountGroupMln <= 1
	typedef int32_t AccountGroupMln;

	//Hedge Robot Oder Pricing Mode
	enum class OderPricingMode : uint32_t
	{
        AskAskPrice = 0,  /* Ask Price for Buying Order, Bid Price for Selling Order */
        AskBidPrice,      /* Ask Price for Buying Order, Ask Price for Selling Order */
		AskLastPrice,     /* Ask Price for Buying Order, Last Price for Selling Order */
        BidAskPrice,
        BidBidPrice,      /* Bid Price for Buying Order, Bid Price for Selling Order */
        BidLastPrice,
        LastAskPrice,
        LastBidPrice,     /* Last Price for Buying Order, Bid Price for Selling Order */
        LastLastPrice,
    };
    #define OderPricingModeNumber ((uint32_t)OderPricingMode::End - (uint32_t)OderPricingMode::Begin + 1)

    #define ZeroOrderPricingMode 0x0
    #define AllOrderPricingMode  0x1FF     /* All mode for PlatformA ~ PlatformB */
    #define MakePtmABFlag(A, B) ((1 << uint32_t(B)) << (uint32_t(A)* OderPricingModeNumber))
    //Hedge Robot Oder Pricing Mode Flag
	typedef uint32_t OrderPricingModeFlag;
};  //namespace HrNs

#endif