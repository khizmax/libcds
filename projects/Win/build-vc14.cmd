call "%VS140COMNTOOLS%"\vsvars32.bat
call build-msbuild.cmd vc14 Release /tv:14.0 /m:2
