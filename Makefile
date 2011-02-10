all: main

headers: functions.oa globals.oa notWAMStructures.oa notWAMFunctions.oa

globals.oa:
	g++-4 -std=c++0x -Iheaders -c -o globals.o headers/globals.cpp
functions.oa: 
	g++-4 -std=c++0x -Iheaders -c -o functions.o headers/functions.cpp 
notWAMStructures.oa: 
	g++-4 -std=c++0x -Iheaders -c -o notWAMStructures.o headers/notWAMStructures.cpp 
notWAMFunctions.oa: 
	g++-4 -std=c++0x -Iheaders -c -o notWAMFunctions.o headers/notWAMFunctions.cpp 

main: headers 
	g++-4 -std=c++0x -Iheaders globals.o functions.o notWAMStructures.o notWAMFunctions.o main.cpp -o mainx
