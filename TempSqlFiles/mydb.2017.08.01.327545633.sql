-- MySQL dump 10.13  Distrib 5.5.25, for Win32 (x86)
--
-- Host: localhost    Database: BtcTradeRobot
-- ------------------------------------------------------
-- Server version	5.5.25

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Account`
--

DROP TABLE IF EXISTS `Account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Account` (
  `Id` int(10) unsigned NOT NULL,
  `typeid` varchar(255) NOT NULL,
  `UserName` text NOT NULL,
  `PublicKey` text NOT NULL,
  `PrivateKey` text NOT NULL,
  `InvestedMoney` double NOT NULL,
  `InvestedCoinNumber` double NOT NULL,
  `Description` text NOT NULL,
  `PlatformId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Account`
--

LOCK TABLES `Account` WRITE;
/*!40000 ALTER TABLE `Account` DISABLE KEYS */;
INSERT INTO `Account` VALUES (1,'HrAccountEntity','327545633@qq.com','dbc11fbb-e473-4e85-bb98-44c3369e930d','51655F8D5528CEAF612241753B77DEE2',1072.19065,0.049988,'',1),(2,'HrAccountEntity','327545633@qq.com','91gdn-c45ww-wugqb-wt2rs-56359-bkkug-p4g6h','8GMjx-Ecj)@-K@RPM-A[YG%-~/CNT-*p]xB-pF){X',64.39,0.594,'',2);
/*!40000 ALTER TABLE `Account` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `HedgeRobot`
--

DROP TABLE IF EXISTS `HedgeRobot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `HedgeRobot` (
  `Id` int(10) unsigned NOT NULL,
  `CoinType` enum('Btc','Dog','Eth','Etc','Ltc','Ybc') NOT NULL,
  `HorzMarginAdjustInvl` bigint(20) NOT NULL,
  `VertMarginAdjustInvl` bigint(20) NOT NULL,
  `OpmAdjustInvl` bigint(20) NOT NULL,
  `MinTradingUnit` double NOT NULL,
  `StopLossAdjustSize` double NOT NULL,
  `Order2ndTimeOut` bigint(20) NOT NULL,
  `StopLossOrderTimeOut` bigint(20) NOT NULL,
  PRIMARY KEY (`Id`),
  CONSTRAINT `HedgeRobot_Id_fk` FOREIGN KEY (`Id`) REFERENCES `robot` (`Id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `HedgeRobot`
--

LOCK TABLES `HedgeRobot` WRITE;
/*!40000 ALTER TABLE `HedgeRobot` DISABLE KEYS */;
INSERT INTO `HedgeRobot` VALUES (1,'Eth',86400,86400,86400,0.05,0.001,120,172800);
/*!40000 ALTER TABLE `HedgeRobot` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `HedgeRobotAccount`
--

DROP TABLE IF EXISTS `HedgeRobotAccount`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `HedgeRobotAccount` (
  `Id` int(10) unsigned NOT NULL,
  `HedgeRobotAccountGroupId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`),
  CONSTRAINT `HedgeRobotAccount_Id_fk` FOREIGN KEY (`Id`) REFERENCES `account` (`Id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `HedgeRobotAccount`
--

LOCK TABLES `HedgeRobotAccount` WRITE;
/*!40000 ALTER TABLE `HedgeRobotAccount` DISABLE KEYS */;
INSERT INTO `HedgeRobotAccount` VALUES (1,1),(2,1);
/*!40000 ALTER TABLE `HedgeRobotAccount` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `HedgeRobotAccountGroup`
--

DROP TABLE IF EXISTS `HedgeRobotAccountGroup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `HedgeRobotAccountGroup` (
  `Id` int(10) unsigned NOT NULL,
  `typeid` varchar(255) NOT NULL,
  `Description` text NOT NULL,
  `HedgeRobotId` int(10) unsigned NOT NULL,
  `HrAccount0Id` int(10) unsigned NOT NULL,
  `HrAccount1Id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `HedgeRobotAccountGroup`
--

LOCK TABLES `HedgeRobotAccountGroup` WRITE;
/*!40000 ALTER TABLE `HedgeRobotAccountGroup` DISABLE KEYS */;
INSERT INTO `HedgeRobotAccountGroup` VALUES (1,'HrAccGrpNormalEntity','',1,1,2);
/*!40000 ALTER TABLE `HedgeRobotAccountGroup` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `HedgeRobotAccountGroupHorzTrial`
--

DROP TABLE IF EXISTS `HedgeRobotAccountGroupHorzTrial`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `HedgeRobotAccountGroupHorzTrial` (
  `Id` int(10) unsigned NOT NULL,
  `AccountGroupNormalId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`),
  CONSTRAINT `HedgeRobotAccountGroupHorzTrial_Id_fk` FOREIGN KEY (`Id`) REFERENCES `hedgerobotaccountgroup` (`Id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `HedgeRobotAccountGroupHorzTrial`
--

LOCK TABLES `HedgeRobotAccountGroupHorzTrial` WRITE;
/*!40000 ALTER TABLE `HedgeRobotAccountGroupHorzTrial` DISABLE KEYS */;
/*!40000 ALTER TABLE `HedgeRobotAccountGroupHorzTrial` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `HedgeRobotAccountGroupNormal`
--

DROP TABLE IF EXISTS `HedgeRobotAccountGroupNormal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `HedgeRobotAccountGroupNormal` (
  `Id` int(10) unsigned NOT NULL,
  `AccountGroupMln` int(11) NOT NULL,
  PRIMARY KEY (`Id`),
  CONSTRAINT `HedgeRobotAccountGroupNormal_Id_fk` FOREIGN KEY (`Id`) REFERENCES `hedgerobotaccountgroup` (`Id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `HedgeRobotAccountGroupNormal`
--

LOCK TABLES `HedgeRobotAccountGroupNormal` WRITE;
/*!40000 ALTER TABLE `HedgeRobotAccountGroupNormal` DISABLE KEYS */;
INSERT INTO `HedgeRobotAccountGroupNormal` VALUES (1,0);
/*!40000 ALTER TABLE `HedgeRobotAccountGroupNormal` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `HedgeRobotAccountGroupOpmTrial`
--

DROP TABLE IF EXISTS `HedgeRobotAccountGroupOpmTrial`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `HedgeRobotAccountGroupOpmTrial` (
  `Id` int(10) unsigned NOT NULL,
  `AccountGroupNormalId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`),
  CONSTRAINT `HedgeRobotAccountGroupOpmTrial_Id_fk` FOREIGN KEY (`Id`) REFERENCES `hedgerobotaccountgroup` (`Id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `HedgeRobotAccountGroupOpmTrial`
--

LOCK TABLES `HedgeRobotAccountGroupOpmTrial` WRITE;
/*!40000 ALTER TABLE `HedgeRobotAccountGroupOpmTrial` DISABLE KEYS */;
/*!40000 ALTER TABLE `HedgeRobotAccountGroupOpmTrial` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `HedgeRobotOrder`
--

DROP TABLE IF EXISTS `HedgeRobotOrder`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `HedgeRobotOrder` (
  `Id` int(10) unsigned NOT NULL,
  `PriceAdjustScheduleTime` bigint(20) DEFAULT NULL,
  `ParentOrderId` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`Id`),
  CONSTRAINT `HedgeRobotOrder_Id_fk` FOREIGN KEY (`Id`) REFERENCES `order` (`Id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `HedgeRobotOrder`
--

LOCK TABLES `HedgeRobotOrder` WRITE;
/*!40000 ALTER TABLE `HedgeRobotOrder` DISABLE KEYS */;
INSERT INTO `HedgeRobotOrder` VALUES (4,NULL,NULL),(5,1501594799,NULL),(8,NULL,NULL),(9,1501594811,NULL),(10,NULL,NULL),(11,1501594814,NULL),(17,NULL,NULL),(18,1501594863,NULL),(19,1501767666,18),(24,NULL,NULL),(25,1501595218,NULL),(28,NULL,NULL),(29,1501595399,NULL),(30,NULL,NULL),(31,1501595409,NULL),(34,NULL,NULL),(35,1501597608,NULL),(41,NULL,NULL),(42,1501602262,NULL),(43,1501775063,42);
/*!40000 ALTER TABLE `HedgeRobotOrder` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary table structure for view `HedgeRobotOrderView`
--

DROP TABLE IF EXISTS `HedgeRobotOrderView`;
/*!50001 DROP VIEW IF EXISTS `HedgeRobotOrderView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `HedgeRobotOrderView` (
  `OrderId` int(10) unsigned,
  `ParentOrderId` int(10) unsigned,
  `OrderType` enum('Buy','Sell'),
  `LimitedPrice` double,
  `ConcludedPrice` double,
  `CoinNumber` double,
  `FilledCoinNumber` double,
  `CreatingTime` varchar(24),
  `ClosingTime` varchar(24),
  `CanceledReason` enum('CanceledByCmd','CanceledByIncAskPrice','CanceledByDecAskPrice','CanceledByIncBidPrice','CanceledByDecBidPrice','CanceledByFilledSellOrder','CanceledByOrder2ndTimeOut'),
  `PriceAdjustScheduleTime` varchar(24),
  `UserName` text,
  `PlatformName` text
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `HedgeRobotPlatformPair`
--

DROP TABLE IF EXISTS `HedgeRobotPlatformPair`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `HedgeRobotPlatformPair` (
  `Id` int(10) unsigned NOT NULL,
  `PlatformIdA` int(10) unsigned NOT NULL,
  `PlatformIdB` int(10) unsigned NOT NULL,
  `Margin` double NOT NULL,
  `MarginAdjustSize` double NOT NULL,
  `OderPricingMode` enum('AskAskPrice','AskBidPrice','AskLastPrice','BidAskPrice','BidBidPrice','BidLastPrice','LastAskPrice','LastBidPrice','LastLastPrice') NOT NULL,
  `OderPricingModeFlag` int(10) unsigned NOT NULL,
  `HedgeRobotId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `HedgeRobotPlatformPair`
--

LOCK TABLES `HedgeRobotPlatformPair` WRITE;
/*!40000 ALTER TABLE `HedgeRobotPlatformPair` DISABLE KEYS */;
INSERT INTO `HedgeRobotPlatformPair` VALUES (1,1,2,0.002,0.001,'BidBidPrice',0,1),(2,2,1,0.002,0.001,'AskAskPrice',0,1);
/*!40000 ALTER TABLE `HedgeRobotPlatformPair` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary table structure for view `HedgeRobotPlatformPairView`
--

DROP TABLE IF EXISTS `HedgeRobotPlatformPairView`;
/*!50001 DROP VIEW IF EXISTS `HedgeRobotPlatformPairView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `HedgeRobotPlatformPairView` (
  `RobotId` int(10) unsigned,
  `RobotName` varchar(10),
  `CoinType` enum('Btc','Dog','Eth','Etc','Ltc','Ybc'),
  `PlatformA` text,
  `TransactionFeeRate` double,
  `CoinWithdrawalChargeType` enum('FixedFee','FloatingRate'),
  `CoinWithdrawalPara` double,
  `PlatformB` text,
  `Margin` double,
  `MarginAdjustSize` double,
  `OderPricingMode` enum('AskAskPrice','AskBidPrice','AskLastPrice','BidAskPrice','BidBidPrice','BidLastPrice','LastAskPrice','LastBidPrice','LastLastPrice')
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `HedgeRobotView`
--

DROP TABLE IF EXISTS `HedgeRobotView`;
/*!50001 DROP VIEW IF EXISTS `HedgeRobotView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `HedgeRobotView` (
  `RobotId` int(10) unsigned,
  `RobotName` varchar(10),
  `RobotCfgState` enum('Enabled','Disabled'),
  `MinTradingUnit` double,
  `StopLossAdjustSize` double,
  `Order2ndTimeOut` bigint(20),
  `StopLossOrderTimeOut` bigint(20),
  `CoinType` enum('Btc','Dog','Eth','Etc','Ltc','Ybc'),
  `HrAccountGroupId` int(10) unsigned
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `HrAccountView`
--

DROP TABLE IF EXISTS `HrAccountView`;
/*!50001 DROP VIEW IF EXISTS `HrAccountView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `HrAccountView` (
  `AccountId` int(10) unsigned,
  `UserName` text,
  `InvestedMoney` double,
  `InvestedCoinNumber` double,
  `HrAccountGroupId` int(10) unsigned,
  `PlatformName` text
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `HrRobotAccountView`
--

DROP TABLE IF EXISTS `HrRobotAccountView`;
/*!50001 DROP VIEW IF EXISTS `HrRobotAccountView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `HrRobotAccountView` (
  `RobotId` int(10) unsigned,
  `RobotName` varchar(10),
  `CoinType` enum('Btc','Dog','Eth','Etc','Ltc','Ybc'),
  `HrAccountGroupId` int(10) unsigned,
  `AccountId` int(10) unsigned,
  `UserName` text,
  `InvestedMoney` double,
  `InvestedCoinNumber` double,
  `PlatformName` text
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `Market`
--

DROP TABLE IF EXISTS `Market`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Market` (
  `Id` int(10) unsigned NOT NULL,
  `CoinType` enum('Btc','Dog','Eth','Etc','Ltc','Ybc') NOT NULL,
  `BidPrice` double NOT NULL,
  `AskPrice` double NOT NULL,
  `LastPrice` double NOT NULL,
  `Time` bigint(20) NOT NULL,
  `PlatformId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Market`
--

LOCK TABLES `Market` WRITE;
/*!40000 ALTER TABLE `Market` DISABLE KEYS */;
/*!40000 ALTER TABLE `Market` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Order`
--

DROP TABLE IF EXISTS `Order`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Order` (
  `Id` int(10) unsigned NOT NULL,
  `typeid` varchar(255) NOT NULL,
  `OrderType` enum('Buy','Sell') NOT NULL,
  `PlatformOrderId` bigint(20) unsigned NOT NULL,
  `LimitedPrice` double NOT NULL,
  `ConcludedPrice` double DEFAULT NULL,
  `CoinNumber` double NOT NULL,
  `FilledCoinNumber` double NOT NULL,
  `CreatingTime` bigint(20) NOT NULL,
  `ClosingTime` bigint(20) DEFAULT NULL,
  `CanceledReason` enum('CanceledByCmd','CanceledByIncAskPrice','CanceledByDecAskPrice','CanceledByIncBidPrice','CanceledByDecBidPrice','CanceledByFilledSellOrder','CanceledByOrder2ndTimeOut') DEFAULT NULL,
  `AccountId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Order`
--

LOCK TABLES `Order` WRITE;
/*!40000 ALTER TABLE `Order` DISABLE KEYS */;
INSERT INTO `Order` VALUES (4,'HrOrderEntity','Buy',38907316,1468.29,1468.29,0.05,0.05,1501594677,1501594679,NULL,1),(5,'HrOrderEntity','Sell',376439882,1470.32,1470.69,0.05,0.05,1501594679,1501594681,NULL,2),(8,'HrOrderEntity','Buy',38907505,1467.48,1467.48,0.05,0.05,1501594688,1501594691,NULL,1),(9,'HrOrderEntity','Sell',376439924,1470.39,1470.52,0.05,0.05,1501594691,1501594692,NULL,2),(10,'HrOrderEntity','Buy',38907599,1467.95,1467.95,0.05,0.05,1501594692,1501594693,NULL,1),(11,'HrOrderEntity','Sell',376439939,1470.32,1470.32,0.05,0.05,1501594694,1501594699,NULL,2),(17,'HrOrderEntity','Buy',38908365,1461.73,1461.73,0.05,0.05,1501594740,1501594742,NULL,1),(18,'HrOrderEntity','Sell',376440122,1464.7,NULL,0.05,0,1501594743,1501594866,'CanceledByOrder2ndTimeOut',2),(19,'HrOrderEntity','Sell',376440697,1463.24,1463.24,0.05,0.05,1501594866,1501597485,NULL,2),(24,'HrOrderEntity','Sell',38915505,1453.63,1453.63,0.05,0.05,1501595096,1501595097,NULL,1),(25,'HrOrderEntity','Buy',376441976,1448.3,1448,0.05,0.05,1501595098,1501595101,NULL,2),(28,'HrOrderEntity','Sell',38918444,1455.41,1455.41,0.05,0.05,1501595275,1501595277,NULL,1),(29,'HrOrderEntity','Buy',376442726,1450.25,1450,0.05,0.05,1501595279,1501595282,NULL,2),(30,'HrOrderEntity','Sell',38918672,1457.72,1457.72,0.05,0.05,1501595286,1501595289,NULL,1),(31,'HrOrderEntity','Buy',376442770,1450.3,1450,0.05,0.05,1501595289,1501595290,NULL,2),(34,'HrOrderEntity','Sell',38950667,1470.48,1470.48,0.05,0.05,1501597485,1501597487,NULL,1),(35,'HrOrderEntity','Buy',376448642,1463.48,1463.18,0.05,0.05,1501597488,1501597490,NULL,2),(41,'HrOrderEntity','Buy',39007390,1459.89,1459.89,0.05,0.05,1501602138,1501602141,NULL,1),(42,'HrOrderEntity','Sell',376457613,1461.7,NULL,0.05,0,1501602142,1501602263,'CanceledByOrder2ndTimeOut',2),(43,'HrOrderEntity','Sell',376457808,1460.24,1460.24,0.05,0.05,1501602263,1501602270,NULL,2);
/*!40000 ALTER TABLE `Order` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Platform`
--

DROP TABLE IF EXISTS `Platform`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Platform` (
  `Id` int(10) unsigned NOT NULL,
  `Name` text NOT NULL,
  `MinMarketQueryInterval` bigint(20) NOT NULL,
  `MinOrderQueryInterval` bigint(20) NOT NULL,
  `MinTradeInterval` bigint(20) NOT NULL,
  `CashWithdrawalFeeRate` double NOT NULL,
  `ApiAverageResponseTime` bigint(20) NOT NULL,
  `Description` text NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Platform`
--

LOCK TABLES `Platform` WRITE;
/*!40000 ALTER TABLE `Platform` DISABLE KEYS */;
INSERT INTO `Platform` VALUES (1,'okcoin.cn',100,100,200,0.005,200,''),(2,'btctrade.com',200,200,500,0.004,250,'');
/*!40000 ALTER TABLE `Platform` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PlatformApiRule`
--

DROP TABLE IF EXISTS `PlatformApiRule`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `PlatformApiRule` (
  `Id` int(10) unsigned NOT NULL,
  `PlatformId` int(10) unsigned NOT NULL,
  `ApiType` enum('Buy','Sell','CancelOrder','FetchOrder','FetchOrders','FetchBalance') NOT NULL,
  `Url` text NOT NULL,
  `CurlTimeout` bigint(20) NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `PlatformApiRule`
--

LOCK TABLES `PlatformApiRule` WRITE;
/*!40000 ALTER TABLE `PlatformApiRule` DISABLE KEYS */;
INSERT INTO `PlatformApiRule` VALUES (1,1,'Buy','https://www.okcoin.cn/api/v1/trade.do',40),(2,1,'Sell','https://www.okcoin.cn/api/v1/trade.do',40),(3,1,'CancelOrder','https://www.okcoin.cn/api/v1/cancel_order.do',40),(4,1,'FetchOrder','https://www.okcoin.cn/api/v1/order_info.do',10),(5,1,'FetchOrders','https://www.okcoin.cn/api/v1/order_history.do',10),(6,1,'FetchBalance','https://www.okcoin.cn/api/v1/userinfo.do',10),(7,2,'Buy','https://api.btctrade.com/api/buy',40),(8,2,'Sell','https://api.btctrade.com/api/sell',40),(9,2,'CancelOrder','https://api.btctrade.com/api/cancel_order',40),(10,2,'FetchOrder','https://api.btctrade.com/api/fetch_order',10),(11,2,'FetchOrders','https://api.btctrade.com/api/orders',10),(12,2,'FetchBalance','https://api.btctrade.com/api/balance',10);
/*!40000 ALTER TABLE `PlatformApiRule` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary table structure for view `PlatformApiRuleView`
--

DROP TABLE IF EXISTS `PlatformApiRuleView`;
/*!50001 DROP VIEW IF EXISTS `PlatformApiRuleView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `PlatformApiRuleView` (
  `id` int(10) unsigned,
  `Name` text,
  `MinMarketQueryInterval` bigint(20),
  `MinOrderQueryInterval` bigint(20),
  `MinTradeInterval` bigint(20),
  `CashWithdrawalFeeRate` double,
  `ApiType` enum('Buy','Sell','CancelOrder','FetchOrder','FetchOrders','FetchBalance'),
  `Url` text,
  `CurlTimeout` bigint(20)
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `PlatformCoinRule`
--

DROP TABLE IF EXISTS `PlatformCoinRule`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `PlatformCoinRule` (
  `Id` int(10) unsigned NOT NULL,
  `PlatformId` int(10) unsigned NOT NULL,
  `CoinType` enum('Btc','Dog','Eth','Etc','Ltc','Ybc') NOT NULL,
  `MinTradingUnit` double NOT NULL,
  `TransactionFeeRate` double NOT NULL,
  `CoinWithdrawalChargeType` enum('FixedFee','FloatingRate') NOT NULL,
  `CoinWithdrawalPara` double NOT NULL,
  `MinPriceSize` double NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `PlatformCoinRule`
--

LOCK TABLES `PlatformCoinRule` WRITE;
/*!40000 ALTER TABLE `PlatformCoinRule` DISABLE KEYS */;
INSERT INTO `PlatformCoinRule` VALUES (1,1,'Eth',0.01,0.0005,'FloatingRate',0,0.15),(2,2,'Eth',0.01,0.0005,'FixedFee',0.01,0.3);
/*!40000 ALTER TABLE `PlatformCoinRule` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary table structure for view `PlatformCoinRuleView`
--

DROP TABLE IF EXISTS `PlatformCoinRuleView`;
/*!50001 DROP VIEW IF EXISTS `PlatformCoinRuleView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `PlatformCoinRuleView` (
  `id` int(10) unsigned,
  `Name` text,
  `CashWithdrawalFeeRate` double,
  `CoinType` enum('Btc','Dog','Eth','Etc','Ltc','Ybc'),
  `MinTradingUnit` double,
  `TransactionFeeRate` double,
  `CoinWithdrawalChargeType` enum('FixedFee','FloatingRate'),
  `CoinWithdrawalPara` double,
  `MinPriceSize` double
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `Robot`
--

DROP TABLE IF EXISTS `Robot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Robot` (
  `Id` int(10) unsigned NOT NULL,
  `typeid` varchar(255) NOT NULL,
  `RobotCfgState` enum('Enabled','Disabled') NOT NULL,
  `Description` text NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Robot`
--

LOCK TABLES `Robot` WRITE;
/*!40000 ALTER TABLE `Robot` DISABLE KEYS */;
INSERT INTO `Robot` VALUES (1,'HrEntity','Enabled','');
/*!40000 ALTER TABLE `Robot` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `TableId`
--

DROP TABLE IF EXISTS `TableId`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `TableId` (
  `TableName` varchar(255) NOT NULL,
  `TableId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`TableName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `TableId`
--

LOCK TABLES `TableId` WRITE;
/*!40000 ALTER TABLE `TableId` DISABLE KEYS */;
INSERT INTO `TableId` VALUES ('Account',3),('HedgeRobotAccountGroup',2),('HedgeRobotPlatformPair',3),('Market',1),('Order',44),('Platform',3),('PlatformApiRule',13),('PlatformCoinRule',3),('Robot',2);
/*!40000 ALTER TABLE `TableId` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Final view structure for view `HedgeRobotOrderView`
--

/*!50001 DROP TABLE IF EXISTS `HedgeRobotOrderView`*/;
/*!50001 DROP VIEW IF EXISTS `HedgeRobotOrderView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `HedgeRobotOrderView` AS select `o`.`Id` AS `OrderId`,`hro`.`ParentOrderId` AS `ParentOrderId`,`o`.`OrderType` AS `OrderType`,`o`.`LimitedPrice` AS `LimitedPrice`,`o`.`ConcludedPrice` AS `ConcludedPrice`,`o`.`CoinNumber` AS `CoinNumber`,`o`.`FilledCoinNumber` AS `FilledCoinNumber`,date_format(from_unixtime(`o`.`CreatingTime`),'%Y-%m-%d %H:%i:%s') AS `CreatingTime`,date_format(from_unixtime(`o`.`ClosingTime`),'%Y-%m-%d %H:%i:%s') AS `ClosingTime`,`o`.`CanceledReason` AS `CanceledReason`,date_format(from_unixtime(`hro`.`PriceAdjustScheduleTime`),'%Y-%m-%d %H:%i:%s') AS `PriceAdjustScheduleTime`,`hra`.`UserName` AS `UserName`,`hra`.`PlatformName` AS `PlatformName` from ((`Order` `o` join `HedgeRobotOrder` `hro`) join `HrRobotAccountView` `hra` on(((`o`.`Id` = `hro`.`Id`) and (`o`.`AccountId` = `hra`.`AccountId`)))) order by `o`.`Id` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `HedgeRobotPlatformPairView`
--

/*!50001 DROP TABLE IF EXISTS `HedgeRobotPlatformPairView`*/;
/*!50001 DROP VIEW IF EXISTS `HedgeRobotPlatformPairView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `HedgeRobotPlatformPairView` AS select `hr`.`RobotId` AS `RobotId`,`hr`.`RobotName` AS `RobotName`,`hr`.`CoinType` AS `CoinType`,`p1`.`Name` AS `PlatformA`,`p1`.`TransactionFeeRate` AS `TransactionFeeRate`,`p1`.`CoinWithdrawalChargeType` AS `CoinWithdrawalChargeType`,`p1`.`CoinWithdrawalPara` AS `CoinWithdrawalPara`,`p2`.`Name` AS `PlatformB`,`hrpp`.`Margin` AS `Margin`,`hrpp`.`MarginAdjustSize` AS `MarginAdjustSize`,`hrpp`.`OderPricingMode` AS `OderPricingMode` from (((`HedgeRobotView` `hr` join `HedgeRobotPlatformPair` `hrpp`) join `PlatformCoinRuleView` `p1`) join `PlatformCoinRuleView` `p2` on(((`hr`.`RobotId` = `hrpp`.`HedgeRobotId`) and (`hrpp`.`PlatformIdA` = `p1`.`id`) and (`hr`.`CoinType` = `p1`.`CoinType`) and (`hrpp`.`PlatformIdB` = `p2`.`id`) and (`hr`.`CoinType` = `p2`.`CoinType`)))) order by `hrpp`.`PlatformIdA` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `HedgeRobotView`
--

/*!50001 DROP TABLE IF EXISTS `HedgeRobotView`*/;
/*!50001 DROP VIEW IF EXISTS `HedgeRobotView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `HedgeRobotView` AS select `r`.`Id` AS `RobotId`,'HedgeRobot' AS `RobotName`,`r`.`RobotCfgState` AS `RobotCfgState`,`hr`.`MinTradingUnit` AS `MinTradingUnit`,`hr`.`StopLossAdjustSize` AS `StopLossAdjustSize`,`hr`.`Order2ndTimeOut` AS `Order2ndTimeOut`,`hr`.`StopLossOrderTimeOut` AS `StopLossOrderTimeOut`,`hr`.`CoinType` AS `CoinType`,`hrag`.`Id` AS `HrAccountGroupId` from ((`Robot` `r` join `HedgeRobot` `hr`) join `HedgeRobotAccountGroup` `hrag` on(((`r`.`Id` = `hr`.`Id`) and (`hr`.`Id` = `hrag`.`HedgeRobotId`)))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `HrAccountView`
--

/*!50001 DROP TABLE IF EXISTS `HrAccountView`*/;
/*!50001 DROP VIEW IF EXISTS `HrAccountView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `HrAccountView` AS select `a`.`Id` AS `AccountId`,`a`.`UserName` AS `UserName`,`a`.`InvestedMoney` AS `InvestedMoney`,`a`.`InvestedCoinNumber` AS `InvestedCoinNumber`,`hra`.`HedgeRobotAccountGroupId` AS `HrAccountGroupId`,`p`.`Name` AS `PlatformName` from ((`Account` `a` join `HedgeRobotAccount` `hra`) join `Platform` `p` on(((`a`.`Id` = `hra`.`Id`) and (`a`.`PlatformId` = `p`.`Id`)))) order by `a`.`Id` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `HrRobotAccountView`
--

/*!50001 DROP TABLE IF EXISTS `HrRobotAccountView`*/;
/*!50001 DROP VIEW IF EXISTS `HrRobotAccountView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `HrRobotAccountView` AS select `r`.`RobotId` AS `RobotId`,`r`.`RobotName` AS `RobotName`,`r`.`CoinType` AS `CoinType`,`r`.`HrAccountGroupId` AS `HrAccountGroupId`,`a`.`AccountId` AS `AccountId`,`a`.`UserName` AS `UserName`,`a`.`InvestedMoney` AS `InvestedMoney`,`a`.`InvestedCoinNumber` AS `InvestedCoinNumber`,`a`.`PlatformName` AS `PlatformName` from (`HedgeRobotView` `r` join `HrAccountView` `a` on((`r`.`HrAccountGroupId` = `a`.`HrAccountGroupId`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `PlatformApiRuleView`
--

/*!50001 DROP TABLE IF EXISTS `PlatformApiRuleView`*/;
/*!50001 DROP VIEW IF EXISTS `PlatformApiRuleView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `PlatformApiRuleView` AS select `p`.`Id` AS `id`,`p`.`Name` AS `Name`,`p`.`MinMarketQueryInterval` AS `MinMarketQueryInterval`,`p`.`MinOrderQueryInterval` AS `MinOrderQueryInterval`,`p`.`MinTradeInterval` AS `MinTradeInterval`,`p`.`CashWithdrawalFeeRate` AS `CashWithdrawalFeeRate`,`r`.`ApiType` AS `ApiType`,`r`.`Url` AS `Url`,`r`.`CurlTimeout` AS `CurlTimeout` from (`Platform` `p` join `PlatformApiRule` `r` on((`p`.`Id` = `r`.`PlatformId`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `PlatformCoinRuleView`
--

/*!50001 DROP TABLE IF EXISTS `PlatformCoinRuleView`*/;
/*!50001 DROP VIEW IF EXISTS `PlatformCoinRuleView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `PlatformCoinRuleView` AS select `p`.`Id` AS `id`,`p`.`Name` AS `Name`,`p`.`CashWithdrawalFeeRate` AS `CashWithdrawalFeeRate`,`r`.`CoinType` AS `CoinType`,`r`.`MinTradingUnit` AS `MinTradingUnit`,`r`.`TransactionFeeRate` AS `TransactionFeeRate`,`r`.`CoinWithdrawalChargeType` AS `CoinWithdrawalChargeType`,`r`.`CoinWithdrawalPara` AS `CoinWithdrawalPara`,`r`.`MinPriceSize` AS `MinPriceSize` from (`Platform` `p` join `PlatformCoinRule` `r` on((`p`.`Id` = `r`.`PlatformId`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2017-08-01 23:54:07
