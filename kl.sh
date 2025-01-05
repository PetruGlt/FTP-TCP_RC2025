#!/bin/bash

if [ ! -f "xprocesator/procesator.sh" ]; then
    echo "Error: 'procesator.sh' not found in 'xprocesator' directory." >%2
    exit 1
fi

if [ ! -x "xprocesator/procesator.sh" ]; then
    echo "Error: 'procesator.sh' does not have execution permission." >%2
    exit 1
fi

if [ ! -f "validators/validatorASCII.c" ]; then
    echo "Error: 'valiatorASCII.c' not found in 'validators' directory." >%2
    exit 2
fi

if [ ! -r "validators/validatorASCII.c" ]; then
    echo "Error: 'valiatorASCII.c' does not have reading permission." >%2
    exit 2
fi

if [ ! -d "$1" ]; then
    echo "Error: Directory '$1' not found.">&2
    exit 3
fi

if [ ! -r "$1" ];then 
    echo "Error: '$1' does not have reading permission.">&2;
    exit 3
fi

xprocesator/procesator.sh "$1"

exit #?
