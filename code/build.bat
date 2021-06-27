@echo off

mkdir ..\build
pushd ..\build


cl -Zi ..\code\win32_drawing_board.cpp User32.lib Gdi32.lib

popd