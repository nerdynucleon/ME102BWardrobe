all: testcamera

testcamera: main.o
	g++ main.o -o test `pkg-config opencv --cflags --libs`

main.o: main.cpp
	g++ -c main.cpp -I.

clean:
	rm *o test
