/*
 * FastestBilateralFilter.h
 *
 *  Created on: Dec 17, 2012
 *      Author: thypo
 */

#include <boost/gil/gil_all.hpp>
#include "RPFPixel.h"

#ifndef FASTESTBILATERALFILTER_H_
#define FASTESTBILATERALFILTER_H_

class FastestBilateralFilter {
public:
	//Constructor that is given the domain and the range sigma and the size of the neighboorhood n.
	FastestBilateralFilter(double sigmaD, double sigmaR, int n);
	void applyFilter(std::vector<RPFPixel> input, const boost::gil::rgb8_image_t::view_t& output, int xRes, int yRes, int samplesPerPixel);
	~FastestBilateralFilter();
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

	boost::gil::rgb8_pixel_t applyToPixel(int i, int j, std::vector<RPFPixel> input);
	double applyToChannel(int i, int j, int chan, std::vector<RPFPixel> input);
	double getPixelValue(int chan, RPFPixel pixel);
};

#endif /* FASTESTBILATERALFILTER_H_ */
