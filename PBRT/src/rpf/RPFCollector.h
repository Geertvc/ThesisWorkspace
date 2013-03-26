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
#include "core/intersection.h"
#include "core/geometry.h"

class RPFCollector {
public:
	RPFCollector(const int xRes, const int yRes, const int nbOfsamplesPerPixel, RPFOptions &rpfOptions);
	void AddSample(const CameraSample &sample, const Spectrum &L, const RayDifferential &ray, const Intersection &intersection);
	void ExecuteRPF();
	~RPFCollector(){
	}
private:
	const int xRes;
	const int yRes;
	const int nbPixels;
	const int samplesPerPixel;
	std::vector<RPFPixel> rpfPixels;
	RPFOptions rpfOptions;
};


#endif /* RPFCOLLECTOR_H_ */
