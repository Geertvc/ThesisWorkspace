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
		//The depth is smaller than 1.
		if(depth < 1){
			cout << "Error: The depth cannot be smaller than 1." << endl;
			cout << "Use -- help for help on using this program." << endl;
			return 1;
		}

		const char *fileName = argv[2];
		cout << "The depth is " << depth << endl;
		cout << "The file that will be used is " << fileName << endl;
		sierpinksiTriangle4(depth, fileName);
	}
	// strcmp can compare two strings and gives 0 if they are equal.
	else if((argc == 2) && (!strcmp(argv[1], "--help"))){
		cout << "usage: ObjectMaker [depth] [filename.obj]" << endl;
	} else{
		//Other arguments are given which cannot be used.
		cout << "Error: could not run using the following arguments:" << endl; // prints !!!Hello World!!!

		for(int i = 2; i< argc; i++){
			printf("arg %d: %s\n", i-2, argv[i]);
		}
		cout << "Use --help for help on using this program." << endl;
	}
	return 0;
}
