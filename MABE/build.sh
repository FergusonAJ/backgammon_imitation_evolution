rm makefile
rm mabe
rm objectFiles/World_BackgammonWorld_BackgammonWorld.o
python3 pythonTools/mbuild.py -p8
./inject_empirical.sh
make
