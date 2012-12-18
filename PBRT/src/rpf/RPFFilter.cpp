/*
 * RPFFilter.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: thypo
 */

#include "RPFFilter.h"
#include <iostream>
#include <cmath>
#include "RPFPixel.h"
#include "progressreporter.h"
using namespace std;

RPFFilter::RPFFilter(double sigmaD, double sigmaR, int n)
: sigmaD(sigmaD), sigmaR(sigmaR), n(n), xRes(0), yRes(0), samplesPerPixel(0){
	cout << "Making RPFFilter" << endl;

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

RPFFilter::~RPFFilter() {
	int kernelSize = n*2 + 1;
	for (int i = 0; i < kernelSize; ++i) {
		delete[] gaussianKernel[i];
	}
	delete[] gaussianKernel;
	delete[] rangeKernel;
}

void RPFFilter::applyFilter(std::vector<RPFPixel> &input, float *rgb, int xResolution, int yResolution, int samples){
	xRes = xResolution;
	yRes = yResolution;
	samplesPerPixel = samples;

	ProgressReporter reporter(yRes, "Filtering");

	unsigned int offset = 0;
	for (int y=0; y<yRes; ++y) {
		//cout << "Row " << y << endl;
		for (int x=0; x<xRes; ++x){
			for (int chan = 0; chan < 3; ++chan) {
				rgb[3*offset + chan] = applyToChannel(x, y, chan, input);
			}
			//cout << "Column " << x << endl;
			offset++;
		}
		reporter.Update();
	}
	reporter.Done();
}

double RPFFilter::domainFilter(int p, int q, int i, int j){
	int x = i-p;
	int y = j-q;
	return gaussianKernel[x+n][y+n];
}

double RPFFilter::rangeFilter(double neighBorPixelValue, double pixelValue){
	return rangeKernel[(int) abs(neighBorPixelValue-pixelValue)];
}

double RPFFilter::gaussian(double twoSigmaSquared, int i, int j){
	return exp(-((i*i + j*j) / (twoSigmaSquared)));
}

double RPFFilter::applyToChannel(int x, int y, int chan, std::vector<RPFPixel> &input){
	double numeratorSum = 0.0;
	double denominatorSum = 0.0;
	//Calculate pixel Value from sample values.
	double pixelValue = input[y*xRes + x].totalrgb[chan]/samplesPerPixel;

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
	return numeratorSum/denominatorSum;
}

double RPFFilter::getPixelValue(int chan, RPFPixel &pixel){
	double total = 0.0;
	for (unsigned int i = 0; i < pixel.rpfsamples.size(); ++i) {
		total += pixel.rpfsamples[i].Lrgb[chan];
	}
	return total/samplesPerPixel;
}

