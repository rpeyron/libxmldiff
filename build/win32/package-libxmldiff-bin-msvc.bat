@echo off
cd ..\..
mkdir distrib
cd distrib

mkdir package
cd package

mkdir bin
copy ..\..\bin\libxmldiff.dll bin

zip -R -9 ..\libxmldiff_bin_msvc.zip *.*

cd ..
del /q /s package
rmdir /q /s package

cd ..\build\win32
