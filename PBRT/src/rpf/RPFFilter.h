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
	//Starts the filter algorithm on the input variable and puts the output in the rgb array.
	void applyFilter(std::vector<RPFPixel> &input, float *xyz, int xRes, int yRes, int samplesPerPixel);
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

	//Returns the value of the domain filter for the given coordinates of the 2 pixels.
	double domainFilter(int p, int q, int i, int j);
	//Returns the value of the range filter depending on the value of the 2 pixels.
	double rangeFilter(double neighBorPixelValue, double pixelValue);
	//Returns the value of the gaussian function for the given parameters.
	double gaussian(double sigmaSquared, int i, int j);
	//Returns the filtered value of pixel (x,y) for channel chan.
	double applyToChannel(int x, int y, int chan, std::vector<RPFPixel> &input);
	//Returns the pixel value based on all the sample values.
	double getPixelValue(int chan, RPFPixel &pixel);
};

#endif /* RPFFILTER_H_ */
