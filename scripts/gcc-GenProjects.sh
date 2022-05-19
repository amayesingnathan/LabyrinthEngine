#!/bin/bash

architecture=""
case $(arch) in

    x86-64) architecture="x64" ;;
    armv7l) architecture="arm" ;;
    arm)    dpkg --print-architecture | grep -q "arm64" && architecture="arm64" || architecture="arm" ;;
esac

while getopts 'dr' flag
do
    case "${flag}" in
        d) 
          architecture+="d"
          echo "Building projects $architecture."
          pushd ..
          dependencies/premake/bin/premake5 gmake
          make config=$architecture
          popd
          ;;
        r) 
          echo "Building projects $architecture."
          pushd ..
          dependencies/premake/bin/premake5 gmake
          make config=$architecture
          popd
          ;;
        *) 
          echo "Please specify Debug or Release mode with -d or -r" ;;
    esac
done