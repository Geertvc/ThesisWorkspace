/*
 * FastestBilateralFilter.cpp
 *
 *  Created on: Dec 17, 2012
 *      Author: thypo
 */

#include "FastestBilateralFilter.h"
#include <iostream>
#include <math.h>
#include <boost/gil/gil_all.hpp>
#include "RPFPixel.h"
#include "spectrum.h"
using namespace std;

FastestBilateralFilter::FastestBilateralFilter(double sigmaD, double sigmaR, int n)
: sigmaD(sigmaD), sigmaR(sigmaR), n(n), xRes(0), yRes(0), samplesPerPixel(0){
	cout << "Making FastestBilateralFilter" << endl;

	double twoSigmaDSquared = 2*sigmaD*sigmaD;
	double twoSigmaRSquared = 2*sigmaR*sigmaR;

	int kernelSize = n*2 + 1;
	gaussianKernel = new double*[kernelSize];
	for (int i = 0; i < kernelSize; ++i) {
		gaussianKernel[i] = new double[kernelSize];
	}
	for (int i = 0; i < kernelSize; ++i) {
		for (int j = 0; j < kernelSize; ++j) {
			gaussianKernel[i][j] = gaussian(twoSigmaDSquared, i-n, j-n);
		}
	}

	rangeKernel = new double[256];
	for (int i = 0; i < 256; ++i) {
		rangeKernel[i] = exp((double)-((i*i) / twoSigmaRSquared));
	}
}

void FastestBilateralFilter::applyFilter(std::vector<RPFPixel> input, const boost::gil::rgb8_image_t::view_t& output, int xResolution, int yResolution, int s){
	xRes = xResolution;
	yRes = yResolution;
	samplesPerPixel = s;
	for (int y=0; y<yRes; ++y) {
		cout << "Row " << y << endl;
		for (int x=0; x<xRes; ++x){
			//boost::gil::rgb8_pixel_t pixel(255,255,255);
			//output(x,y) = pixel;
			//cout << "Column " << x << endl;
			output(x,y) = applyToPixel(x,y, input);
		}
	}
}

boost::gil::rgb8_pixel_t FastestBilateralFilter::applyToPixel(int x, int y, std::vector<RPFPixel> input){
	/*char r = 0;
	char g = 255;
	char b = 255;*/


	float xyz[3];
	xyz[0] = getPixelValue(0, input[y*xRes +x]);
	xyz[1] = getPixelValue(1, input[y*xRes +x]);
	xyz[2] = getPixelValue(2, input[y*xRes +x]);
	float rgb[3];
	XYZToRGB(xyz,rgb);


	boost::gil::rgb8_pixel_t pixel(0,0,0);
	for (int chan = 0; chan < 3; ++chan) {
		//cout << "Chan " << chan << endl;
		//pixel[chan] = applyToChannel(x,y,chan, input);

		//double value = getPixelValue(chan, input[y*xRes+x]);
		//pixel[chan] = value > 1 ? 255 : value*256;



		pixel[chan] = rgb[chan]*255;
	}

	/*boost::gil::rgb8_pixel_t rgb8(r, g, b);
	unsigned char testR = rgb8[0];
	unsigned char testG = rgb8[1];
	unsigned char testB = rgb8[2];
	cout << (int)testR << ", " << (int)testG << ", " << (int)testB << endl;*/
	return pixel;
}

double FastestBilateralFilter::applyToChannel(int x, int y, int chan, std::vector<RPFPixel> input){
	double numeratorSum = 0.0;
	double denominatorSum = 0.0;
	//Calculate pixel Value from sample values.
	double pixelValue = input[y*xRes + x].totalrgb[chan]/samplesPerPixel;

	//cout << "PixelValue: " << pixelValue << endl;
	for (int p = x-n; p <= x + n; ++p) {
		//Check of p binnen de image ligt.
		if(p > -1 && p < xRes){
			for (int q = y-n; q <= y + n; ++q) {
				//Check of q binnen de image ligt.
				if(q > -1 && q < yRes){
					double tempPixelValue = input[q*xRes+p].totalrgb[chan]/samplesPerPixel;
					double filterValue = domainFilter(p, q, x, y)*rangeFilter(tempPixelValue, pixelValue);
					numeratorSum += tempPixelValue*filterValue;
					denominatorSum += filterValue;
					//cout << rangeFilter(tempPixelValue, pixelValue) << endl;
				}
			}
		}
	}
	//double result = numeratorSum/denominatorSum;
	//cout << denominatorSum << endl;
	return numeratorSum/denominatorSum;
	//cout << pixelValue << endl;
	//return 100;
}

double FastestBilateralFilter::domainFilter(int p, int q, int i, int j){
	int x = i-p;
	int y = j-q;
	return gaussianKernel[x+n][y+n];
}

double FastestBilateralFilter::rangeFilter(double neighBorPixelValue, double pixelValue){
	//Je moet hier toch geen absolute waarde nemen van double-double door het kwadraat e?
	//int diff = (int) neighBorPixelValue-pixelValue;
	//cout << abs(diff) << endl;
	return rangeKernel[(int) abs(neighBorPixelValue-pixelValue)];
}

double FastestBilateralFilter::gaussian(double twoSigmaSquared, int i, int j){
	return exp(-((i*i + j*j) / (twoSigmaSquared)));
}

double FastestBilateralFilter::getPixelValue(int chan, RPFPixel pixel){
	double total = 0.0;
	for (unsigned int i = 0; i < pixel.rpfsamples.size(); ++i) {
		total += pixel.rpfsamples[i].Lrgb[chan];
	}
	return total/samplesPerPixel;
}

