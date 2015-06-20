@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
set path=w:\handmade\misc;c:\Program Files (x86)\GnuWin32\bin;%path%
doskey ci=git commit -a -m
