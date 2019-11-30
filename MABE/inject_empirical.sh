#!/bin/bash

cat makefile > temporary.txt
echo "EMP_SRC := /home/austin/research/tools/empirical_clean/Empirical/source" > makefile
echo " " >> makefile
cat temporary.txt >> makefile
rm temporary.txt
python3 inject_helper.py
