@echo off
cd ..\..
mkdir distrib
cd distrib

mkdir package
cd package

mkdir src
mkdir build\vc
mkdir doc
REM Copy include
copy ..\..\src\*.* src
REM Librairies
copy ..\..\build\vc\*.bat build\vc
copy ..\..\build\vc\*.dsp build\vc
copy ..\..\build\vc\*.dsw build\vc
REM Doc
copy ..\..\doc\*.* doc

zip -R -9 ..\libxmldiff_src.zip *.*

cd ..
del /q /s package
rmdir /q /s package

cd ..\build\win32
