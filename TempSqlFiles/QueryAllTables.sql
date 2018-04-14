-- use BtcTradeRobotUnitTest;
-- use BtcTradeRobot;
-- set names latin1;
-- set names utf8;

select * from `Market` order by Id Desc limit 10;
select * from `Order`;
select * from `HedgeRobotOrder`;
select * from `HedgeRobotOrderView`;

select * from `Account`;
select * from `HedgerobotAccount`;
select * from `HedgerobotAccountgroup`;
select * from `HedgeRobotAccountGroupHorzTrial`;
select * from `HedgeRobotAccountGroupNormal`;
select * from `HedgeRobotAccountGroupOpmTrial`;
select * from `HrAccountView`;
select * from `HrRobotAccountView`;

select * from `Robot`;
select * from `HedgeRobot`;
select * from `HedgeRobotView`;

select * from `Platform`;
select * from `PlatformApiRule`;
select * from `PlatformApiRuleView`;
select * from `PlatformCoinRule`;
select * from `PlatformCoinRuleView`;
select * from `HedgeRobotPlatformPair`;
select * from `HedgeRobotPlatformPairView`;

-- delete from `PlatformCoinRule`;
-- delete from `PlatformApiRule`;
-- delete from `Platform`;

select * from `TableId`;

/******************* unit test specific table *******************/
select * from `Ut1v1SharedPkt`;
select * from `Ut1v1LazySharedPkt`;
select * from `Ut1v1WeakFkt`;

select * from `Ut1v1SharedFkt`;
select * from `Ut1v1WeakPkt`;

select * from `Ut1vNSharedPkt`;
select * from `Ut1vNWeakFkt1`;

select * from `Ut1vNLazySharedPkt`;
select * from `Ut1vNWeakFkt2`;

select * from `Ut1vNWeakPkt1`;
select * from `Ut1vNSharedFkt`;

select * from `Ut1vNWeakPkt2`;
select * from `Ut1vNLazySharedFkt`;