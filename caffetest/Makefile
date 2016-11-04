CC = g++
INC = -I/home/ubuntu/caffe/include -I/usr/local/cuda-8.0/targets/aarch64-linux/include
LIBS = -lboost_system -lglog -L/home/ubuntu/caffe/lib -lcaffe `pkg-config --libs opencv`
CFLAGS = -g -O3 -Wall -D USE_OPENCV -std=c++11

all: classification

classification.o: classification.cpp
	$(CC) $(CFLAGS) $(INC) -c classification.cpp -o classification.o

classification: classification.o
	$(CC) -o test classification.o $(LIBS)

clean:
	rm test classification.o
