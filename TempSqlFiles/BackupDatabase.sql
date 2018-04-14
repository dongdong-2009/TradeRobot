use BtcTradeRobotBackup;

delete from `Nonce`;
delete from `TableIdx`;
delete from `RobotException`;
delete from `Platform`;
delete from `Order`;
delete from `AccountGroup`;
delete from `Account`;
delete from `Robot`;

insert into `Robot` 
    select * from `BtcTradeRobot`.`Robot`;
    
insert into `Account` 
    select * from `BtcTradeRobot`.`Account`;
    
insert into `AccountGroup` 
    select * from `BtcTradeRobot`.`AccountGroup`;
    
insert into `Order` 
    select * from `BtcTradeRobot`.`Order`;
    
insert into `Platform` 
    select * from `BtcTradeRobot`.`Platform`;
        
insert into `Market` 
    select * from `BtcTradeRobot`.`Market`
        where `Idx` > (select max(`Idx`) from `BtcTradeRobotBackup`.`Market`);
    
insert into `RobotException` 
    select * from `BtcTradeRobot`.`RobotException`;
    
insert into `TableIdx` 
    select * from `BtcTradeRobot`.`TableIdx`;
    
insert into `Nonce` 
    select * from `BtcTradeRobot`.`Nonce`;