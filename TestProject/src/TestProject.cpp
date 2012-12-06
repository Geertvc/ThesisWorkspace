//============================================================================
// Name        : TestProject.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include "rpf/RPFCollector.h"
using namespace std;

void RPFCollectorTest();
void sizeofTest();
void stringstreamTest();

template <class T>
void printArray(T *array, int size);

int main() {
	RPFCollectorTest();
	//sizeofTest();
	//stringstreamTest();
	return 0;
}

void RPFCollectorTest(){
	//RPFCollector test
	int xRes = 5;
	int yRes = 5;
	int size = 3*xRes*yRes;
	RPFCollector *test = new RPFCollector(xRes,yRes);
	float *values = new float[size];
	int i;
	for (i = 0; i < size; ++i) {
		values[i] = i;
	}
	printArray <float> (values, size);
	test->setColorValues(values);
	printArray <float> (test->getColorValues(), size);
}

template <class T>
void printArray(T *array, int size){
	int i;
	for (i = 0; i < size; ++i) {
		cout << i << ": " << array[i] << endl;
	}
}

void sizeofTest(){
	//othertest
	int xRes = 100;
	int yRes = 200;
	float *longArray = new float[3*xRes*yRes];
	cout << "size: " << sizeof(longArray) << endl;

	float longArray2[3*xRes*yRes];
	cout << "size: " << sizeof(longArray2) << endl;
}

void stringstreamTest(){
	//test
	float testValue = 1.2345678910;
	float value2 = 2.3456789;
	float value3 = 3.456;
	stringstream os;
	os << setprecision(numeric_limits<float>::digits10+3);
	os << testValue;
	string result = os.str();

	/*std::string result = boost::lexical_cast<std::string>(testValue);
	result.append(" ");
	result.append(boost::lexical_cast<std::string>(value2));
	result.append(" ");
	result.append(boost::lexical_cast<std::string>(value3));*/
	cout << result; // prints !!!Hello World!!!


	ofstream file;
	file.open("testfile.txt");
	file << "f " << testValue << " " << value2 << " " << value3 << endl;
	file.close();
}
