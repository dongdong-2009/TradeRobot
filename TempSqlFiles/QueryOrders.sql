-- use BtcTradeRobotUnitTest;
-- use BtcTradeRobot;
-- set names latin1;
-- set names utf8;

select * from `HedgeRobotPlatformPairView`;
select * from `HrRobotAccountView`;

select * from `Market` order by Id Desc limit 10;

select @idx:=@idx+1 as idx, t.* 
from (select @idx:=0) as it, `HedgeRobotOrderView` as t;

select @cost:=sum(ConcludedPrice * FilledCoinNumber) from `HedgeRobotOrderView` 
where OrderType='Buy' and OrderId not in(70, 71, 72);

select @turnover:=sum(ConcludedPrice * FilledCoinNumber * 0.999) from `HedgeRobotOrderView` 
where OrderType='Sell' and OrderId not in(70, 71, 72);

select convert(@turnover, decimal(10,2)) as Turnover, 
       convert(@cost, decimal(10,2)) as Cost, 
       convert(@turnover - @cost, decimal(10,2)) as Profit

/* 
delete from `Market` where Id>0; 
update `TableId` set TableId=1 where TableName in("Market");
select * from `TableId`;

set @maxId:=0;
delete from `HedgeRobotOrder` where Id>=@maxId; 
delete from `Order` where Id>=@maxId; 
delete from `Market` where Id>0; 
update `TableId` set TableId=1 where TableName="Market";
update `TableId` set TableId=@maxId+1 where TableName="Order";
select * from `TableId`;

update `HedgeRobotPlatformPair` set Margin=0.004 where PlatformIdA=1 and PlatformIdB=2;
update `HedgeRobotPlatformPair` set Margin=0.003 where PlatformIdA=2 and PlatformIdB=1;
update Account set InvestedMoney=0.0, InvestedCoinNumber=0.0;
*/



