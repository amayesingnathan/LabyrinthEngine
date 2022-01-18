#!/usr/bin/env bash
pushd ..
./premake5 gmake
make
popd