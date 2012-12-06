//============================================================================
// Name        : Main.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "BilateralFilter.hpp"
#include "FastBilateralFilter.hpp"
#include "FastestBilateralFilter.hpp"
#include "proces.hpp"

#include <time.h>

#include <string>
#include <sstream>

//#include <boost/gil/image.hpp>
//#include <boost/gil/typedefs.hpp>
//#include <boost/gil/extension/io/jpeg_io.hpp>

#include <boost/gil/gil_all.hpp>
using namespace std;

int main() {
	int imageSize = 512;
	double sigmaD = 3.0;
	double sigmaR = 20.0;//1/0.0;
	int n = 20;
	int usedFilterVersion = 3;

	cout << "Bilateral Filter (sigmaD=" << sigmaD << ", sigmaR=" << sigmaR << ", n=" << n << ")" << endl; // prints !!!Hello World!!!

	/*int xRes = 5;
	int yRes = 5;
	TestImage *image = new TestImage(xRes, yRes);
	BilateralFilter *filter = new BilateralFilter(1.0, 1.0, 1);
	TestImage *result = filter->applyFilter(image);
	result->print();*/

	typedef boost::gil::pixel<boost::gil::bits8, boost::gil::rgb_layout_t> rgb8_pixel_t;

	ostringstream nameStream;
	nameStream << "lena";
	nameStream << imageSize;
	nameStream << ".jpg";
	string inputFileName = nameStream.str();

	//string inputFileName = "lena256.jpg";
	cout << "Reading image " << inputFileName << endl;
	boost::gil::rgb8_image_t inputImage;
	boost::gil::jpeg_read_image(inputFileName,inputImage);
	boost::gil::rgb8_image_t outputImage(inputImage.dimensions());

	rgb8_pixel_t rgb8(0,0,0);
	boost::gil::fill_pixels(view(outputImage), rgb8);

	const boost::gil::rgb8_image_t::view_t& inputImageview = boost::gil::rgb8_image_t::view_t(inputImage._view);
	const boost::gil::rgb8_image_t::view_t& outputImageview = boost::gil::rgb8_image_t::view_t(outputImage._view);



	clock_t start, end;
	start = clock();



	if(usedFilterVersion == 1){
		BilateralFilter *filter = new BilateralFilter(sigmaD, sigmaR, n);
		cout << "Start filtering (v1.0)..." << endl;
		filter->applyFilter(inputImageview, outputImageview);
		cout << "Filtering done." << endl;
	} else if(usedFilterVersion == 2){
		FastBilateralFilter *filter = new FastBilateralFilter(sigmaD, sigmaR, n);
		cout << "Start filtering (v2.0)..." << endl;
		filter->applyFilter(inputImageview, outputImageview);
		cout << "Filtering done." << endl;
	} else{
		FastestBilateralFilter *filter = new FastestBilateralFilter(sigmaD, sigmaR, n);
		cout << "Start filtering (v3.0)..." << endl;
		filter->applyFilter(inputImageview, outputImageview);
		cout << "Filtering done." << endl;
	}

	end = clock();
	cout << "Elapsed time: " << (double)(end-start)/CLOCKS_PER_SEC << " seconds." << endl;

	/*for (int y=0; y<inputImageview.height(); ++y) {
		//boost::gil::rgb8_view_t::x_iterator src_it = inputImageview.row_begin(y);
	    //boost::gil::rbg8_view_t::x_iterator dst_it = outputImageview.row_begin(y);
	    for (int x=1; x<inputImageview.width()-1; ++x){
	        //dst_it[x] = src_it[x];
	    	outputImageview(x,y) = inputImageview(x,y);
	    	//cout << "(" << x << ", " << y << ")" << endl;
	    }double
	}*/






	//process(inputImageview, outputImageview);

	//process(boost::gil::nth_channel_view(view(inputImage), 1),boost::gil::nth_channel_view(view(inputImage),1));

	//boost::gil::rgb8_view_t src_view = boost::gil::interleaved_view(inputImage.width(), inputImage.height(), (x_iterator_t)&(inputImage(0,0)[n]), view(inputImage).pixels().row_size());

	//const_view(inputImage);
	//view(outputImage);

	//process(const_view(inputImage), view(outputImage));



	//boost::gil::rgb8c_view_t  src = interleaved_view(w,h,(const rgb8_pixel_t*)src_pixels,src_row_bytes);
	//boost::gil::rgb16s_view_t dst = interleaved_view(w,h,(    rgb16s_pixel_t*)dst_pixels,dst_row_bytes);

	ostringstream os;
	os << "scaledlena";
	os << "D=";
	os << sigmaD;
	os << "R=";
	os << sigmaR;
	os << "n=";
	os << n;
	os << "size=";
	os << imageSize;
	os << "v=";
	os << usedFilterVersion;
	os << ".jpg";
	string outputFileName = "testAgain.jpg";//os.str();

	cout << "Writing image to " << outputFileName << endl;
	boost::gil::jpeg_write_view(outputFileName, view(outputImage));
	cout << "Writing image done." << endl;
	return 0;
}


//int main() {
//	cout << "Bilateral Filter" << endl; // prints !!!Hello World!!!
//
//	int xRes = 5;
//	int yRes = 5;
//	//double data[xRes][yRes] = {{1,2},{3,4}};
//	double data[xRes][yRes];
//
//	//Filling the data
//	for (int i = 0; i < xRes; ++i) {
//		for (int j = 0; j < yRes; ++j) {
//			data[i][j] = i*xRes + j;
//			cout << "(" << i << ", " << j << "): " << data[i][j] << endl;
//		}
//	}
//
//
//
//
//	return 0;
//}
