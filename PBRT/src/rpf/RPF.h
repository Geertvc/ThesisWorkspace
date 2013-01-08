/*
 * RPF.h
 *
 *  Created on: Jan 8, 2013
 *      Author: thypo
 */

#ifndef RPF_H_
#define RPF_H_

#include <vector>
#include "RPFPixel.h"

class RPF {
public:
	RPF();
	~RPF(){}
	void applyFilter(std::vector<RPFPixel> &input, float *xyz, int xResolution, int yResolution, int samplesPerPixel);
private:
	double sigma;
};

#endif /* RPF_H_ */
