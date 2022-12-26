for /d %%G in ("Debug*") do rd /s /q "%%~G"
for /d %%G in ("Release*") do rd /s /q "%%~G"
rmdir /s / q .vs vcpkg_installed x64 win32 Release Debug
del /q ..\..\bin\*.ilk
del /q ..\..\bin\*.pdb
del /q ..\..\bin\*.bsc
pause