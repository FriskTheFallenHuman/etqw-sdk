@echo off
mkdir build\msvc2022-x64
pushd build\msvc2022-x64
cmake -G "Visual Studio 17 2022" -A x64 -DQT_BASEDIR="D:/Qt/6.5.3/msvc2019_64/" %* ../..
popd
pause