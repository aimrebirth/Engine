@echo off
setlocal
IF NOT EXIST tools cd ..
cmake -G "Visual Studio 15 2017 Win64" -H. -Bwin64
