/*
 * RPFCollector.cpp
 *
 *  Created on: Dec 6, 2012
 *      Author: thypo
 */

#include "RPFCollector.h"
#include <cstring>

RPFCollector::RPFCollector(int xRes, int yRes){
	nbPixels = xRes*yRes;
	colorValues = new float[3*nbPixels];
}

void RPFCollector::setColorValues(float *values){
	memcpy(colorValues, values, sizeof(values)*3*nbPixels);
}

float *RPFCollector::getColorValues(){
	return colorValues;
}
