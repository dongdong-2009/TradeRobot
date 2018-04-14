#include "EnumHelper.h"

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

using namespace std;
using namespace std::placeholders;

/**********************class EnumHelperInterface**********************/
EnumHelperInterface& EnumHelperInterface::GetInstance()
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
    static EnumHelper instance;
    return instance;
}

/**********************class EnumHelper**********************/
EnumHelper::EnumHelper()
    : apiTypes({ { ApiType::Buy, "buy" }, { ApiType::Sell, "sell"},
                 { ApiType::CancelOrder, "cancel_order"},
                 { ApiType::FetchOrder, "fetch_order"},
                 { ApiType::FetchOrders, "fetch_orders" },
                 { ApiType::FetchBalance, "fetch_balance"} }),
      orderTypes({ { OrderType::Buy, "buy" }, { OrderType::Sell, "sell" } }),
      orderCanceledReasons({
          { OrderCanceledReason::CanceledByCmd, "CanceledByCmd" },
          { OrderCanceledReason::CanceledByIncAskPrice, "CanceledByIncAskPrice" },
          { OrderCanceledReason::CanceledByDecAskPrice, "CanceledByDecAskPrice" },
          { OrderCanceledReason::CanceledByIncBidPrice, "CanceledByIncBidPrice" },
          { OrderCanceledReason::CanceledByDecBidPrice, "CanceledByDecBidPrice" },
          { OrderCanceledReason::CanceledByFilledSellOrder, "CanceledByFilledSellOrder" },
          { OrderCanceledReason::CanceledByOrder2ndTimeOut, "CanceledByOrder2ndTimeOut" } }),
      coinTypes({ { CoinType::Btc, "btc" }, { CoinType::Ltc, "ltc" }, { CoinType::Eth, "eth" } }),
      chargeTypes({ { ChargeType::FixedFee, "FixedFee" }, { ChargeType::FloatingRate, "FloatingRate" } }),
      robotCfgStates({ { RobotCfgState::Enabled, "enabled" }, { RobotCfgState::Disabled, "disabled" } }),
      serviceStates({ { ServiceState::ServiceRunning, "running" }, { ServiceState::ServiceStopped, "stopted" } }),
      opms({ { HrNs::OderPricingMode::AskBidPrice, "AskBidPrice" },
             { HrNs::OderPricingMode::AskLastPrice, "AskLastPrice" },
             { HrNs::OderPricingMode::LastBidPrice, "LastBidPrice" } })
{}

EnumHelper::~EnumHelper()
{}

const char* EnumHelper::GetName(ApiType enumValue) const
{
    auto iter = find_if(apiTypes.begin(), apiTypes.end(), bind(CmpEnum<ApiType>, enumValue, _1));
    return iter->second;
}

const char* EnumHelper::GetName(OrderType enumValue) const
{
    auto iter = find_if(orderTypes.begin(), orderTypes.end(), bind(CmpEnum<OrderType>, enumValue, _1));
    return iter->second;
}

const char* EnumHelper::GetName(OrderCanceledReason enumValue) const
{
    auto iter = find_if(orderCanceledReasons.begin(), orderCanceledReasons.end(), bind(CmpEnum<OrderCanceledReason>, enumValue, _1));
    return iter->second;
}

const char* EnumHelper::GetName(CoinType enumValue) const
{
    auto iter = find_if(coinTypes.begin(), coinTypes.end(), bind(CmpEnum<CoinType>, enumValue, _1));
    return iter->second;
}

const char* EnumHelper::GetName(ChargeType enumValue) const
{
    auto iter = find_if(chargeTypes.begin(), chargeTypes.end(), bind(CmpEnum<ChargeType>, enumValue, _1));
    return iter->second;
}

const char* EnumHelper::GetName(RobotCfgState enumValue) const
{
    auto iter = find_if(robotCfgStates.begin(), robotCfgStates.end(), bind(CmpEnum<RobotCfgState>, enumValue, _1));
    return iter->second;
}

const char* EnumHelper::GetName(ServiceState enumValue) const
{
    auto iter = find_if(serviceStates.begin(), serviceStates.end(), bind(CmpEnum<ServiceState>, enumValue, _1));
    return iter->second;
}

const char* EnumHelper::GetName(HrNs::OderPricingMode enumValue) const
{
    auto iter = find_if(opms.begin(), opms.end(), bind(CmpEnum<HrNs::OderPricingMode>, enumValue, _1));
    return iter->second;
}

ApiType EnumHelper::GetApiType(const char *name) const
{
    auto iter = find_if(apiTypes.begin(), apiTypes.end(), bind(CmpName<ApiType>, name, _1));
    return iter->first;
}

OrderType EnumHelper::GetOrderType(const char *name) const
{
    auto iter = find_if(orderTypes.begin(), orderTypes.end(), bind(CmpName<OrderType>, name, _1));
    return iter->first;
}

OrderCanceledReason EnumHelper::GetOrderCanceledReason(const char *name) const
{
    auto iter = find_if(orderCanceledReasons.begin(), orderCanceledReasons.end(), bind(CmpName<OrderCanceledReason>, name, _1));
    return iter->first;
}

CoinType EnumHelper::GetCoinType(const char *name) const
{
    auto iter = find_if(coinTypes.begin(), coinTypes.end(), bind(CmpName<CoinType>, name, _1));
    return iter->first;
}

ChargeType EnumHelper::GetChargeType(const char *name) const
{
    auto iter = find_if(chargeTypes.begin(), chargeTypes.end(), bind(CmpName<ChargeType>, name, _1));
    return iter->first;
}

RobotCfgState EnumHelper::GetRobotCfgState(const char *name) const
{
    auto iter = find_if(robotCfgStates.begin(), robotCfgStates.end(), bind(CmpName<RobotCfgState>, name, _1));
    return iter->first;
}

ServiceState EnumHelper::GetServiceState(const char *name) const
{
    auto iter = find_if(serviceStates.begin(), serviceStates.end(), bind(CmpName<ServiceState>, name, _1));
    return iter->first;
}

HrNs::OderPricingMode EnumHelper::GetOderPricingMode(const char *name) const
{
    auto iter = find_if(opms.begin(), opms.end(), bind(CmpName<HrNs::OderPricingMode>, name, _1));
    return iter->first;
}
