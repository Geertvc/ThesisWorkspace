/*
 * RPFSample.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#include "RPFSample.h"
#include "core/geometry.h"

RPFSample::RPFSample() {
	int i = 0;
	for (i = 0; i < 3; ++i) {
		Lrgb[i] = 0.f;
	}
	imageX = 0.f;
	imageY = 0.f;

    randomX = 0.f;
    randomY = 0.f;
    randomLensU = 0.f;
    randomLensV = 0.f;
    randomTime = 0.f;
    nx = 0.f;
    ny = 0.f;
    nz = 0.f;
    wx = 0.f;
    wy = 0.f;
    wz = 0.f;
    //TODO rest params
}

RPFSample& RPFSample::operator=( RPFSample& rhs ){
	Lrgb[0] = rhs.Lrgb[0];
	Lrgb[1] = rhs.Lrgb[1];
	Lrgb[2] = rhs.Lrgb[2];
	imageX = rhs.imageX;
	imageY = rhs.imageY;

	randomX = rhs.randomX;
	randomY = rhs.randomY;
	randomLensU = rhs.randomLensU;
	randomLensV = rhs.randomLensV;
	randomTime = rhs.randomTime;
	nx = rhs.nx;
	ny = rhs.ny;
	nz = rhs.nz;
	wx = rhs.wx;
	wy = rhs.wy;
	wz = rhs.wz;
	//TODO rest params
	return *this;
}

