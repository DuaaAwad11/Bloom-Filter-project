#!/bin/bash
g++ -std=c++17 -O2 -Wall -Wextra main.cpp HashFunctions.cpp BloomHasher.cpp -o main -lcrypto -lssl
if [ $? -eq 0 ]; then
    echo "Compilation successful! Run with: ./main"
else
    echo "Compilation failed."
fi
