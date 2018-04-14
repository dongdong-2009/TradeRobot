-- use BtcTradeRobotUnitTest;
-- use BtcTradeRobot;
-- set names latin1;
-- set names utf8;

/* Platform View */
drop view if exists PlatformCoinRuleView;
create view PlatformCoinRuleView as
select p.id, p.Name, p.CashWithdrawalFeeRate,
    r.CoinType, r.MinTradingUnit, r.TransactionFeeRate, 
    r.CoinWithdrawalChargeType, r.CoinWithdrawalPara, r.MinPriceSize
from Platform as p
    inner join PlatformCoinRule as r
    on p.id = r.PlatformId;
    
drop view if exists PlatformApiRuleView;
create view PlatformApiRuleView as
select p.id, p.Name, p.MinMarketQueryInterval, p.MinOrderQueryInterval, 
    p.MinTradeInterval, p.CashWithdrawalFeeRate,
    r.ApiType, r.Url, r.CurlTimeout
from Platform as p
    inner join PlatformApiRule as r
    on p.id = r.PlatformId;

/* HedgeRobot View */
drop view if exists HedgeRobotView;
create view HedgeRobotView as
select r.Id as RobotId, "HedgeRobot" as RobotName, r.RobotCfgState,
    hr.MinTradingUnit, hr.StopLossAdjustSize, hr.Order2ndTimeOut, hr.StopLossOrderTimeOut, 
    hr.CoinType, hrag.Id as HrAccountGroupId
from Robot as r
    inner join HedgeRobot as hr
    inner join HedgeRobotAccountGroup as hrag
    on r.Id = hr.Id and hr.Id = hrag.HedgeRobotId;
    
/* HedgeRobotPlatformPair View */
drop view if exists HedgeRobotPlatformPairView;
create view HedgeRobotPlatformPairView as
select hr.RobotId, hr.RobotName, hr.CoinType, 
    p1.Name as PlatformA, p1.TransactionFeeRate, p1.CoinWithdrawalChargeType, p1.CoinWithdrawalPara,
    p2.Name as PlatformB, 
    hrpp.Margin, hrpp.MarginAdjustSize, hrpp.OderPricingMode
from HedgeRobotView as hr    
    inner join HedgeRobotPlatformPair as hrpp
    inner join PlatformCoinRuleView as p1
    inner join PlatformCoinRuleView as p2
    on hr.RobotId = hrpp.HedgeRobotId 
        and hrpp.PlatformIdA = p1.Id and hr.CoinType = p1.CoinType
        and hrpp.PlatformIdB = p2.Id and hr.CoinType = p2.CoinType
order by hrpp.PlatformIdA;

    
/* hr Account View */
drop view if exists HrAccountView;
create view HrAccountView as
select a.Id as AccountId, a.UserName, a.InvestedMoney, a.InvestedCoinNumber, 
       hra.HedgeRobotAccountGroupId as HrAccountGroupId, 
       p.Name as PlatformName
from Account as a
    inner join HedgeRobotAccount as hra
    inner join Platform as p
    on a.Id = hra.Id
    and a.PlatformId = p.Id
order by AccountId;
    
/* hr Robot Account View */
drop view if exists HrRobotAccountView;
create view HrRobotAccountView as
select r.RobotId, r.RobotName, r.CoinType, r.HrAccountGroupId, 
    a.AccountId, a.UserName, a.InvestedMoney, a.InvestedCoinNumber, 
    a.PlatformName
from HedgeRobotView as r
    inner join HrAccountView as a
    on r.HrAccountGroupId = a.HrAccountGroupId;

drop view if exists HedgeRobotOrderView;
create view HedgeRobotOrderView as 
select o.Id as OrderId, hro.ParentOrderId, o.OrderType, o.LimitedPrice, o.ConcludedPrice, 
    o.CoinNumber, o.FilledCoinNumber,
    FROM_UNIXTIME(o.CreatingTime,'%Y-%m-%d %H:%i:%s') as CreatingTime, 
    FROM_UNIXTIME(o.ClosingTime, '%Y-%m-%d %H:%i:%s') as ClosingTime, o.CanceledReason,
    FROM_UNIXTIME(hro.PriceAdjustScheduleTime,'%Y-%m-%d %H:%i:%s') as PriceAdjustScheduleTime,      
    hra.UserName, hra.PlatformName
from `Order` o
    inner join `HedgeRobotOrder` as hro
    inner join `HrRobotAccountView` as hra
    on o.id = hro.id
    and o.AccountId = hra.AccountId
order by o.Id;
  
