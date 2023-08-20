clang -x c tests\test_mem_debug.c -o test.exe -std=c89 -Wno-deprecated-declarations 



@echo off
REM gcc -x c tests/test_mem_debug.c -o test.exe -std=c89 -Wno-deprecated-declarations -fsanitize=address

rem REM or rem is a comment in batch script

REM Get start time
for /f "tokens=1-4 delims=:.," %%a in ("%time%") do (
   set /A "start=(((%%a*60)+1%%b %% 100)*6000)+1%%c%%d-610000"
)


REM Run Compiled Program
@echo on
.\test.exe
@echo off


REM Get end time
for /f "tokens=1-4 delims=:.," %%a in ("%time%") do (
   set /A "end=(((%%a*60)+1%%b %% 100)*6000)+1%%c%%d-610000"
)

REM Calculate elapsed time by subtracting values
set /A elapsed=end-start

echo Elapsed time: %elapsed% milliseconds
@echo on

