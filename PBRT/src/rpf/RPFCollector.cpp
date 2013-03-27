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
RPFCollector::RPFCollector(const int xResolution, const int yResolution, const int s, RPFOptions &rpfOpt)
: xRes(xResolution), yRes(yResolution), nbPixels(xRes*yRes), samplesPerPixel(s), rpfPixels () {
	//samplesPerPixel = s;
	std::cout << " -> Resolution: width = " << xRes << ", height = " << yRes << std::endl;
	std::cout << " -> nbPixel: " << nbPixels << std::endl;
	std::cout << " -> spp: " << samplesPerPixel << std::endl;

	rpfPixels.reserve(nbPixels); // Reserve memory not to allocate it 10 times...
	for (int i = 0; i < nbPixels; ++i){
		//std::cout << i << std::endl;
		rpfPixels.push_back(RPFPixel (samplesPerPixel));
	}
	rpfOptions = rpfOpt;
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
	//clock_t start, end;
	//start = clock();

	float *xyz = new float[3*nbPixels];
	//RPFFilter *filter = new RPFFilter(sigmaD, sigmaR, n);
	//filter->applyFilter(rpfPixels, xyz, xRes, yRes, samplesPerPixel);

	RPF *rpf = new RPF(rpfOptions);
	rpf->applyFilter(rpfPixels, xyz, xRes, yRes, samplesPerPixel);


	//end = clock();
	//std::cout << " -> Total time used by RPF: " << (double)(end-start)/CLOCKS_PER_SEC << " seconds." << std::endl;

	std::cout << " -> Converting xyz values to rgb..." << std::endl;
	float *rgb = new float[3*nbPixels];
	int offset = 0;
	for (int y = 0; y < yRes; ++y) {
		for (int x = 0; x < xRes; ++x) {
			Assert((3*offset < 3*nbPixels));
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

	std::cout << " -> Writing image..." << std::endl;

	//string outputFileName = "testRgb.tga";
	string outputFileName = rpfOptions.rpfOutfile;
	WriteImage(outputFileName, rgb, NULL, xRes, yRes,
					 xRes, yRes, 0, 0);
	std::cout << " -> Image written to " << outputFileName << std::endl;
	delete rpf;
	delete[] xyz;
	delete[] rgb;
}
