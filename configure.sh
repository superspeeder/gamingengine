#!/bin/bash

if [ ! -d build ]; then
    mkdir build
fi

cmake -S . -B build/ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G Ninja

if [ ! -f compile_commands.json ]; then
    ln -s build/compile_commands.json
fi


