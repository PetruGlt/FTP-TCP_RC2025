#!/bin/bash

if [ "$#" -ne 1 ]; then
    exit 2
fi

if [ -f "$1" ]; then
    ./filegraffiti "$1" "$USER"
elif [ -d "$1" ]; then
    for entry in "$1"/*; do
        $0 "$entry"
    done
fi
