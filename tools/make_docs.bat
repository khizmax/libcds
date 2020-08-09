set DOXYPRJ_ROOT=.
cd ..
del /F /Q doc\*
"%DOXYGEN_PATH%\bin\doxygen" doxygen\cds.doxy > tools\doxygen.log 2>&1

copy /Y doxygen\index.html doc\index.html
cd tools
