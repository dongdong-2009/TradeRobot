set version=0
@svn update
@for /f %%i in ('svn info --show-item revision') do set version="%%i"
@echo #define ExeVersion %version% > Version.h