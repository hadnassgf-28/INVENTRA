@echo off
REM Windows: kompilasi program C++ lalu jalankan website
cd /d "%~dp0"
g++ -std=c++17 -O2 -o inventra.exe src\main.cpp
if errorlevel 1 (echo Kompilasi gagal. Pastikan g++ ^(MinGW^) terpasang. & pause & exit /b 1)
echo Kompilasi berhasil.
python server.py
pause
