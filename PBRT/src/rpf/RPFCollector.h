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
	void AddSample(const CameraSample &sample, const Spectrum &L);
	void ExecuteRPF();
	~RPFCollector(){
	}
private:
	const int xRes;
	const int yRes;
	const int nbPixels;
	const int samplesPerPixel;
	std::vector<RPFPixel> rpfPixels;
};


#endif /* RPFCOLLECTOR_H_ */
