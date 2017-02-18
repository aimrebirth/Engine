@echo off
setlocal
IF NOT EXIST tools cd ..
cmake -G "Visual Studio 14 2015 Win64" -H. -Bwin64
