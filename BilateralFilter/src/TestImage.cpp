/*
 * TestImage.cpp
 *
 *  Created on: Nov 6, 2012
 *      Author: thypo
 */

#include "TestImage.hpp"
#include <iostream>
using namespace std;

	int xRes;
	int yRes;
	double **data;

	TestImage::TestImage(int xRes, int yRes){
		cout << "Making Test Image" << endl;
		this->xRes = xRes;
		this->yRes = yRes;
		data = new double*[xRes];
		for (int i = 0; i < xRes; ++i) {
			data[i] = new double[yRes];
		}


		//Filling the data
		for (int i = 0; i < xRes; ++i) {
			for (int j = 0; j < yRes; ++j) {
				data[i][j] = i*xRes + j;
				cout << "(" << i << ", " << j << "): " << data[i][j] << endl;
			}
		}
	}

	double TestImage::getValue(int i, int j){
		return data[i][j];
	}

	void TestImage::setValue(int i, int j, double value){
		data[i][j] = value;
	}

	void TestImage::print(){
		for (int i = 0; i < yRes; ++i) {
			for (int j = 0; j < xRes; ++j) {
				cout << data[i][j] << " ";
			}
			cout << endl;
		}
	}

	TestImage::~TestImage(){
		for (int i = 0; i < xRes; ++i) {
			delete [] data[i];
		}
		delete [] data;
	}

