/*
 * sierpinskiTrangle.cpp
 *
 *  Created on: Nov 1, 2012
 *      Author: thypo
 */

#include <iostream>
#include <fstream>
using namespace std;

void sierpinksiTriangle4(int depth, const char* fileName){
	ofstream file;
	file.open(fileName);
	cout << "Writing to " << fileName << endl;
	file << "v 1 1 1" << endl;
	file << "v 1 1 -1" << endl;
	file << "v 1 -1 1" << endl;
	file << "v 1 -1 -1" << endl;
	file << "v -1 1 1" << endl;
	file << "v -1 1 -1" << endl;
	file << "v -1 -1 1" << endl;
	file << "v -1 -1 -1" << endl;
	file << "f 1 3 4 2" << endl;
	file << "f 5 7 8 6" << endl;
	file << "f 1 5 6 2" << endl;
	file << "f 3 7 8 4" << endl;
	file << "f 1 5 7 3" << endl;
	file << "f 2 6 8 4" << endl;


	file.close();
}


