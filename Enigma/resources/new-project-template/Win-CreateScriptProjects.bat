@echo off

pushd %~dp0
call $LAB_ROOT_DIR$\dependencies\premake\bin\premake5.exe vs2022
popd
