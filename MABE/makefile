EMP_SRC := /home/austin/research/tools/empirical_clean/Empirical/source

all: mabe

mabe: objectFiles/main.o objectFiles/Analyze_neurocorrelates.o objectFiles/Archivist_DefaultArchivist.o objectFiles/Brain_AbstractBrain.o objectFiles/Genome_AbstractGenome.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_MTree.o objectFiles/Utilities_Parameters.o objectFiles/Utilities_Loader.o objectFiles/Utilities_Filesystem.o objectFiles/Utilities_CSV.o objectFiles/World_AbstractWorld.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Optimizer_EcoEAOptimizer_EcoEAOptimizer.o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o objectFiles/Brain_ANNBrain_ANNBrain.o 
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread objectFiles/main.o objectFiles/Analyze_neurocorrelates.o objectFiles/Archivist_DefaultArchivist.o objectFiles/Brain_AbstractBrain.o objectFiles/Genome_AbstractGenome.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_MTree.o objectFiles/Utilities_Parameters.o objectFiles/Utilities_Loader.o objectFiles/Utilities_Filesystem.o objectFiles/Utilities_CSV.o objectFiles/World_AbstractWorld.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Optimizer_EcoEAOptimizer_EcoEAOptimizer.o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o objectFiles/Brain_ANNBrain_ANNBrain.o  -o mabe

objectFiles/main.o: ./main.cpp
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./main.cpp -o objectFiles/main.o -I ${EMP_SRC}

objectFiles/Analyze_neurocorrelates.o: ./Analyze/neurocorrelates.cpp ./Analyze/neurocorrelates.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Analyze/neurocorrelates.cpp -o objectFiles/Analyze_neurocorrelates.o

objectFiles/Archivist_DefaultArchivist.o: ./Archivist/DefaultArchivist.cpp ./Archivist/DefaultArchivist.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Archivist/DefaultArchivist.cpp -o objectFiles/Archivist_DefaultArchivist.o

objectFiles/Brain_AbstractBrain.o: ./Brain/AbstractBrain.cpp ./Brain/AbstractBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/AbstractBrain.cpp -o objectFiles/Brain_AbstractBrain.o

objectFiles/Genome_AbstractGenome.o: ./Genome/AbstractGenome.cpp ./Genome/AbstractGenome.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Genome/AbstractGenome.cpp -o objectFiles/Genome_AbstractGenome.o

objectFiles/Global.o: ./Global.cpp ./Global.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Global.cpp -o objectFiles/Global.o

objectFiles/Group_Group.o: ./Group/Group.cpp ./Group/Group.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Group/Group.cpp -o objectFiles/Group_Group.o

objectFiles/Optimizer_AbstractOptimizer.o: ./Optimizer/AbstractOptimizer.cpp ./Optimizer/AbstractOptimizer.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Optimizer/AbstractOptimizer.cpp -o objectFiles/Optimizer_AbstractOptimizer.o

objectFiles/Organism_Organism.o: ./Organism/Organism.cpp ./Organism/Organism.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Organism/Organism.cpp -o objectFiles/Organism_Organism.o

objectFiles/Utilities_Data.o: ./Utilities/Data.cpp ./Utilities/Data.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/Data.cpp -o objectFiles/Utilities_Data.o

objectFiles/Utilities_MTree.o: ./Utilities/MTree.cpp ./Utilities/MTree.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/MTree.cpp -o objectFiles/Utilities_MTree.o

objectFiles/Utilities_Parameters.o: ./Utilities/Parameters.cpp ./Utilities/Parameters.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/Parameters.cpp -o objectFiles/Utilities_Parameters.o

objectFiles/Utilities_Loader.o: ./Utilities/Loader.cpp ./Utilities/Loader.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/Loader.cpp -o objectFiles/Utilities_Loader.o

objectFiles/Utilities_Filesystem.o: ./Utilities/Filesystem.cpp ./Utilities/Filesystem.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/Filesystem.cpp -o objectFiles/Utilities_Filesystem.o

objectFiles/Utilities_CSV.o: ./Utilities/CSV.cpp ./Utilities/CSV.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/CSV.cpp -o objectFiles/Utilities_CSV.o

objectFiles/World_AbstractWorld.o: ./World/AbstractWorld.cpp ./World/AbstractWorld.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./World/AbstractWorld.cpp -o objectFiles/World_AbstractWorld.o

objectFiles/Genome_CircularGenome_CircularGenome.o: ./Genome/CircularGenome/CircularGenome.cpp ./Genome/CircularGenome/CircularGenome.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Genome/CircularGenome/CircularGenome.cpp -o objectFiles/Genome_CircularGenome_CircularGenome.o

objectFiles/Optimizer_EcoEAOptimizer_EcoEAOptimizer.o: ./Optimizer/EcoEAOptimizer/EcoEAOptimizer.cpp ./Optimizer/EcoEAOptimizer/EcoEAOptimizer.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Optimizer/EcoEAOptimizer/EcoEAOptimizer.cpp -o objectFiles/Optimizer_EcoEAOptimizer_EcoEAOptimizer.o

objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o: ./Optimizer/LexicaseOptimizer/LexicaseOptimizer.cpp ./Optimizer/LexicaseOptimizer/LexicaseOptimizer.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Optimizer/LexicaseOptimizer/LexicaseOptimizer.cpp -o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o

objectFiles/Brain_ANNBrain_ANNBrain.o: ./Brain/ANNBrain/ANNBrain.cpp ./Brain/ANNBrain/ANNBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/ANNBrain/ANNBrain.cpp -o objectFiles/Brain_ANNBrain_ANNBrain.o

clean:
	rm -r objectFiles/* mabe

cleanup:
	rm -r objectFiles/*
