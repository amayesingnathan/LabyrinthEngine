#!/usr/bin/env bash
pushd ..
dependencies/premake/bin/premake5 gmake
make
popd