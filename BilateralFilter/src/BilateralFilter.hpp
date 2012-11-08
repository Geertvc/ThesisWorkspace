/*
 * BilateralFilter.hpp
 *
 *  Created on: Nov 6, 2012
 *      Author: thypo
 */

#include "TestImage.hpp"

#ifndef BILATERALFILTER_HPP_
#define BILATERALFILTER_HPP_

class BilateralFilter{
public:
	//Constructor that is given the domain and the range sigma.
	BilateralFilter(double sigmaD, double sigmaR, int n);
	TestImage * applyFilter(TestImage *image);

private:
	double sigmaD;
	double sigmaR;
	//Size of the neighborhood.
	int n;
	double applyToPixel(int i, int j, TestImage *image);
	double domainFilter(int p, int q, int i, int j);
	double rangeFilter(double neighBorPixelValue, double pixelValue);
};


#endif /* BILATERALFILTER_HPP_ */
