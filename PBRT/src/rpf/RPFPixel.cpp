/*
 * RPFPixel.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#include "RPFPixel.h"
#include "RPFSample.h"
#include <vector>

RPFPixel::RPFPixel(const int samplesPerPixel) : rpfsamples (){
	rpfsamples.reserve (samplesPerPixel); // Reserve memory not to allocate it 10 times...
	for (int i = 0; i < samplesPerPixel; ++i){
		rpfsamples.push_back(RPFSample ());
	}
	nextSample = 0;
	totalrgb[0] = 0;
	totalrgb[1] = 0;
	totalrgb[2] = 0;
}

RPFPixel& RPFPixel::operator=( RPFPixel& rhs ){
	nextSample = rhs.nextSample;
	unsigned int i;
	for (i = 0; i < rhs.rpfsamples.size(); ++i) {
		rpfsamples[i] = rhs.rpfsamples[i];
	}
	totalrgb[0] = rhs.totalrgb[0];
	totalrgb[1] = rhs.totalrgb[1];
	totalrgb[2] = rhs.totalrgb[2];
	return *this;
}


void RPFPixel::AddSample(RPFSample *newSample){
	if(nextSample >= rpfsamples.size()){
		return;
		//Severe("Severe error: more samples inserted than samplesPerPixel.");
	}
	rpfsamples[nextSample] = *newSample;


	totalrgb[0] += newSample->Lrgb[0];
	totalrgb[1] += newSample->Lrgb[1];
	totalrgb[2] += newSample->Lrgb[2];
	nextSample++;
}
