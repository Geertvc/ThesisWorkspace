/*
 * BilateralFilter.hpp
 *
 *  Created on: Nov 6, 2012
 *      Author: thypo
 */

#include "TestImage.hpp"
#include <boost/gil/gil_all.hpp>

#ifndef BILATERALFILTER_HPP_
#define BILATERALFILTER_HPP_

class BilateralFilter{
public:
	//Constructor that is given the domain and the range sigma and the size of the neighboorhood n.
	BilateralFilter(double sigmaD, double sigmaR, int n);
	TestImage * applyFilter(TestImage *image);

	void applyFilter(const boost::gil::rgb8_image_t::view_t& input, const boost::gil::rgb8_image_t::view_t& output);

private:
	double sigmaD;
	double sigmaR;
	//Size of the neighborhood.
	int n;
	double applyToPixel(int i, int j, TestImage *image);
	double domainFilter(int p, int q, int i, int j);
	double rangeFilter(double neighBorPixelValue, double pixelValue);

	boost::gil::rgb8_pixel_t applyToPixel(int i, int j, const boost::gil::rgb8_image_t::view_t& input);
	double applyToChannel(int i, int j, int chan, const boost::gil::rgb8_image_t::view_t& input);
};


#endif /* BILATERALFILTER_HPP_ */
