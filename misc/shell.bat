@echo off

echo 2
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
set path=w:\volleyball\misc;%path%
doskey ci=git commit -a -m
