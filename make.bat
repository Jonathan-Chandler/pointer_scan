@ECHO OFF
call mingw32-make clean
echo.
call mingw32-make
echo.
if errorlevel 1 goto build_failed

rem rem wait 1 second for test target application to start
rem start "test_application" "test_application.exe"
rem SLEEP 1
rem echo.
rem call unit_test.exe
rem if errorlevel 1 goto unit_test_failed
rem echo.
call scanner.exe "Sacrifice"
goto exit_make

:build_failed
echo.
echo Build failed
goto exit_make

:unit_test_failed
echo.
echo Unit test failed
goto exit_make

:exit_make
