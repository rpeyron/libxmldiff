@echo off
cd ..\..
mkdir distrib
cd distrib

mkdir package
cd package

copy ..\..\bin\*.dll .
copy ..\..\bin\*.exe .

zip -R -9 ..\xmldiff_bin.zip *.*

cd ..
del /q /s package
rmdir /q /s package

cd ..\build\win32
