//============================================================================
// Name        : Main.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "TestImage.hpp"
#include "BilateralFilter.hpp"
using namespace std;

int main() {
	cout << "Bilateral Filter" << endl; // prints !!!Hello World!!!

	int xRes = 5;
	int yRes = 5;

	TestImage *image = new TestImage(xRes, yRes);
	BilateralFilter *filter = new BilateralFilter(1.0, 1.0, 1);
	TestImage *result = filter->applyFilter(image);
	result->print();

	return 0;
}


//int main() {
//	cout << "Bilateral Filter" << endl; // prints !!!Hello World!!!
//
//	int xRes = 5;
//	int yRes = 5;
//	//double data[xRes][yRes] = {{1,2},{3,4}};
//	double data[xRes][yRes];
//
//	//Filling the data
//	for (int i = 0; i < xRes; ++i) {
//		for (int j = 0; j < yRes; ++j) {
//			data[i][j] = i*xRes + j;
//			cout << "(" << i << ", " << j << "): " << data[i][j] << endl;
//		}
//	}
//
//
//
//
//	return 0;
//}
