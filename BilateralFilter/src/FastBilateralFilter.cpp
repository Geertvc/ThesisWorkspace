

#include "FastBilateralFilter.hpp"
#include <iostream>
#include <math.h>

#include <boost/gil/gil_all.hpp>
using namespace std;


FastBilateralFilter::FastBilateralFilter(double sigmaD, double sigmaR, int n){
	cout << "Making FastBilateralFilter" << endl;
	this->sigmaD = sigmaD;
	this->sigmaR = sigmaR;
	this->n = n;

	double twoSigmaSquared = 2*sigmaD*sigmaD;

	int kernelSize = n*2 + 1;
	gaussianKernel = new double*[kernelSize];
	for (int i = 0; i < kernelSize; ++i) {
		gaussianKernel[i] = new double[kernelSize];
	}

	for (int i = 0; i < kernelSize; ++i) {
		for (int j = 0; j < kernelSize; ++j) {
			gaussianKernel[i][j] = gaussian(twoSigmaSquared, i-n, j-n);
		}
	}

}

void FastBilateralFilter::applyFilter(const boost::gil::rgb8_image_t::view_t& input, const boost::gil::rgb8_image_t::view_t& output){
	for (int y=0; y<input.height(); ++y) {
		cout << "Row " << y << endl;
		for (int x=1; x<input.width()-1; ++x){
			//output(x,y) = input(x,y);
			output(x,y) = applyToPixel(x,y, input);
		}
	}
}

boost::gil::rgb8_pixel_t FastBilateralFilter::applyToPixel(int i, int j, const boost::gil::rgb8_image_t::view_t& input){
	/*char r = 0;
	char g = 255;
	char b = 255;*/

	boost::gil::rgb8_pixel_t pixel(0,0,0);
	for (int chan = 0; chan < 3; ++chan) {
		pixel[chan] = applyToChannel(i,j,chan, input);
	}

	/*boost::gil::rgb8_pixel_t rgb8(r, g, b);
	unsigned char testR = rgb8[0];
	unsigned char testG = rgb8[1];
	unsigned char testB = rgb8[2];
	cout << (int)testR << ", " << (int)testG << ", " << (int)testB << endl;*/
	return pixel;
}

double FastBilateralFilter::applyToChannel(int i, int j, int chan, const boost::gil::rgb8_image_t::view_t& input){
	double numeratorSum = 0.0;
	double denominatorSum = 0.0;
	double pixelValue = input(i,j)[chan];
	//cout << pixelValue << endl;
	int xRes = input.width();
	int yRes = input.height();
	for (int p = i-n; p <= i + n; ++p) {
		//Check of p binnen de image ligt.
		if(p > -1 && p < xRes){
			for (int q = j-n; q <= j + n; ++q) {
				//Check of q binnen de image ligt.
				if(q > -1 && q < yRes){
					double tempPixelValue = input(p,q)[chan];
					double filterValue = domainFilter(p, q, i, j)*rangeFilter(tempPixelValue, pixelValue);
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

double FastBilateralFilter::domainFilter(int p, int q, int i, int j){
	int x = i-p;
	int y = j-q;
	return gaussianKernel[x+n][y+n];
}

double FastBilateralFilter::rangeFilter(double neighBorPixelValue, double pixelValue){
	//Je moet hier toch geen absolute waarde nemen van double-double door het kwadraat e?
	double diff = neighBorPixelValue-pixelValue;
	//cout << diff << endl;
	if(diff == 0.0){
		return 0.0;
	}

	return exp(-pow((neighBorPixelValue-pixelValue)/this->sigmaR,2)/2);
}

double FastBilateralFilter::gaussian(double twoSigmaSquared, int i, int j){
	return exp(-((i*i + j*j) / (twoSigmaSquared)));
}

//template <typename T, size_t N>
//inline
//size_t SizeOfArray( const T(&)[ N ] )
//{
//  return N;
//}
