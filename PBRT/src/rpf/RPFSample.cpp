/*
 * RPFSample.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#include "RPFSample.h"
#include <algorithm>

RPFSample::RPFSample() {
	int i = 0;
	for (i = 0; i < 3; ++i) {
		Lrgb[i] = 0.f;
	}
}

/*RPFSample::RPFSample( RPFSample& other ){
	Lrgb[0] = other.Lrgb[0];
	Lrgb[1] = other.Lrgb[1];
	Lrgb[2] = other.Lrgb[2];
}*/

RPFSample& RPFSample::operator=( RPFSample& rhs ){
	Lrgb[0] = rhs.Lrgb[0];
	Lrgb[1] = rhs.Lrgb[1];
	Lrgb[2] = rhs.Lrgb[2];
	return *this;
}

