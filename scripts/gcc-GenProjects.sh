#!/bin/bash
pushd ..

if [ $# -eq 0 ] ; then
  echo "Please specify debug or release build with -d or -r. Build both with -dr."
fi

PREMAKE_BIN=dependencies/premake/bin/premake5
if [ ! -f "$PREMAKE_BIN" ]; then
  pushd dependencies/premake-core
  make -f Bootstrap.mak linux
  mv ./bin/release/premake5 ../premake/bin/
  popd
fi

architecture=""
case $(arch) in
  x86_64) architecture="x64" ;;
  armv7l) architecture="arm" ;;
  arm)    dpkg --print-architecture | grep -q "arm64" && architecture="arm64" || architecture="arm" ;;
esac

while getopts 'dr' flag
do
  case "${flag}" in
    d) 
      architecture+="d"
      echo "Building projects $architecture."
      dependencies/premake/bin/premake5 gmake
      make config=$architecture
      ;;
    r) 
      echo "Building projects $architecture."
      dependencies/premake/bin/premake5 gmake
      make config=$architecture
      ;;
    *) 
      echo "Please specify Debug or Release mode with -d or -r" ;;
  esac
done

popd