/*
 * RPF.cpp
 *
 *  Created on: Jan 7, 2013
 *      Author: thypo
 */

#include "RPF.h"
#include "RPFPixel.h"
#include "Tuple3f.h"
#include <iostream>

RPF::RPF(){

}

void RPF::applyFilter(std::vector<RPFPixel> &input, float *xyz, int xResolution, int yResolution, int samplesPerPixel){
	//copy the color of all samples to c'
	std::vector<Tuple3f> copyColors = *(new std::vector<Tuple3f> ());
	copyColors.reserve(xResolution*yResolution*samplesPerPixel);
	for (int y = 0; y < yResolution; ++y) {
		for (int x = 0; x < xResolution; ++x) {
			int pixelIndex = y*xResolution + x;
			for (int s = 0; s < samplesPerPixel; ++s) {
				//int index = (pixelIndex)*samplesPerPixel + s;
				float *colors = input[pixelIndex].rpfsamples[s].Lrgb;

				/*if(isinf(colors[0])){
					std::cout << "0 is: " << colors[0] << std::endl;
				}
				if(isinf(colors[1])){
					std::cout << "1 is: " << colors[1] << std::endl;
				}
				if(isinf(colors[2])){
					std::cout << "2 is: " << colors[2] << std::endl;
				}*/

				/*if(y< 10 ){
					std::cout << colors[0]  << ", " << colors[1] << ", " << colors[2]  << std::endl;
				}*/

				/*std::vector<float> row;
				row.push_back(colors[0]);
				row.push_back(colors[1]);
				row.push_back(colors[2]);*/
				Tuple3f *tuple = new Tuple3f(colors[0], colors[1], colors[2]);
				/*if(y< 10 ){
					std::cout << "tuple: " << tuple->x << ", " << tuple->y << ", " << tuple->z  << std::endl;
				}*/
				copyColors.push_back(*tuple);
				//copyColors[index][0] = colors[0];
				//copyColors[index][1] = colors[1];
				//copyColors[index][2] = colors[2];
			}
		}
	}

	//TODO
	//filter the image 4 times with different neighboorhood sizes.
	int box[] = {55, 35, 17, 7};
	for (int t = 0; t < 4; ++t) {
		int b = box[t];
		//Max number of samples
		int M = b*b*samplesPerPixel/2;
		//Main forloop over all pixels.
		for (int y = 0; y < yResolution; ++y) {
			for (int x = 0; x < xResolution; ++x) {
				//TODO pre-process samples
				//TODO compute feature weights
				//TODO filter color samples
			}
		}

		//TODO copy the new filtered colors c'' to c'
	}

	float invSamplesPerPixel = 1.f/samplesPerPixel;
	for (int y = 0; y < yResolution; ++y) {
		for (int x = 0; x < xResolution; ++x) {
			int pixelIndex = y*xResolution + x;
			float *totalPixelColor = new float[3];
			for (int s = 0; s < samplesPerPixel; ++s) {
				int index = pixelIndex*samplesPerPixel + s;
				Tuple3f sampleColorValue = copyColors[index];
				totalPixelColor[0] += sampleColorValue.x;
				totalPixelColor[1] += sampleColorValue.y;
				totalPixelColor[2] += sampleColorValue.z;
				/*if(y< 10 ){
					std::cout << sampleColorValue.x  << ", " << sampleColorValue.y << ", " << sampleColorValue.z  << std::endl;
				}*/
			}
			xyz[3*pixelIndex    ] = totalPixelColor[0]*invSamplesPerPixel;
			xyz[3*pixelIndex + 1] = totalPixelColor[1]*invSamplesPerPixel;
			xyz[3*pixelIndex + 2] = totalPixelColor[2]*invSamplesPerPixel;

			/*if(y< 10 ){
				std::cout << xyz[3*pixelIndex    ]  << ", " << xyz[3*pixelIndex + 1] << ", " << xyz[3*pixelIndex + 2]  << std::endl;
			}*/
		}
	}
}


