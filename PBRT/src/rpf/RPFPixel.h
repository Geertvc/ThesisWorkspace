/*
 * RPFPixel.h
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#ifndef RPFPIXEL_H_
#define RPFPIXEL_H_

#include "RPFSample.h"
#include <vector>

class RPFPixel{
public:
	RPFPixel(const int samplesPerPixel);
	RPFPixel& operator=( RPFPixel& rhs );
	~RPFPixel(){}
	void AddSample(RPFSample *newSample);
	std::vector<RPFSample> rpfsamples;
	unsigned int nextSample;
	float totalrgb[3];
};


#endif /* RPFPIXEL_H_ */
