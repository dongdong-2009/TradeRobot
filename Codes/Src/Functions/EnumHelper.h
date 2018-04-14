#ifndef _EnumHelper_h_
#define _EnumHelper_h_

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Functions */
#include "Include/Functions/EnumHelperInterface.h"

/**********************class EnumHelper**********************/
class EnumHelper : public EnumHelperInterface
{
public:
    EnumHelper();
    ~EnumHelper();

    const char* GetName(ApiType enumValue) const;
    const char* GetName(OrderType enumValue) const;
    const char* GetName(OrderCanceledReason enumValue) const;
    const char* GetName(CoinType enumValue) const;
    const char* GetName(ChargeType enumValue) const;
    const char* GetName(RobotCfgState enumValue) const;
    const char* GetName(ServiceState enumValue) const;
    const char* GetName(HrNs::OderPricingMode enumValue) const;

    ApiType GetApiType(const char *name) const;
    OrderType GetOrderType(const char *name) const;
    OrderCanceledReason GetOrderCanceledReason(const char *name) const;
    CoinType GetCoinType(const char *name) const;
    ChargeType GetChargeType(const char *name) const;
    RobotCfgState GetRobotCfgState(const char *name) const;
    ServiceState GetServiceState(const char *name) const;
    HrNs::OderPricingMode GetOderPricingMode(const char *name) const;

private:
    template<typename T>
    static bool CmpEnum(T l, const std::pair<T, const char *>& r)
    {
        return l == r.first;
    }

    template<typename T>
    static bool CmpName(const char *name, const std::pair<T, const char *>& r)
    {
        return strcmp(name, r.second) == 0;
    }

private:
    std::list<std::pair<ApiType, const char*>> apiTypes;
    std::list<std::pair<OrderType, const char*>> orderTypes;
    std::list<std::pair<OrderCanceledReason, const char*>> orderCanceledReasons;
    std::list<std::pair<CoinType, const char*>> coinTypes;
    std::list<std::pair<ChargeType, const char*>> chargeTypes;
    std::list<std::pair<RobotCfgState, const char*>> robotCfgStates;
    std::list<std::pair<ServiceState, const char*>> serviceStates;
    std::list<std::pair<HrNs::OderPricingMode, const char*>> opms;
};

#endif