/*
 * RPFSample.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#include "RPFSample.h"

RPFSample::RPFSample() {
	int i = 0;
	for (i = 0; i < 3; ++i) {
		Lrgb[i] = 0.f;
	}
}

RPFSample& RPFSample::operator=( RPFSample& rhs ){
	Lrgb[0] = rhs.Lrgb[0];
	Lrgb[1] = rhs.Lrgb[1];
	Lrgb[2] = rhs.Lrgb[2];
	return *this;
}

