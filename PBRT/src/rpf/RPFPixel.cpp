/*
 * RPFPixel.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#include "RPFPixel.h"
#include "RPFSample.h"
#include <iostream>


/*RPFPixel::RPFPixel(){
	rpfsamples = new RPFSample[0];
	nextSample = 0;
}*/

RPFPixel::RPFPixel(const int samplesPerPixel){
	/*rpfsamples = new RPFSample[samplesPerPixel];
	for (int i = 0; i < samplesPerPixel; ++i){
		new (&rpfsamples[i]) RPFSample();
	}
	nextSample = 0;*/
	nextSample = samplesPerPixel;
}

void RPFPixel::AddSample(){
	nextSample++;
	if(nextSample > 1){
		//std::cout << nextSample;
	}
	std::cout << "bla" << std::endl;
}
