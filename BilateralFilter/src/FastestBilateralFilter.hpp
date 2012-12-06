/*
 * FastestBilateralFilter.hpp
 *
 *  Created on: Nov 13, 2012
 *      Author: Geert Van Campenhout
 */

#include <boost/gil/gil_all.hpp>

#ifndef FASTESTBILATERALFILTER_HPP_
#define FASTESTBILATERALFILTER_HPP_

class FastestBilateralFilter{
public:
	//Constructor that is given the domain and the range sigma and the size of the neighboorhood n.
	FastestBilateralFilter(double sigmaD, double sigmaR, int n);
	void applyFilter(const boost::gil::rgb8_image_t::view_t& input, const boost::gil::rgb8_image_t::view_t& output);

private:
	double sigmaD;
	double sigmaR;
	//Size of the neighborhood.
	int n;
	double **gaussianKernel;
	double *rangeKernel;

	double domainFilter(int p, int q, int i, int j);
	double rangeFilter(double neighBorPixelValue, double pixelValue);
	double gaussian(double sigmaSquared, int i, int j);

	boost::gil::rgb8_pixel_t applyToPixel(int i, int j, const boost::gil::rgb8_image_t::view_t& input);
	double applyToChannel(int i, int j, int chan, const boost::gil::rgb8_image_t::view_t& input);
};


#endif /* FASTESTBILATERALFILTER_HPP_ */
