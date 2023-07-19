gcc tests\threaded_test_1.c -o test.exe


@echo off
:: Save the start time
echo Start time: %time%
set startTime=%time%

:: Run your command
.\test.exe

:: Save the end time
echo End time: %time%
set endTime=%time%

:: Calculate and display the runtime
set /A "startHour=1%startTime:~0,2%-100, endHour=1%endTime:~0,2%-100"
set /A "startMinute=1%startTime:~3,2%-100, endMinute=1%endTime:~3,2%-100"
set /A "startSecond=1%startTime:~6,2%-100, endSecond=1%endTime:~6,2%-100"
set /A "startHundredths=1%startTime:~9,2%-100, endHundredths=1%endTime:~9,2%-100"

:: Calculate elapsed time by subtracting values
set /A "elapHour=endHour-startHour"
set /A "elapMinute=endMinute-startMinute"
set /A "elapSecond=endSecond-startSecond"
set /A "elapHundredths=endHundredths-startHundredths"

:: Normalize elapsed time
if %elapHundredths% lss 0 (
    set /A "elapSecond-=1"
    set /A "elapHundredths+=100"
)
if %elapSecond% lss 0 (
    set /A "elapMinute-=1"
    set /A "elapSecond+=60"
)
if %elapMinute% lss 0 (
    set /A "elapHour-=1"
    set /A "elapMinute+=60"
)

:: Display elapsed time
echo Exe elapsed time: %elapHour%:%elapMinute%:%elapSecond%.%elapHundredths%