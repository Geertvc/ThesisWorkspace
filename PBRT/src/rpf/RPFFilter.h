/*
 * RPFFilter.h
 *
 *  Created on: Dec 18, 2012
 *      Author: thypo
 */

#ifndef RPFFILTER_H_
#define RPFFILTER_H_

#include "RPFPixel.h"

class RPFFilter {
public:
	//Constructor that is given the domain and the range sigma and the size of the neighboorhood n.
	RPFFilter(double sigmaD, double sigmaR, int n);
	void applyFilter(std::vector<RPFPixel> &input, float *rgb, int xRes, int yRes, int samplesPerPixel);
	~RPFFilter();
private:
	double sigmaD;
	double sigmaR;
	//Size of the neighborhood.
	int n;
	double **gaussianKernel;
	double *rangeKernel;
	int xRes;
	int yRes;
	int samplesPerPixel;

	double domainFilter(int p, int q, int i, int j);
	double rangeFilter(double neighBorPixelValue, double pixelValue);
	double gaussian(double sigmaSquared, int i, int j);

	double applyToChannel(int i, int j, int chan, std::vector<RPFPixel> &input);
	double getPixelValue(int chan, RPFPixel &pixel);
};

#endif /* RPFFILTER_H_ */
