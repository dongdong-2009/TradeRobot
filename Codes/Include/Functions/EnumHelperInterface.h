#ifndef _EnumHelperInterface_h_
#define _EnumHelperInterface_h_

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Base Type */
#include "Include/Type/BaseType.h"

/**********************class EnumHelperInterface**********************/
class EnumHelperInterface
{
public:
    EnumHelperInterface() {}
    virtual ~EnumHelperInterface() {}
    static EnumHelperInterface& GetInstance();

    virtual const char* GetName(ApiType enumValue) const = 0;
    virtual const char* GetName(OrderType enumValue) const = 0;
    virtual const char* GetName(OrderCanceledReason enumValue) const = 0;
    virtual const char* GetName(CoinType enumValue) const = 0;
    virtual const char* GetName(ChargeType enumValue) const = 0;
    virtual const char* GetName(RobotCfgState enumValue) const = 0;
    virtual const char* GetName(ServiceState enumValue) const = 0;
    virtual const char* GetName(HrNs::OderPricingMode enumValue) const = 0;

    virtual ApiType GetApiType(const char *name) const = 0;
    virtual OrderType GetOrderType(const char *name) const = 0;
    virtual OrderCanceledReason GetOrderCanceledReason(const char *name) const = 0;
    virtual CoinType GetCoinType(const char *name) const = 0;
    virtual ChargeType GetChargeType(const char *name) const = 0;
    virtual RobotCfgState GetRobotCfgState(const char *name) const = 0;
    virtual ServiceState GetServiceState(const char *name) const = 0;
    virtual HrNs::OderPricingMode GetOderPricingMode(const char *name) const = 0;
};

inline const char* GetEnumName(ApiType enumValue)
{
    return EnumHelperInterface::GetInstance().GetName(enumValue);
}

inline const char* GetEnumName(OrderType enumValue)
{
    return EnumHelperInterface::GetInstance().GetName(enumValue);
}

inline const char* GetEnumName(OrderCanceledReason enumValue)
{
    return EnumHelperInterface::GetInstance().GetName(enumValue);
}

inline const char* GetEnumName(CoinType enumValue)
{
    return EnumHelperInterface::GetInstance().GetName(enumValue);
}

inline const char* GetEnumName(ChargeType enumValue)
{
    return EnumHelperInterface::GetInstance().GetName(enumValue);
}

inline const char* GetEnumName(RobotCfgState enumValue)
{
    return EnumHelperInterface::GetInstance().GetName(enumValue);
}

inline const char* GetEnumName(ServiceState enumValue)
{
    return EnumHelperInterface::GetInstance().GetName(enumValue);
}

inline const char* GetEnumName(HrNs::OderPricingMode enumValue)
{
    return EnumHelperInterface::GetInstance().GetName(enumValue);
}

#endif