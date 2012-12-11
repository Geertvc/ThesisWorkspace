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

int RPFCollector::samplesPerPixel = 0;
/**
 * nbPixels and samplesPerPixel are initialized using the constructor initialization list.
 */
RPFCollector::RPFCollector(const int xResolution, const int yResolution, const int s)
: xRes(xResolution), yRes(yResolution), nbPixels(xRes*yRes){
	samplesPerPixel = s;
	//colorValues = new float[3*nbPixels*samplesPerPixel];
	std::cout << "nbPixel: " << nbPixels << std::endl;
	std::cout << "spp: " << samplesPerPixel << std::endl;
	//rpfpixels = new BlockedArray<RPFPixel>(xRes, yRes);
	rpfpixels = (RPFPixel*) malloc(sizeof(RPFPixel) * nbPixels);
	std::cout << "size of RPFPixel: " << sizeof(RPFPixel) << std::endl;
	int i;
	for (i = 0; i < nbPixels; ++i) {
		new (&rpfpixels[i]) RPFPixel(/*samplesPerPixel*/i);
	}

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
	L.ToRGB(newSample->Lrgb);
	//Haal de correcte pixel op
	//RPFPixel &rpfPixel = (*rpfpixels)(x, y);

	RPFPixel &rpfPixel = rpfpixels[x*yRes +y];
	//std::cout << rpfPixel.nextSample << std::endl;
	if(rpfPixel.nextSample != (x*yRes + y)){
		std::cout << x << ", " << y << ": " << (y*xRes + x) << ": " << rpfPixel.nextSample << std::endl;
	}

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
