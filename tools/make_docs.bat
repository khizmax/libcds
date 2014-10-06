set DOXYPRJ_ROOT=.
cd ..
rm -fr doc
md doc
"%DOXYGEN_PATH%\bin\doxygen" doxygen\cds.doxy > tools\doxygen.log 2>&1
cp -f doxygen/index.html doc/index.html
cd tools
