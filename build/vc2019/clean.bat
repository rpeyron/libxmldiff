rmdir /s / q .vs
for /d %%G in ("Debug*") do rd /s /q "%%~G"
for /d %%G in ("Release*") do rd /s /q "%%~G"
del /q ..\..\bin\*.ilk
del /q ..\..\bin\*.pdb
del /q ..\..\bin\*.bsc
pause