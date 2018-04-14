cd /D %~dp0%

:: compile Entities.h
"%ODB_HOME%\bin\odb.exe" -I ..\..\Codes --std c++11 --database mysql --generate-query --generate-schema  --generate-session --cxx-suffix .cpp --hxx-suffix .h --sql-suffix -odb.sql UtEntities.h

pause
