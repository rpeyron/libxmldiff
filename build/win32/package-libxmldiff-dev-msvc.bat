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
copy ..\..\build\vc\Release\*.lib lib
copy ..\..\build\vc\Release\*.exp lib


zip -R -9 ..\libxmldiff_dev_msvc.zip *.*

cd ..
del /q /s package
rmdir /q /s package

cd ..\build\win32
