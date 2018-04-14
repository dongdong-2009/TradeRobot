use BtcTradeRobot;

insert into `Robot` 
    select * from `BtcTradeRobotBackup`.`Robot`;
    
insert into `Account` 
    select * from `BtcTradeRobotBackup`.`Account`;
    
insert into `AccountGroup` 
    select * from `BtcTradeRobotBackup`.`AccountGroup`;
    
insert into `Order` 
    select * from `BtcTradeRobotBackup`.`Order`;
    
insert into `Platform` 
    select * from `BtcTradeRobotBackup`.`Platform`;
        
insert into `Market` 
    select * from `BtcTradeRobotBackup`.`Market`;
    
insert into `RobotException` 
    select * from `BtcTradeRobotBackup`.`RobotException`;
    
insert into `TableIdx` 
    select * from `BtcTradeRobotBackup`.`TableIdx`;
    
insert into `Nonce` 
    select * from `BtcTradeRobotBackup`.`Nonce`;