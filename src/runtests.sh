#!/bin/bash

./a.out < test1.in > test1k.out

echo -e "TEST1\n"

diff -bw test1k.out test1.out
