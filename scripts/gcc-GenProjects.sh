#!/bin/bash

pushd ..
dependencies/premake/bin/premake5 gmake
make
make help
popd