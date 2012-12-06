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

class RPFCollector {
public:
	RPFCollector(const int xRes, const int yRes);
	void setColorValues(float *values);
	float *getColorValues();
	void AddSample(const CameraSample &sample, const Spectrum &L);
	~RPFCollector(){
		delete[] colorValues;
	}
private:
	int nbPixels;
	float *colorValues;
};


#endif /* RPFCOLLECTOR_H_ */
