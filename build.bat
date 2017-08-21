@echo off
setlocal EnableDelayedExpansion

:: get list of all .c files in current directory
set filelist=
for /r . %%a in (*.c) do (
    set filelist=!filelist! %%a
)

:: define vars
set error_log=%~n0_error.log

:: compile
echo %0: Stage - Compilation
echo %0: Compiling using gcc...
gcc *.c -o particle 2> %error_log%

:: report compilation result
if %ERRORLEVEL% equ 0 (
    echo %0: Build successfully finished.
) else (
    echo %0: Build failed at compilation stage.
    echo %0: See %error_log% for details.
)
