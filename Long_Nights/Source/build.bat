@echo off
pushd ..\..\..\build
cl -Zi ..\Long_Nights\Source\main.cpp User32.lib
popd