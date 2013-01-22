/*
 * RPFSample.h
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#ifndef RPFSAMPLE_H_
#define RPFSAMPLE_H_

#include "core/geometry.h"

class RPFSample {
public:
	RPFSample();
	RPFSample& operator=( RPFSample& rhs );
	~RPFSample(){}
	//Color values
	float Lrgb[3];

	//The position of the sample in the image.
	float imageX, imageY;

	/*Random parameters*/
	//Random position within the pixel
    float randomX, randomY;
    //Random position on lens
    float randomLensU, randomLensV;
    //Random time in interval
    float randomTime;
    /*Scene features for the first intersection point of the ray*/
    //Normal
    float nx, ny, nz;
    //World-space position
    float wx, wy, wz;
    //Texture values (the set of floats from texture lookups)
    //TODO
    /*Scene features for the second intersection point of the ray*/
    //World-space position
    //TODO
    //Normal
    //TODO
};

#endif /* RPFSAMPLE_H_ */
