#all:
#g++ main.cpp csketch.hpp csketch_merged.hpp hasher.hpp hasher_merged.hp -O3 -o pis -fopenmp -std=c++11

#./pis 23 20 0.01 0.002 8 2
CXX =g++
CXXFLAGS =-O3 -o pis -fopenmp -std=c++11
SRCS =main.cpp csketch_merged.hpp  hasher_merged.hpp csketch_merged_padded.hpp 
all:
	$(CXX) $(SRCS) $(CXXFLAGS)
clean:
	rm  pis
