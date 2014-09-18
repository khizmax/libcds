rm -fr doc
md doc
set DOXYPRJ_ROOT=.
%DOXYGEN_PATH%\bin\doxygen doxygen\cds.doxy > doxygen\doxygen.log 2>&1
cp -f doxygen/index.html doc/index.html

