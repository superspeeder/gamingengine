#!/bin/bash

cd build/
ninja
cd ..

if [ ! -f gaming ]; then
    ln -s build/gaming
fi
