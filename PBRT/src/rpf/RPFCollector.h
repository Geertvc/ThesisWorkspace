/*
 * RPFCollector.h
 *
 *  Created on: Dec 6, 2012
 *      Author: thypo
 */

#ifndef RPFCOLLECTOR_H_
#define RPFCOLLECTOR_H_

#include "spectrum.h"
#include "sampler.h"
#include "memory.h"
#include "RPFPixel.h"
#include <stdlib.h>
#include <vector>
#include <iostream>

class RPFCollector {
public:
	RPFCollector(const int xRes, const int yRes, const int nbOfsamplesPerPixel);
	//void setColorValues(float *values);
	//float *getColorValues();
	void AddSample(const CameraSample &sample, const Spectrum &L);
	void ExecuteRPF();
	~RPFCollector(){
		//free(rpfpixels);
	}
private:
	const int xRes;
	const int yRes;
	const int nbPixels;
	//float *colorValues;
	const int samplesPerPixel;
	//BlockedArray<RPFPixel> *rpfpixels;
	//RPFPixel *rpfpixels;
	std::vector<RPFPixel> rpfPixels;
};


#endif /* RPFCOLLECTOR_H_ */
