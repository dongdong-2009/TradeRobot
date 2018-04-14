cd /D %~dp0%

"%MYSQL_SERVER_HOME%/bin/mysql.exe" -u "root" --password="000000" --database "BtcTradeRobot" < RestoreDatabase.sql