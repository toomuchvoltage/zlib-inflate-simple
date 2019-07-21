@echo off
echo Finding VC variables batch file...
set "curdir=%cd%"
cd \
FOR /F "tokens=* USEBACKQ" %%F IN (`dir vcvarsall.bat /s /a /b`) DO (
	SET vcvarsloc=%%F
)
echo Done!
cd %curdir%
call "%vcvarsloc%" x86_amd64
cl /I .\include /EHsc main.cpp zlibinflate.cpp .\src\adler32.c .\src\compress.c .\src\crc32.c .\src\deflate.c .\src\gzclose.c .\src\gzlib.c .\src\gzread.c .\src\gzwrite.c .\src\infback.c .\src\inffast.c .\src\inflate.c .\src\inftrees.c .\src\trees.c .\src\uncompr.c .\src\zutil.c > nul
del *.obj
main.exe
pause