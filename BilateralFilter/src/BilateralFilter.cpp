

#include "BilateralFilter.hpp"
#include "TestImage.hpp"
#include <iostream>
#include <math.h>
using namespace std;


BilateralFilter::BilateralFilter(double sigmaD, double sigmaR, int n){
	cout << "Making BilateralFilter" << endl;
	this->sigmaD = sigmaD;
	this->sigmaR = sigmaR;
	this->n = n;
}

TestImage * BilateralFilter::applyFilter(TestImage *image){
	cout << "Applying filter" << endl;
	TestImage *result = new TestImage(image->xRes, image->yRes);
	//Loop over all data
	for (int i = 0; i < image->xRes; ++i) {
		for (int j = 0; j < image->yRes; ++j) {
			result->setValue(i,j,applyToPixel(i,j, image));
		}
	}
	return result;
}

double BilateralFilter::applyToPixel(int i, int j, TestImage *image){
	double numeratorSum = 0.0;
	double denominatorSum = 0.0;
	double pixelValue = image->getValue(i,j);
	for (int p = i-n; p <= i + n; ++p) {
		//Check of p binnen de image ligt.
		if(p > -1 && p < image->xRes){
			for (int q = j-n; q <= j + n; ++q) {
				//Check of q binnen de image ligt.
				if(q > -1 && q < image->yRes){
					double tempPixelValue = image->getValue(p,q);
					numeratorSum += tempPixelValue*domainFilter(p, q, i, j)*rangeFilter(tempPixelValue, pixelValue);
					denominatorSum += domainFilter(p, q, i, j)*rangeFilter(tempPixelValue, pixelValue);
				}
			}
		}
	}
	return numeratorSum/denominatorSum;
}

double BilateralFilter::domainFilter(int p, int q, int i, int j){
	int x = i-p;
	int y = j-q;
	return exp(-pow((sqrt(x*x + y*y))/this->sigmaD,2)/2);
}

double BilateralFilter::rangeFilter(double neighBorPixelValue, double pixelValue){
	//Je moet hier toch geen absolute waarde nemen van double-double door het kwadraat e?
	return exp(-pow((neighBorPixelValue-pixelValue)/this->sigmaR,2)/2);
}

//template <typename T, size_t N>
//inline
//size_t SizeOfArray( const T(&)[ N ] )
//{
//  return N;
//}
