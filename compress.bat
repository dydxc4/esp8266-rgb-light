@echo off
setlocal ENABLEEXTENSIONS
setlocal ENABLEDELAYEDEXPANSION
set sevenzip_path="C:\Program Files\7-Zip\7z.exe"
set input_path=%cd%\x-data
set output_path=%cd%\data

if not exist %input_path% echo Input folder missing && exit /b

cls
if exist %output_path% rmdir /s /q %output_path% && md %output_path%

echo Comprimiendo archivos
for /d /r "%input_path%" %%d in (*) do (
    set dir=%%~dpnd
    call set dir=!!dir:%input_path%=%output_path%!
    md !dir!

    for %%i in ("%%~d\*") do (
        echo Procesando %%i
        !sevenzip_path! a -tgzip -mx9 "!dir!\%%~nxi.gz" %%i > nul
    )
)

echo Completado!