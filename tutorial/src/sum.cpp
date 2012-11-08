/*
 * sum.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: thypo
 */

#include <iostream>
#include "FList.h"
using namespace std;

double sum(FList List){
	double sum = 0.0;
	for(int i = 0; i < List.Count(); i++){
		sum += List.Retrieve(i);
		cout << "Sum: " << sum << endl;
		cout << "Retrieve " << i << ": " << List.Retrieve(i) << endl;
	}
	return sum;
}


