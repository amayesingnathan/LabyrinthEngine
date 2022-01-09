pushd %~dp0\..\
call dependencies\bin\premake\premake5.exe vs2019
popd
PAUSE