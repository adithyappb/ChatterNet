#!/bin/bash

echo "Executing script"
gcc -o serverChat serverChat.c utilities.c
gcc -o clientChat -g -Wall clientChat.c utilities.c
echo "Compile [COMPLETED]"
