call %VS90COMNTOOLS%\vsvars32.bat

set cfg=Release

devenv /clean "%cfg%|x64" vc9\cds.sln
devenv vc9\cds.sln /build "%cfg%|x64" 2>&1 |tee vc9_x64.log

devenv /clean "%cfg%|Win32" vc9\cds.sln
devenv vc9\cds.sln /build "%cfg%|Win32" 2>&1 |tee vc9_win32.log