all: main.o
	g++ -pthread -o jetson main.o
main.o: main.cpp
	g++ -std=c++11 -c main.cpp
clean: jetson main.o
	rm jetson main.o
