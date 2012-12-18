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
	//RPFPixel();
	RPFPixel(const int samplesPerPixel);
	//RPFPixel( RPFPixel& other );
	RPFPixel& operator=( RPFPixel& rhs );
	~RPFPixel(){
		//delete[] rpfsamples;
	}
	void AddSample(RPFSample *newSample);
	//RPFSample *rpfsamples;
	std::vector<RPFSample> rpfsamples;
	unsigned int nextSample;
	float totalrgb[3];
};


#endif /* RPFPIXEL_H_ */
