@echo off
cd ..\..
mkdir distrib
cd distrib

mkdir package
cd package

mkdir bin
mkdir lib
mkdir include\libxmldiff
REM Copy include
copy ..\..\src\*.h include\libxmldiff\
del /q include\libxmldiff\xmldiff.h
REM Librairies
copy ..\..\bin\*.lib lib
copy ..\..\bin\*.a lib
copy ..\..\bin\*.def lib
copy ..\..\build\dev-cpp\libxmldiff-dev.DevPackage .
copy ..\..\build\dev-cpp\License.txt .
copy ..\..\build\dev-cpp\Readme.txt .

zip -R -9 ..\libxmldiff_dev_devcpp.zip *.*
tar cjvf ..\libxmldiff-dev.DevPak *

cd ..
del /q /s package
rmdir /q /s package

cd ..\build\win32
