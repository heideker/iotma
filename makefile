# for raspbian, include libcurl4-openssl-dev package
all: 
	gcc -Wall -o iotma iotma.cpp -lstdc++ -lcurl -lpthread -ljsoncpp -std=c++11

