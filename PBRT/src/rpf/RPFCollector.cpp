/*
 * RPFCollector.cpp
 *
 *  Created on: Dec 6, 2012
 *      Author: thypo
 */

#include "RPFCollector.h"
#include "RPFPixel.h"
#include "RPFSample.h"
#include <cstring>
#include <iostream>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include "FastestBilateralFilter.h"
#include "RPFFilter.h"
#include "spectrum.h"
#include "imageio.h"

//int RPFCollector::samplesPerPixel = 0;
/**
 * nbPixels and samplesPerPixel are initialized using the constructor initialization list.
 */
RPFCollector::RPFCollector(const int xResolution, const int yResolution, const int s)
: xRes(xResolution), yRes(yResolution), nbPixels(xRes*yRes), samplesPerPixel(s), rpfPixels () {
	//samplesPerPixel = s;
	std::cout << "Resolution: width = " << xRes << ", height = " << yRes << std::endl;
	std::cout << "nbPixel: " << nbPixels << std::endl;
	std::cout << "spp: " << samplesPerPixel << std::endl;

	rpfPixels.reserve (nbPixels); // Reserve memory not to allocate it 10 times...
	for (int i = 0; i < nbPixels; ++i){
		//std::cout << i << std::endl;
		rpfPixels.push_back (RPFPixel (samplesPerPixel));
	}

	/*RPFPixel *newPixel = new RPFPixel(samplesPerPixel);
	int i;
	for (i = 0; i < samplesPerPixel; ++i) {
		std::cout << "sample " << i << " (" << newPixel->rpfsamples[i].Lrgb[0] << ", " << newPixel->rpfsamples[i].Lrgb[1] << ", " << newPixel->rpfsamples[i].Lrgb[2] << ")" << std::endl;
	}

	RPFSample *newSample = new RPFSample();
	newSample->Lrgb[0] = 1.f;
	newSample->Lrgb[1] = 2.f;
	newSample->Lrgb[2] = 3.f;
	newPixel->AddSample(newSample);
	newPixel->AddSample(newSample);
	newPixel->AddSample(newSample);
	newPixel->AddSample(newSample);
	newPixel->AddSample(newSample);
	newPixel->AddSample(newSample);
	newPixel->AddSample(newSample);
	newPixel->AddSample(newSample);

	for (i = 0; i < samplesPerPixel; ++i) {
		std::cout << "sample " << i << " (" << newPixel->rpfsamples[i].Lrgb[0] << ", " << newPixel->rpfsamples[i].Lrgb[1] << ", " << newPixel->rpfsamples[i].Lrgb[2] << ")" << std::endl;
	}*/


	/*rpfpixels = (RPFPixel*) malloc(sizeof(RPFPixel) * nbPixels);
	std::cout << "size of RPFPixel: " << sizeof(RPFPixel) << std::endl;
	int i;
	for (i = 0; i < nbPixels; ++i) {
		new (&rpfpixels[i]) RPFPixel(i);
		std::cout << "pixel " << i << std::endl;
	}*/

	/*int j;
	for (i = 0; i < xRes; ++i) {
		for (j = 0; j < yRes; ++j) {
			RPFPixel &rpfPixel = rpfpixels[(i*yRes)+j];
			std::cout << rpfPixel.nextSample << std::endl;
		}
	}*/

	/*int i;
	int j;
	int k;
	for (i = 0; i < xRes; ++i) {
		for (j = 0; j < yRes; ++j) {
			RPFPixel &rpfPixel = (*rpfpixels)(i,j);
			RPFSample *rpfsamples = rpfPixel.rpfsamples;
			for (k = 0; k < samplesPerPixel; ++k) {
				std::cout << "(" << i << ", " << j << "): " << rpfsamples[k].Lrgb[0] << ", " << rpfsamples[k].Lrgb[1] << ", " << rpfsamples[k].Lrgb[2] << std::endl;
			}
		}
	}*/
}

/*void RPFCollector::setColorValues(float *values){
	memcpy(colorValues, values, sizeof(values)*3*nbPixels);
}*/

/*float *RPFCollector::getColorValues(){
	return colorValues;
}*/

//Problemen om dit parallel aan te roepen.
void RPFCollector::AddSample(const CameraSample &sample, const Spectrum &L){
	//Zet continu sample waarde om naar discrete range.
	//float dimageX = sample.imageX - 0.5f;
	//float dimageY = sample.imageY - 0.5f;
	//Zet sample waarde om naar discrete waarde.
	int x = Floor2Int(sample.imageX);
	int y = Floor2Int(sample.imageY);

	//Filter out all samples with x > xRes-1 or y > yRes -1
	if(x > (xRes-1) || y > (yRes-1)){
		return;
	}

	//Maak een correct RPFSample aan.
	RPFSample *newSample = new RPFSample();
	L.ToXYZ(newSample->Lrgb);

	//y*xRes + x sorts the entries like this: (0,0), (1,0), ... (xRes, 0), (0,1), (1,1), ... (xRes, 1), ....
	int index = y*xRes + x;
	rpfPixels[index].AddSample(newSample);

	/*if(rpfPixels[index].nextSample > samplesPerPixel){
		std::cout << "too much: " << index << std::endl;
	}*/

	//Haal de correcte pixel op
	//RPFPixel &rpfPixel = (*rpfpixels)(x, y);
	//RPFPixel &rpfPixel = rpfpixels[x*yRes +y];
	//rpfPixel.AddSample(newSample);
	//std::cout << rpfPixel.nextSample << std::endl;
	/*if(rpfPixel.nextSample != (x*yRes + y)){
		std::cout << x << ", " << y << ": " << (y*xRes + x) << ": " << rpfPixel.nextSample << std::endl;
	}*/

	/*if(sample.imageX < 1){
		std::cout << x << ": " << sample.imageX << ": " << std::endl;
	} else{
		return;
	}*/

	/*if(x == 0){
		std::cout << "xyes";
	}*/
	/*if(y == 0){
		std::cout << "yyes";
	}*/




	//RPFPixel rpfPixel = rpfpixels[x*xRes + y];
	//rpfPixel.AddSample(newSample);
	//rpfPixel.nextSample += 1;
	//rpfPixel.AddSample();

	/*float rgb[3];
	L.ToRGB(rgb);
	if(newSample.Lrgb[0] - rgb[0] > 0.00000000001){
		std::cout << "the 0 is different" << std::endl;
	}
	if(newSample.Lrgb[1] - rgb[1] > 0.000000000001){
		std::cout << "the 1 is different" << std::endl;
	}
	if(newSample.Lrgb[2] - rgb[2] > 0.0000000000001){
		std::cout << "the 2 is different" << std::endl;
	}*/

	//std::cout << "sample: (" << sample.imageX << ", " << sample.imageY << ")" << std::endl;
	//std::cout << "rgb: (" << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << ")" << std::endl;
	//if(rgb[0] > 0 || rgb[1] > 0 || rgb[2] > 0){
		//std::cout << "(" << sample.imageX << ", " << sample.imageY << ") " << "(" << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << ")" << std::endl;
	//}
	/*if(sample.imageX < 1.f || sample.imageY < 1.f
			|| sample.imageX > 640.f || sample.imageY > 480.f){
		std::cout << "sample: (" << sample.imageX << ", " << sample.imageY << ")" << std::endl;
	}*/
}

double getPixelValue(int chan, RPFPixel pixel){
	double total = 0.0;
	for (unsigned int i = 0; i < pixel.rpfsamples.size(); ++i) {
		total += pixel.rpfsamples[i].Lrgb[chan];
	}
	return total;
}

void RPFCollector::ExecuteRPF(){
	std::cout << "RPF execution started" << std::endl;
	/*double sigmaD = 3.0;
	double sigmaR = 20.0;//1/0.0;
	int n = 1;
	//int usedFilterVersion = 3;
	std::cout << "Bilateral Filter (sigmaD=" << sigmaD << ", sigmaR=" << sigmaR << ", n=" << n << ")" << std::endl;
	typedef boost::gil::pixel<boost::gil::bits8, boost::gil::rgb_layout_t> rgb8_pixel_t;

	boost::gil::rgb8_image_t outputImage(xRes,yRes);
	rgb8_pixel_t rgb8(0,0,0);
	boost::gil::fill_pixels(view(outputImage), rgb8);
	const boost::gil::rgb8_image_t::view_t& outputImageview = boost::gil::rgb8_image_t::view_t(outputImage._view);

	clock_t start, end;
	start = clock();

	FastestBilateralFilter *filter = new FastestBilateralFilter(sigmaD, sigmaR, n);
	std::cout << "Start filtering (v3.0)..." << std::endl;
	filter->applyFilter(rpfPixels, outputImageview, xRes, yRes, samplesPerPixel);
	std::cout << "Filtering done." << std::endl;

	end = clock();
	std::cout << "Elapsed time: " << (double)(end-start)/CLOCKS_PER_SEC << " seconds." << std::endl;

	string outputFileName = "images/testAgain.jpg";
	std::cout << "Writing image to " << outputFileName << std::endl;
	boost::gil::jpeg_write_view(outputFileName, view(outputImage));
	std::cout << "Writing image done." << std::endl;*/

	/*float *rgb = new float[3*nbPixels];
	int offset = 0;
	for (int y=0; y<yRes; ++y) {
		std::cout << "Row " << y << std::endl;
		for (int x=0; x<xRes; ++x){
			// Convert pixel XYZ color to RGB
			float xyz[3];
			xyz[0] = getPixelValue(0, rpfPixels[y*xRes +x]);
			xyz[1] = getPixelValue(1, rpfPixels[y*xRes +x]);
			xyz[2] = getPixelValue(2, rpfPixels[y*xRes +x]);
			XYZToRGB(xyz, &rgb[3*offset]);

			float invWt = 1.f / samplesPerPixel;
			rgb[3*offset  ] = max(0.f, rgb[3*offset  ] * invWt);
			rgb[3*offset+1] = max(0.f, rgb[3*offset+1] * invWt);
			rgb[3*offset+2] = max(0.f, rgb[3*offset+2] * invWt);
			offset++;
		}
	}
	string outputFileName = "images/newTest.tga";
	WriteImage(outputFileName, rgb, NULL, xRes, yRes,
	                 xRes, yRes, 0, 0);*/

	double sigmaD = 3.0;
	double sigmaR = 20.0;//1/0.0;
	int n = 5;
	float *rgb = new float[3*nbPixels];
	RPFFilter *filter = new RPFFilter(sigmaD, sigmaR, n);
	clock_t start, end;
	start = clock();
	filter->applyFilter(rpfPixels, rgb, xRes, yRes, samplesPerPixel);
	end = clock();
	std::cout << "Elapsed time: " << (double)(end-start)/CLOCKS_PER_SEC << " seconds." << std::endl;
	string outputFileName = "images/newTest2.tga";
	WriteImage(outputFileName, rgb, NULL, xRes, yRes,
					 xRes, yRes, 0, 0);

	/*int i;
	int j;
	for (i = 0; i < yRes; ++i) {
		for (j = 0; j < xRes; ++j) {
			RPFPixel &rpfPixel = rpfPixels[(i*xRes)+j];
			for (int k = 0; k < rpfPixel.rpfsamples.size(); ++k) {
				RPFSample &rpfSample = rpfPixel.rpfsamples[k];
				std::cout << j << ", " << i << ": " << k << " = "<< "(" << rpfSample.Lrgb[0] << ", " << rpfSample.Lrgb[1] << ", " << rpfSample.Lrgb[2] << ")" << std::endl;
			}
		}
	}*/
}
