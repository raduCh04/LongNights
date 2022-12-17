@echo off
pushd ..\..\build
cl -FC -Zi ..\Long_Nights\Source\main.cpp User32.lib Gdi32.lib
popd