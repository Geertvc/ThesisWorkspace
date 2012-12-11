/*
 * RPFPixel.h
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#ifndef RPFPIXEL_H_
#define RPFPIXEL_H_

#include "RPFSample.h"

class RPFPixel{
public:
	//RPFPixel();
	RPFPixel(const int samplesPerPixel);
	~RPFPixel(){
		delete[] rpfsamples;
	}
	void AddSample();
	RPFSample *rpfsamples;
	int nextSample;
};


#endif /* RPFPIXEL_H_ */
