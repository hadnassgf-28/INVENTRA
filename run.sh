#!/usr/bin/env bash
# Linux / macOS: kompilasi program C++ lalu jalankan website
set -e
cd "$(dirname "$0")"
g++ -std=c++17 -O2 -o inventra src/main.cpp
echo "Kompilasi berhasil."
python3 server.py
