/*
 * RPFPixel.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#include "RPFPixel.h"
#include "RPFSample.h"
#include "error.h"
#include <iostream>
#include <vector>

/*RPFPixel::RPFPixel(){
	rpfsamples = new RPFSample[0];
	nextSample = 0;
}*/

RPFPixel::RPFPixel(const int samplesPerPixel) : rpfsamples (){
	//rpfsamples = new RPFSample[samplesPerPixel];
	//for (int i = 0; i < samplesPerPixel; ++i){
		//new (&rpfsamples[i]) RPFSample();
	//}
	//nextSample = 0;

	rpfsamples.reserve (samplesPerPixel); // Reserve memory not to allocate it 10 times...
	for (int i = 0; i < samplesPerPixel; ++i){
		rpfsamples.push_back (RPFSample ());
	}
	nextSample = 0;
	totalrgb[0] = 0;
	totalrgb[1] = 0;
	totalrgb[2] = 0;
}

/*RPFPixel::RPFPixel( RPFPixel& other){
	nextSample = other.nextSample;
	rpfsamples = other.rpfsamples;
}

RPFPixel& operator=( const RPFPixel& rhs ){
	  RPFPixel tmp( rhs );

	  // Now, swap the data members with the temporary:
	  std::swap( nextSample, tmp.nextSample );
	  std::swap( rpfsamples, tmp.rpfsamples );

	  return *this;
}*/

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
	//std::cout << newSample->Lrgb[0] << ", " << newSample->Lrgb[1] << ", " << newSample->Lrgb[2] << std::endl;
	//std::cout << nextSample << " -> " << rpfsamples.size() << std::endl;
	if(nextSample >= rpfsamples.size()){
		//std::cout << nextSample << std::endl;
		//nextSample++;
		return;
		//Severe("Severe error: more samples inserted than samplesPerPixel.");
	}
	rpfsamples[nextSample] = *newSample;
	totalrgb[0] += newSample->Lrgb[0];
	totalrgb[1] += newSample->Lrgb[1];
	totalrgb[2] += newSample->Lrgb[2];
	nextSample++;
	//if(nextSample > 1){
		//std::cout << nextSample;
	//}
	//std::cout << "bla" << std::endl;
}
