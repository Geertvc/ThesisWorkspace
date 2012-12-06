/*
 * RPFCollector.cpp
 *
 *  Created on: Dec 6, 2012
 *      Author: thypo
 */

#include "RPFCollector.h"
#include <cstring>
#include <iostream>

RPFCollector::RPFCollector(const int xRes, const int yRes){
	nbPixels = xRes*yRes;
	colorValues = new float[3*nbPixels];
}

void RPFCollector::setColorValues(float *values){
	memcpy(colorValues, values, sizeof(values)*3*nbPixels);
}

float *RPFCollector::getColorValues(){
	return colorValues;
}

void RPFCollector::AddSample(const CameraSample &sample, const Spectrum &L){
	std::cout << "sample: (" << sample.imageX << ", " << sample.imageY << ")" << std::endl;
}

