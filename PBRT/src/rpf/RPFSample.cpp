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

