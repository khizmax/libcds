:: Command line arguments:
:: %1 - Visual C++ version: vc12 (2013), vc14 (2015)
:: %2 - configuration to build (Release, Debug etc)
:: %3,%4,...%9 - [optional] additional MSBuild options

set vcXX=%1
set cfg=%2 
set MSBUILD=msbuild

:make
for %%A in (x64 Win32) do (
  %MSBUILD% %vcXX%\cds.sln /t:Clean /p:Configuration=%cfg% /p:Platform=%%A %3 %4 %5 %6 %7 %8 %9
  %MSBUILD% %vcXX%\cds.sln /t:Build /p:Configuration=%cfg% /p:Platform=%%A /fl /flp:LogFile=%vcXX%_%%A.log;Encoding=UTF-8;Verbosity=normal /v:normal %3 %4 %5 %6 %7 %8 %9
)
