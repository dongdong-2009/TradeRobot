--
-- Table structure for table `debitandcreditbaseinfor`
--

DROP TABLE IF EXISTS `debitandcreditbaseinfor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `debitandcreditbaseinfor` (
  `Id` int(10) unsigned NOT NULL,
  `DateTime` bigint(20) NOT NULL,
  `LendableMoney` double NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Table structure for table `debitandcreditdetailinfor`
--

DROP TABLE IF EXISTS `debitandcreditdetailinfor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `debitandcreditdetailinfor` (
  `Id` int(10) unsigned NOT NULL,
  `CoinType` enum('Btc','Dog','Eth','Etc','Ltc','Ybc') NOT NULL,
  `LendableCoinNumber` double NOT NULL,
  `Price` double NOT NULL,
  `DebitAndCreditBaseInforId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
