/*
 * RPFCollector.h
 *
 *  Created on: Dec 6, 2012
 *      Author: thypo
 */

#ifndef RPFCOLLECTOR_H_
#define RPFCOLLECTOR_H_

#include "spectrum.h"
#include "sampler.h"
#include "memory.h"
#include "RPFPixel.h"
#include <stdlib.h>

#include <iostream>

class RPFCollector {
public:
	RPFCollector(const int xRes, const int yRes, const int nbOfsamplesPerPixel);
	//void setColorValues(float *values);
	//float *getColorValues();
	void AddSample(const CameraSample &sample, const Spectrum &L);
	~RPFCollector(){
		//delete[] colorValues;
		//delete rpfpixels;
		free(rpfpixels);
	}
private:
	const int xRes;
	const int yRes;
	const int nbPixels;
	//float *colorValues;
	static int samplesPerPixel;
	/*struct RPFSample{
		RPFSample(){
			int i = 0;
			for (i = 0; i < 3; ++i) {
				Lrgb[i] = 0.f;
			}
			//Lrgb[0] = 1.f;
			//Lrgb[1] = 2.f;
			//Lrgb[2] = 3.f;
		}
		float Lrgb[3];
		//void setRGB(float rgb[]){
		//	Lrgb[0] = rgb[0];
		//	Lrgb[1] = rgb[1];
		//	Lrgb[2] = rgb[2];
		//}
	};*/
	/*struct RPFPixel{
		RPFPixel() {
			rpfsamples = new RPFSample[samplesPerPixel];
			for (int i = 0; i < samplesPerPixel; ++i){
				new (&rpfsamples[i]) RPFSample();
			}
			nextSample = 0;
		}
		void AddSample(RPFSample sample){
			Assert(nextSample <= samplesPerPixel);
			//std::cout << nextSample << std::endl;
			//rpfsamples[nextSample] = sample;
			nextSample++;
			if(nextSample > 1){
				std::cout << nextSample;
			}
		}
		RPFSample *rpfsamples;
		int nextSample;

	};*/
	//BlockedArray<RPFPixel> *rpfpixels;
	RPFPixel *rpfpixels;
};


#endif /* RPFCOLLECTOR_H_ */
