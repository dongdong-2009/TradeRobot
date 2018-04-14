cd /D %~dp0%

"%MYSQL_SERVER_HOME%\bin\mysql" --user="root" --password="000000" --database="BtcTradeRobotUnitTest" < UtEntities-odb.sql

pause