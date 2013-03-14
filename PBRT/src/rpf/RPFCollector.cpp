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
#include "RPFFilter.h"
#include "spectrum.h"
#include "imageio.h"
#include <sstream>
#include "RPF.h"

/**
 * initialization using the constructor initialization list.
 */
RPFCollector::RPFCollector(const int xResolution, const int yResolution, const int s)
: xRes(xResolution), yRes(yResolution), nbPixels(xRes*yRes), samplesPerPixel(s), rpfPixels () {
	//samplesPerPixel = s;
	std::cout << " -> Resolution: width = " << xRes << ", height = " << yRes << std::endl;
	std::cout << " -> nbPixel: " << nbPixels << std::endl;
	std::cout << " -> spp: " << samplesPerPixel << std::endl;

	rpfPixels.reserve (nbPixels); // Reserve memory not to allocate it 10 times...
	for (int i = 0; i < nbPixels; ++i){
		//std::cout << i << std::endl;
		rpfPixels.push_back (RPFPixel (samplesPerPixel));
	}
}

//Dit wordt parallel aangeroepen maar geen probleem want alleen writes naar aparte plaatsen in de array hier.
void RPFCollector::AddSample(const CameraSample &sample, const Spectrum &L, const RayDifferential &ray, const Intersection &intersection){
	//Zet continu sample waarde om naar discrete range.
	//Zet sample waarde om naar discrete waarde.
	int x = Floor2Int(sample.imageX);
	int y = Floor2Int(sample.imageY);

	//Filter out all samples with x > xRes-1 or y > yRes -1
	if(x > (xRes-1) || y > (yRes-1)){
		return;
	}

	//Maak een correct RPFSample aan.
	RPFSample *newSample = new RPFSample();
	//Set color values.
	L.ToXYZ(newSample->Lrgb);
	//Set position of the sample
	newSample->imageX = sample.imageX;
	newSample->imageY = sample.imageY;
	//Set random parameters.
	newSample->randomX = sample.imageX-x;
	newSample->randomY = sample.imageY-y;
	newSample->randomLensU = sample.lensU;
	newSample->randomLensV = sample.lensV;
	newSample->randomTime = sample.time;
	//Set normal.
	Normal normal = intersection.dg.nn;
	newSample->nx = normal.x;
	newSample->ny = normal.y;
	newSample->nz = normal.z;
	//Set world-space position.
	Point point = intersection.dg.p;
	newSample->wx = point.x;
	newSample->wy = point.y;
	newSample->wz = point.z;

	//y*xRes + x sorts the entries like this: (0,0), (1,0), ... (xRes, 0), (0,1), (1,1), ... (xRes, 1), ....
	rpfPixels[y*xRes + x].AddSample(newSample);

	delete newSample;
	//Sommige pixels hebben teveel samples?
	/*if(rpfPixels[index].nextSample > samplesPerPixel){
		std::cout << "too much: " << index << std::endl;
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
	std::cout << " -> RPF execution started" << std::endl;
	clock_t start, end;
	start = clock();
	/*//De grootte van de neighboorhood.
	int n = 7;
	//Dit is sigmaP uit de paper.
	double sigmaD = n/4;//3.0;
	//De experimenteel bepaalde sigma voor 8 samples per pixel tot de tweede macht:
	double sigma8Squared = 0.002;
	//De sigma die gebruikt wordt om sigmaC en sigmaF te berekenen tot de tweede macht:
	double sigmaSquared = 8*sigma8Squared/samplesPerPixel;
	//Dit is sigmaC uit de paper
	double sigmaR = sigmaSquared;//20.0;//1/0.0;*/
	float *xyz = new float[3*nbPixels];
	//RPFFilter *filter = new RPFFilter(sigmaD, sigmaR, n);
	//filter->applyFilter(rpfPixels, xyz, xRes, yRes, samplesPerPixel);

	RPF *rpf = new RPF();
	rpf->applyFilter(rpfPixels, xyz, xRes, yRes, samplesPerPixel);


	end = clock();
	std::cout << " -> Total time used by RPF: " << (double)(end-start)/CLOCKS_PER_SEC << " seconds." << std::endl;

	std::cout << " -> Converting xyz values to rgb..." << std::endl;
	float *rgb = new float[3*nbPixels];
	int offset = 0;
	for (int y = 0; y < yRes; ++y) {
		for (int x = 0; x < xRes; ++x) {
			XYZToRGB(&xyz[3*offset], &rgb[3*offset]);
			/*if(rgb[3*offset] > 1)
				rgb[3*offset] = 0;
			if(rgb[3*offset+1] > 1)
				rgb[3*offset+1] = 0;
			if(rgb[3*offset+2] > 1)
				rgb[3*offset+2] = 0;*/

			/*if(y< 10 ){
				std::cout << rgb[3*offset] << ", " << rgb[3*offset + 1] << ", " << rgb[3*offset +2] << std::endl;
			}*/

			/*if(rgb[3*offset] < 0.f)
				rgb[3*offset] = 0.f;
			if(rgb[3*offset+1] < 0.f)
				rgb[3*offset+1] = 0.f;
			if(rgb[3*offset+2] < 0.f)
				rgb[3*offset+2] = 0.f;*/

			/*if(isinf(rgb[3*offset])){
				std::cout << "0 is: " << rgb[3*offset] << std::endl;
			}
			if(isinf(rgb[3*offset + 1])){
				std::cout << "1 is: " << rgb[3*offset + 1] << std::endl;
			}
			if(isinf(rgb[3*offset + 2])){
				std::cout << "2 is: " << rgb[3*offset + 2] << std::endl;
			}*/

			offset++;
		}
	}
	std::cout << " -> Converting to rgb finished" << std::endl;

	/*std::cout << " -> Writing image..." << std::endl;
	std::ostringstream os;
	os << "images/bunny";
	os << "D=";
	os << sigmaD;
	os << "R=";
	os << sigmaR;
	os << "n=";
	os << n;
	os << "spp=";
	os << samplesPerPixel;
	os << "size=";
	os << xRes;
	os << "x";
	os << yRes;
	os << ".tga";*/
	//string outputFileName = os.str();
	string outputFileName = "testRgb.tga";
	WriteImage(outputFileName, rgb, NULL, xRes, yRes,
					 xRes, yRes, 0, 0);
	std::cout << " -> Image written to " << outputFileName << std::endl;



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
}


		//How executeRPF() worked with boost
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
