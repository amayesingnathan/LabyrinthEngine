#!/bin/bash

# Change working directory to the root of the engine.
pushd "$(dirname ${BASH_SOURCE[0]})/../.."

echo "Generating make files..."
dependencies/premake/bin/premake5 gmake

if [ $# -eq 0 ] ; then
  echo "To build, please specify debug or release build with -d or -r. Build both with -dr."
fi

# Get system architecture
architecture=""
case $(arch) in
  x86_64) architecture="x64" ;;
  armv7l) architecture="arm" ;;
  arm)    dpkg --print-architecture | grep -q "arm64" && architecture="arm64" || architecture="arm" ;;
esac

# Loop over debug or release flags
while getopts 'dr' flag
do
  case "${flag}" in
    d) 
      debugarch="${architecture}d"
      echo "Building projects $debugarch."
      make config=$debugarch
      ;;
    r) 
      echo "Building projects $architecture."
      make config=$architecture
      ;;
    *) 
      echo "Please specify Debug or Release mode with -d or -r" ;;
  esac
done

popd #$(dirname ${BASH_SOURCE[0]})/../..