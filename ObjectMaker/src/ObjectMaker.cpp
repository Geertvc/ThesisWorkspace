//============================================================================
// Name        : ObjectMaker.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "sierpinksiTriangle.hpp"
using namespace std;

int main( int argc, const char* argv[]) {

	if(argc == 3){
		int depth;
		istringstream is(argv[1]);
		is >> depth;

		//string fileName = argv[2];
		//char *name = (char*)fileName.c_str();

		const char *fileName = argv[2];
		cout << depth << " " << fileName << endl;
		sierpinksiTriangle4(depth, fileName);
	}
	// strcmp can compare two strings and gives 0 if they are equal.
	else if((argc == 2) && (!strcmp(argv[1], "--help"))){
		cout << "Help" << endl;
	}


	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	for(int i = 0; i< argc; i++){
		printf("arg %d: %s\n", i, argv[i]);
	}
	return 0;
}
