all : homework2.exe

homework2.exe : main.cpp node.o
	g++ main.cpp node.o -o homework2.exe
	
node.o : node.h node.cpp
	g++ -c node.cpp -o node.o