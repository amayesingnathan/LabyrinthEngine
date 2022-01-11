pushd %~dp0\..\
call dependencies\premake\bin\premake5.exe vs2019
popd
PAUSE