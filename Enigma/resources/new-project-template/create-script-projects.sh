#!/bin/bash

pushd "$(dirname ${BASH_SOURCE[0]})"
scriptGen = "${LAB_ROOT_DIR}dependencies/premake/bin/premake5 gmake"
$(scriptGen)
popd #$(dirname ${BASH_SOURCE[0]})