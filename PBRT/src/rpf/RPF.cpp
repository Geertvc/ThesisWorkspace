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
#include <math.h>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include "progressreporter.h"

RPF::RPF(){
	xRes = 0;
	yRes = 0;
	samplesPerPixel = 0;
	numberOfSceneFeatures = 6;
	epsilon = 0.00001f;
}

void RPF::applyFilter(std::vector<RPFPixel> &input, float *xyz, int xResolution, int yResolution, int samplesPerPixel){
	this->xRes = xResolution;
	this->yRes = yResolution;
	this->samplesPerPixel = samplesPerPixel;

	//copy the color of all samples to c'
	std::vector<Tuple3f> copyColors = *(new std::vector<Tuple3f> ());
	copyColors.reserve(xResolution*yResolution*samplesPerPixel);
	for (int y = 0; y < yResolution; ++y) {
		for (int x = 0; x < xResolution; ++x) {
			int pixelIndex = y*xResolution + x;
			for (int s = 0; s < samplesPerPixel; ++s) {
				float *colors = input[pixelIndex].rpfsamples[s].Lrgb;
				Tuple3f *tuple = new Tuple3f(colors[0], colors[1], colors[2]);
				copyColors.push_back(*tuple);
			}
		}
	}



	//filter the image 4 times with different neighboorhood sizes.
	//TODO change back into and loop back to 4 instead to 1: int box[] = {55, 35, 17, 7};
	int box[] = {7};
	for (int t = 0; t < 1; ++t) {
		int b = box[t];
		//Max number of samples
		int M = b*b*samplesPerPixel/2;

		ProgressReporter reporter(yRes, "Filtering using RPF");

		//Main forloop over all pixels.
		for (int y = 0; y < yRes; ++y) {
			for (int x = 0; x < xRes; ++x) {
				//pre-process samples
				std::vector<RPFSample> *outputNeighboorhood = new std::vector<RPFSample>();
				preProcessSamples(input, b, M, x, y, *outputNeighboorhood);
				// compute feature weights
				std::vector<float> alpha (3);
				std::vector<float> beta (numberOfSceneFeatures);
				computeFeatureWeights(t, *outputNeighboorhood, alpha, beta);
				//TODO filter color samples
				std::vector<Tuple3f> *newFilteredColors = new std::vector<Tuple3f> ();
				filterColorSamples(*outputNeighboorhood, alpha, beta, copyColors, *newFilteredColors);
			}
			reporter.Update();
		}
		reporter.Done();

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
			}
			xyz[3*pixelIndex    ] = totalPixelColor[0]*invSamplesPerPixel;
			xyz[3*pixelIndex + 1] = totalPixelColor[1]*invSamplesPerPixel;
			xyz[3*pixelIndex + 2] = totalPixelColor[2]*invSamplesPerPixel;
		}
	}
}

void RPF::preProcessSamples(std::vector<RPFPixel> &input, int b, float M, int x, int y, std::vector<RPFSample> &outputNeighboorhood){
	double sigma_p = b/4.0;
	//std::cout << "sigma_p: " << sigma_p << std::endl;
	//Add all the samples of the pixel P(x,y) to the outputNeighboorhood
	vector<RPFSample> pixelSamples = input[y*xRes + x].rpfsamples;
	for (int i = 0; i < samplesPerPixel; ++i) {
		outputNeighboorhood.push_back(pixelSamples[i]);
	}

	//compute mean and standard deviation of the features of samples in pixel P(x,y) for clustering
	//mean
	float meanNx = 0.f;
	float meanNy = 0.f;
	float meanNz = 0.f;
	float meanWx = 0.f;
	float meanWy = 0.f;
	float meanWz = 0.f;
	for (int i = 0; i < samplesPerPixel; ++i) {
		meanNx += outputNeighboorhood[i].nx;
		meanNy += outputNeighboorhood[i].ny;
		meanNz += outputNeighboorhood[i].nz;
		meanWx += outputNeighboorhood[i].wx;
		meanWy += outputNeighboorhood[i].wy;
		meanWz += outputNeighboorhood[i].wz;
	}
	meanNx = meanNx/samplesPerPixel;
	meanNy = meanNy/samplesPerPixel;
	meanNz = meanNz/samplesPerPixel;
	meanWx = meanWx/samplesPerPixel;
	meanWy = meanWy/samplesPerPixel;
	meanWz = meanWz/samplesPerPixel;
	//standard deviation = sqrt(sum(x_i-mean)/N)
	float sigmaNx = 0.f;
	float sigmaNy = 0.f;
	float sigmaNz = 0.f;
	float sigmaWx = 0.f;
	float sigmaWy = 0.f;
	float sigmaWz = 0.f;
	for (int i = 0; i < samplesPerPixel; ++i) {
		sigmaNx += pow(outputNeighboorhood[i].nx-meanNx,2);
		sigmaNy += pow(outputNeighboorhood[i].ny-meanNy,2);
		sigmaNz += pow(outputNeighboorhood[i].nz-meanNz,2);
		sigmaWx += pow(outputNeighboorhood[i].wx-meanWx,2);
		sigmaWy += pow(outputNeighboorhood[i].wy-meanWy,2);
		sigmaWz += pow(outputNeighboorhood[i].wz-meanWz,2);
	}
	sigmaNx = sqrt(sigmaNx/samplesPerPixel);
	sigmaNy = sqrt(sigmaNy/samplesPerPixel);
	sigmaNz = sqrt(sigmaNz/samplesPerPixel);
	sigmaWx = sqrt(sigmaWx/samplesPerPixel);
	sigmaWy = sqrt(sigmaWy/samplesPerPixel);
	sigmaWz = sqrt(sigmaWz/samplesPerPixel);

	//add samples to outputNeighboorhood
	//The number of extra samples to add is the max number M minus the # of samples from this pixel = samplesPerPixel
	int numberOfExtraSamples = M-samplesPerPixel;
	std::vector<int> *randomXCoords = new vector<int>();
	std::vector<int> *randomYCoords = new vector<int>();
	std::vector<int> *randomSampleCoords = new vector<int>();
	randomXCoords->reserve(numberOfExtraSamples);
	randomYCoords->reserve(numberOfExtraSamples);
	randomSampleCoords->reserve(numberOfExtraSamples);
	getRandom2DSamples(sigma_p, x, y, b, (b-1)/2, numberOfExtraSamples, input, *randomXCoords, *randomYCoords, *randomSampleCoords);
	double diff;
	for (int q = 0; q < numberOfExtraSamples; ++q) {
		//select a random sample from samples inside the box but outside the pixel P(x,y) with distribution based on sigma_p
		RPFSample *selectedSample = new RPFSample();
		//perform clustering
			//Nx
			diff = abs(selectedSample->nx - meanNx);
			if((diff > 3*sigmaNx) && (diff > 0.1 || sigmaNx > 0.1)){
				continue;
			}
			//Ny
			diff = abs(selectedSample->ny - meanNy);
			if((diff > 3*sigmaNy) && (diff > 0.1 || sigmaNy > 0.1)){
				continue;
			}
			//Nz
			diff = abs(selectedSample->nz - meanNz);
			if((diff > 3*sigmaNz) && (diff > 0.1 || sigmaNz > 0.1)){
				continue;
			}
			//Wx
			diff = abs(selectedSample->wx - meanWx);
			if((diff > 30*sigmaWx) && (diff > 0.1 || sigmaWx > 0.1)){
				continue;
			}
			//Wy
			diff = abs(selectedSample->wy - meanWy);
			if((diff > 30*sigmaWy) && (diff > 0.1 || sigmaWy > 0.1)){
				continue;
			}
			//Wz
			diff = abs(selectedSample->wz - meanWz);
			if((diff > 30*sigmaWz) && (diff > 0.1 || sigmaWz > 0.1)){
				continue;
			}
		//If the execution reaches this statement, add the sample.
		outputNeighboorhood.push_back(*selectedSample);
	}
	int actualAddedSamples = outputNeighboorhood.size();
	//Compute mean and standard deviation of samples in neighboorhood.
	//mean
	meanNx = 0.f;
	meanNy = 0.f;
	meanNz = 0.f;
	meanWx = 0.f;
	meanWy = 0.f;
	meanWz = 0.f;
	for (int i = 0; i < actualAddedSamples; ++i) {
		meanNx += outputNeighboorhood[i].nx;
		meanNy += outputNeighboorhood[i].ny;
		meanNz += outputNeighboorhood[i].nz;
		meanWx += outputNeighboorhood[i].wx;
		meanWy += outputNeighboorhood[i].wy;
		meanWz += outputNeighboorhood[i].wz;
	}
	meanNx = meanNx/actualAddedSamples;
	meanNy = meanNy/actualAddedSamples;
	meanNz = meanNz/actualAddedSamples;
	meanWx = meanWx/actualAddedSamples;
	meanWy = meanWy/actualAddedSamples;
	meanWz = meanWz/actualAddedSamples;
	//standard deviation = sqrt(sum(x_i-mean)/N)
	sigmaNx = 0.f;
	sigmaNy = 0.f;
	sigmaNz = 0.f;
	sigmaWx = 0.f;
	sigmaWy = 0.f;
	sigmaWz = 0.f;
	for (int i = 0; i < actualAddedSamples; ++i) {
		sigmaNx += pow(outputNeighboorhood[i].nx-meanNx,2);
		sigmaNy += pow(outputNeighboorhood[i].ny-meanNy,2);
		sigmaNz += pow(outputNeighboorhood[i].nz-meanNz,2);
		sigmaWx += pow(outputNeighboorhood[i].wx-meanWx,2);
		sigmaWy += pow(outputNeighboorhood[i].wy-meanWy,2);
		sigmaWz += pow(outputNeighboorhood[i].wz-meanWz,2);
	}
	sigmaNx = sqrt(sigmaNx/actualAddedSamples);
	sigmaNy = sqrt(sigmaNy/actualAddedSamples);
	sigmaNz = sqrt(sigmaNz/actualAddedSamples);
	sigmaWx = sqrt(sigmaWx/actualAddedSamples);
	sigmaWy = sqrt(sigmaWy/actualAddedSamples);
	sigmaWz = sqrt(sigmaWz/actualAddedSamples);
	//Compute normalized vector for each sample by removing mean and dividing by standard deviation
	for (int i = 0; i < actualAddedSamples; ++i) {
		outputNeighboorhood[i].nx = (outputNeighboorhood[i].nx-meanNx)/sigmaNx;
		outputNeighboorhood[i].ny = (outputNeighboorhood[i].ny-meanNy)/sigmaNy;
		outputNeighboorhood[i].nz = (outputNeighboorhood[i].nz-meanNz)/sigmaNz;
		outputNeighboorhood[i].wx = (outputNeighboorhood[i].wx-meanWx)/sigmaWx;
		outputNeighboorhood[i].wy = (outputNeighboorhood[i].wy-meanWy)/sigmaWy;
		outputNeighboorhood[i].wz = (outputNeighboorhood[i].wz-meanWz)/sigmaWz;
	}
	//std::cout << "loop: " << x << ", " << y << std::endl;
}

void RPF::computeFeatureWeights(int t, std::vector<RPFSample> &outputNeighboorhood, std::vector<float> &alpha, std::vector<float> &beta){

	/********************************
	 * CREATE THE NORMALIZED ARRAYS *
	 ********************************/
	//!!!!! IF YOU ADD A NEW FEATURE DON'T FORGET TO ADJUST THE NUMBEROFSCENEFEATURE FIELD. !!!!!

	//Make arrays for all the mutual informations we have to calculate.
	unsigned int length = outputNeighboorhood.size();
	//Color channels
	std::vector<float> colorChan1 (length);
	std::vector<float> colorChan2 (length);
	std::vector<float> colorChan3 (length);
	//Random parameters
	std::vector<float> randomLensU (length);
	std::vector<float> randomLensV (length);
	std::vector<float> randomTime (length);
	//Position parameters
	std::vector<float> posX (length);
	std::vector<float> posY (length);
	//Scene features
	std::vector<float> featureNormalX (length);
	std::vector<float> featureNormalY (length);
	std::vector<float> featureNormalZ (length);
	std::vector<float> featureWorldPosX (length);
	std::vector<float> featureWorldPosY (length);
	std::vector<float> featureWorldPosZ (length);

	//Mean variables
	float meanCol1 = 0.f, meanCol2 = 0.f, meanCol3 = 0.f;
	float meanRanLensU = 0.f, meanRanLensV = 0.f, meanRanTime = 0.f;
	float meanPX = 0.f, meanPY = 0.f;
	float meanNx = 0.f, meanNy = 0.f, meanNz = 0.f;
	float meanWx = 0.f, meanWy = 0.f, meanWz = 0.f;

	//fill the arrays with the values of the neighboorhood samples
	for (unsigned int i = 0; i < length; ++i) {
		colorChan1[i] = outputNeighboorhood[i].Lrgb[0];
		colorChan2[i] = outputNeighboorhood[i].Lrgb[1];
		colorChan3[i] = outputNeighboorhood[i].Lrgb[2];

		randomLensU[i] = outputNeighboorhood[i].randomLensU;
		randomLensV[i] = outputNeighboorhood[i].randomLensV;
		randomTime[i] = outputNeighboorhood[i].randomTime;

		posX[i] = outputNeighboorhood[i].imageX;
		posY[i] = outputNeighboorhood[i].imageY;

		featureNormalX[i] = outputNeighboorhood[i].nx;
		featureNormalY[i] = outputNeighboorhood[i].ny;
		featureNormalZ[i] = outputNeighboorhood[i].nz;
		featureWorldPosX[i] = outputNeighboorhood[i].wx;
		featureWorldPosY[i] = outputNeighboorhood[i].wy;
		featureWorldPosZ[i] = outputNeighboorhood[i].wz;

		meanCol1 += colorChan1[i];
		meanCol2 += colorChan2[i];
		meanCol3 += colorChan3[i];

		meanRanLensU += randomLensU[i];
		meanRanLensV += randomLensV[i];
		meanRanTime += randomTime[i];

		meanPX += posX[i];
		meanPY += posY[i];

		meanNx += featureNormalX[i];
		meanNy += featureNormalY[i];
		meanNz += featureNormalZ[i];

		meanWx += featureWorldPosX[i];
		meanWy += featureWorldPosY[i];
		meanWz += featureWorldPosZ[i];
	}

	meanCol1 /= length;
	meanCol2 /= length;
	meanCol3 /= length;

	meanRanLensU /= length;
	meanRanLensV /= length;
	meanRanTime /= length;

	meanPX /= length;
	meanPY /= length;

	meanNx /= length;
	meanNy /= length;
	meanNz /= length;

	meanWx /= length;
	meanWy /= length;
	meanWz /= length;

	//standard deviation = sqrt(sum((x_i-mean)^2)/N)
	float sigmaCol1 = 0.f, sigmaCol2 = 0.f, sigmaCol3 = 0.f;
	float sigmaRanLensU = 0.f, sigmaRanLensV = 0.f, sigmaRanTime = 0.f;
	float sigmaPX = 0.f, sigmaPY = 0.f;
	float sigmaNx = 0.f, sigmaNy = 0.f, sigmaNz = 0.f;
	float sigmaWx = 0.f, sigmaWy = 0.f, sigmaWz = 0.f;

	for (unsigned int i = 0; i < length; ++i) {
		sigmaCol1 += pow((double) (colorChan1[i]-meanCol1),2);
		sigmaCol2 += pow((double) (colorChan2[i]-meanCol2),2);
		sigmaCol3 += pow((double) (colorChan3[i]-meanCol3),2);

		sigmaRanLensU += pow((double) (randomLensU[i]-meanRanLensU),2);
		sigmaRanLensV += pow((double) (randomLensV[i]-meanRanLensV),2);
		sigmaRanTime += pow((double) (randomTime[i]-meanRanTime),2);

		sigmaPX += pow((double) (posX[i]-meanPX),2);
		sigmaPY += pow((double) (posY[i]-meanPY),2);

		sigmaNx += pow((double) (featureNormalX[i]-meanNx),2);
		sigmaNy += pow((double) (featureNormalY[i]-meanNy),2);
		sigmaNz += pow((double) (featureNormalZ[i]-meanNz),2);

		sigmaWx += pow((double) (featureWorldPosX[i]-meanWx),2);
		sigmaWy += pow((double) (featureWorldPosY[i]-meanWy),2);
		sigmaWz += pow((double) (featureWorldPosZ[i]-meanWz),2);
	}
	sigmaCol1 = sqrt(sigmaCol1/length);
	sigmaCol2 = sqrt(sigmaCol2/length);
	sigmaCol3 = sqrt(sigmaCol3/length);

	sigmaRanLensU = sqrt(sigmaRanLensU/length);
	sigmaRanLensV = sqrt(sigmaRanLensV/length);
	sigmaRanTime = sqrt(sigmaRanTime/length);

	sigmaPX = sqrt(sigmaPX/length);
	sigmaPY = sqrt(sigmaPY/length);

	sigmaNx = sqrt(sigmaNx/length);
	sigmaNy = sqrt(sigmaNy/length);
	sigmaNz = sqrt(sigmaNz/length);

	sigmaWx = sqrt(sigmaWx/length);
	sigmaWy = sqrt(sigmaWy/length);
	sigmaWz = sqrt(sigmaWz/length);
	//Compute normalized vector for each sample by removing mean and dividing by standard deviation
	for (unsigned int i = 0; i < length; ++i) {
		colorChan1[i] = (colorChan1[i]-meanCol1)/sigmaCol1;
		colorChan2[i] = (colorChan2[i]-meanCol2)/sigmaCol2;
		colorChan3[i] = (colorChan3[i]-meanCol3)/sigmaCol3;

		randomLensU[i] = (randomLensU[i]-meanRanLensU)/sigmaRanLensU;
		randomLensV[i] = (randomLensV[i]-meanRanLensV)/sigmaRanLensV;
		randomTime[i] = (randomTime[i]-meanRanTime)/sigmaRanTime;

		posX[i] = (posX[i]-meanPX)/sigmaPX;
		posY[i] = (posY[i]-meanPY)/sigmaPY;

		featureNormalX[i] = (featureNormalX[i]-meanNx)/sigmaNx;
		featureNormalY[i] = (featureNormalY[i]-meanNy)/sigmaNy;
		featureNormalZ[i] = (featureNormalZ[i]-meanNz)/sigmaNz;

		featureWorldPosX[i] = (featureWorldPosX[i]-meanWx)/sigmaWx;
		featureWorldPosY[i] = (featureWorldPosY[i]-meanWy)/sigmaWy;
		featureWorldPosZ[i] = (featureWorldPosZ[i]-meanWz)/sigmaWz;
	}

	/***********************************
	 * COMPUTE DEPENDENCIES FOR COLORS *
	 ***********************************/

	//Chan 1
		//Random parameter 1: LensU
		float Dr1_c1 = calculateMutualInformation(colorChan1, randomLensU);
		//Random parameter 2: LensV
		float Dr2_c1 = calculateMutualInformation(colorChan1, randomLensV);
		//Random parameter 3: Time
		float Dr3_c1 = calculateMutualInformation(colorChan1, randomTime);

		//Calc total Dr_c1
		float Dr_c1 = Dr1_c1 + Dr2_c1 + Dr3_c1;

		//Position parameter 1: X
		float Dp1_c1 = calculateMutualInformation(colorChan1, posX);
		//Position parameter 2: Y
		float Dp2_c1 = calculateMutualInformation(colorChan1, posY);
		//Calc total Dp_c1
		float Dp_c1 = Dp1_c1 + Dp2_c1;

		//Scene feature 1: Normal X
		float Df1_c1 = calculateMutualInformation(colorChan1, featureNormalX);
		//Scene feature 2: Normal Y
		float Df2_c1 = calculateMutualInformation(colorChan1, featureNormalY);
		//Scene feature 3: Normal Z
		float Df3_c1 = calculateMutualInformation(colorChan1, featureNormalZ);
		//Scene feature 4: World Position X
		float Df4_c1 = calculateMutualInformation(colorChan1, featureWorldPosX);
		//Scene feature 5: World Position Y
		float Df5_c1 = calculateMutualInformation(colorChan1, featureWorldPosY);
		//Scene feature 6: World Position Z
		float Df6_c1 = calculateMutualInformation(colorChan1, featureWorldPosZ);
		//Calc total Df_c1
		float Df_c1 = Df1_c1 + Df2_c1 + Df3_c1 + Df4_c1 + Df5_c1 + Df6_c1;

		//Calculate Wr_c1 with Dr_c1 and Dp_c1
		float Wr_c1 = Dr_c1/(Dr_c1 + Dp_c1 + epsilon);

		//Calc alpha[0].
		float temp = 1-(2*(1+(0.1*t))*Wr_c1);
		alpha[0] = temp > 0.f ? temp : 0;

	//Chan 2
		//Random parameter 1: LensU
		float Dr1_c2 = calculateMutualInformation(colorChan2, randomLensU);
		//Random parameter 2: LensV
		float Dr2_c2 = calculateMutualInformation(colorChan2, randomLensV);
		//Random parameter 3: Time
		float Dr3_c2 = calculateMutualInformation(colorChan2, randomTime);

		//Calc total Dr_c1
		float Dr_c2 = Dr1_c2 + Dr2_c2 + Dr3_c2;

		//Position parameter 1: X
		float Dp1_c2 = calculateMutualInformation(colorChan2, posX);
		//Position parameter 2: Y
		float Dp2_c2 = calculateMutualInformation(colorChan2, posY);
		//Calc total Dp_c1
		float Dp_c2 = Dp1_c2 + Dp2_c2;

		//Scene feature 1: Normal X
		float Df1_c2 = calculateMutualInformation(colorChan2, featureNormalX);
		//Scene feature 2: Normal Y
		float Df2_c2 = calculateMutualInformation(colorChan2, featureNormalY);
		//Scene feature 3: Normal Z
		float Df3_c2 = calculateMutualInformation(colorChan2, featureNormalZ);
		//Scene feature 4: World Position X
		float Df4_c2 = calculateMutualInformation(colorChan2, featureWorldPosX);
		//Scene feature 5: World Position Y
		float Df5_c2 = calculateMutualInformation(colorChan2, featureWorldPosY);
		//Scene feature 6: World Position Z
		float Df6_c2 = calculateMutualInformation(colorChan2, featureWorldPosZ);
		//Calc total Df_c1
		float Df_c2 = Df1_c2 + Df2_c2 + Df3_c2 + Df4_c2 + Df5_c2 + Df6_c2;

		//Calculate Wr_c1 with Dr_c1 and Dp_c1
		float Wr_c2 = Dr_c2/(Dr_c2 + Dp_c2 + epsilon);

		//Calc alpha[1].
		temp = 1-(2*(1+(0.1*t))*Wr_c2);
		alpha[1] = temp > 0.f ? temp : 0;

	//Chan 3
		//Random parameter 1: LensU
		float Dr1_c3 = calculateMutualInformation(colorChan3, randomLensU);
		//Random parameter 2: LensV
		float Dr2_c3 = calculateMutualInformation(colorChan3, randomLensV);
		//Random parameter 3: Time
		float Dr3_c3 = calculateMutualInformation(colorChan3, randomTime);

		//Calc total Dr_c1
		float Dr_c3 = Dr1_c3 + Dr2_c3 + Dr3_c3;

		//Position parameter 1: X
		float Dp1_c3 = calculateMutualInformation(colorChan3, posX);
		//Position parameter 2: Y
		float Dp2_c3 = calculateMutualInformation(colorChan3, posY);
		//Calc total Dp_c1
		float Dp_c3 = Dp1_c3 + Dp2_c3;

		//Scene feature 1: Normal X
		float Df1_c3 = calculateMutualInformation(colorChan3, featureNormalX);
		//Scene feature 2: Normal Y
		float Df2_c3 = calculateMutualInformation(colorChan3, featureNormalY);
		//Scene feature 3: Normal Z
		float Df3_c3 = calculateMutualInformation(colorChan3, featureNormalZ);
		//Scene feature 4: World Position X
		float Df4_c3 = calculateMutualInformation(colorChan3, featureWorldPosX);
		//Scene feature 5: World Position Y
		float Df5_c3 = calculateMutualInformation(colorChan3, featureWorldPosY);
		//Scene feature 6: World Position Z
		float Df6_c3 = calculateMutualInformation(colorChan3, featureWorldPosZ);
		//Calc total Df_c1
		float Df_c3 = Df1_c3 + Df2_c3 + Df3_c3 + Df4_c3 + Df5_c3 + Df6_c3;

		//Calculate Wr_c1 with Dr_c1 and Dp_c1
		float Wr_c3 = Dr_c3/(Dr_c3 + Dp_c3 + epsilon);

		//Calc alpha[2].
		temp = 1-(2*(1+(0.1*t))*Wr_c3);
		alpha[2] = temp > 0.f ? temp : 0;

	//calc Dr, Dp and Df by adding up the Dr_chan Dp_chan and Df_chan over the channels
	float Dr_c = Dr_c1 + Dr_c2 + Dr_c3;
	float Dp_c = Dp_c1 + Dp_c2 + Dp_c3;
	float Df_c = Df_c1 + Df_c2 + Df_c3;

	/*******************************************
	 * COMPUTE DEPENDENCIES FOR SCENE FEATURES *
	 *******************************************/

	//Feature 1: Normal X
		//Random parameter 1: LensU
		float Dr1_f1 = calculateMutualInformation(featureNormalX, randomLensU);
		//Random parameter 2: LensV
		float Dr2_f1 = calculateMutualInformation(featureNormalX, randomLensV);
		//Random parameter 3: Time
		float Dr3_f1 = calculateMutualInformation(featureNormalX, randomTime);

		//Calc total Dr_f1
		float Dr_f1 = Dr1_f1 + Dr2_f1 + Dr3_f1;

		//Position parameter 1: X
		float Dp1_f1 = calculateMutualInformation(featureNormalX, posX);
		//Position parameter 2: Y
		float Dp2_f1 = calculateMutualInformation(featureNormalX, posY);
		//Calc total Dp_c1
		float Dp_f1 = Dp1_f1 + Dp2_f1;

		//Calc Dfk_c
		float Df1_c = Df1_c1 + Df1_c2 + Df1_c3;

		//Calc Wr_fk
		float Wr_f1 = Dr_f1/(Dr_f1 + Dp_f1 + epsilon);

		//Calc Wfk_c
		float Wf1_c = Df1_c == 0.f ? 0.f : Df1_c/(Dr_c + Dp_c + Df_c);

		//Calc beta[0]
		temp = 1-((1+(0.1*t))*Wr_f1);
		beta[0] = temp > 0.f ? temp*Wf1_c : 0;

	//Feature 2: Normal Y
		//Random parameter 1: LensU
		float Dr1_f2 = calculateMutualInformation(featureNormalY, randomLensU);
		//Random parameter 2: LensV
		float Dr2_f2 = calculateMutualInformation(featureNormalY, randomLensV);
		//Random parameter 3: Time
		float Dr3_f2 = calculateMutualInformation(featureNormalY, randomTime);

		//Calc total Dr_f1
		float Dr_f2 = Dr1_f2 + Dr2_f2 + Dr3_f2;

		//Position parameter 1: X
		float Dp1_f2 = calculateMutualInformation(featureNormalY, posX);
		//Position parameter 2: Y
		float Dp2_f2 = calculateMutualInformation(featureNormalY, posY);
		//Calc total Dp_c1
		float Dp_f2 = Dp1_f2 + Dp2_f2;

		//Calc Dfk_c
		float Df2_c = Df2_c1 + Df2_c2 + Df2_c3;

		//Calc Wr_fk
		float Wr_f2 = Dr_f2/(Dr_f2 + Dp_f2 + epsilon);

		//Calc Wfk_c
		float Wf2_c = Df2_c == 0.f ? 0.f : Df2_c/(Dr_c + Dp_c + Df_c);

		//Calc beta[1]
		temp = 1-((1+(0.1*t))*Wr_f2);
		beta[1] = temp > 0.f ? temp*Wf2_c : 0;

	//Feature 3: Normal Z
		//Random parameter 1: LensU
		float Dr1_f3 = calculateMutualInformation(featureNormalZ, randomLensU);
		//Random parameter 2: LensV
		float Dr2_f3 = calculateMutualInformation(featureNormalZ, randomLensV);
		//Random parameter 3: Time
		float Dr3_f3 = calculateMutualInformation(featureNormalZ, randomTime);

		//Calc total Dr_f1
		float Dr_f3 = Dr1_f3 + Dr2_f3 + Dr3_f3;

		//Position parameter 1: X
		float Dp1_f3 = calculateMutualInformation(featureNormalZ, posX);
		//Position parameter 2: Y
		float Dp2_f3 = calculateMutualInformation(featureNormalZ, posY);
		//Calc total Dp_c1
		float Dp_f3 = Dp1_f3 + Dp2_f3;

		//Calc Dfk_c
		float Df3_c = Df3_c1 + Df3_c2 + Df3_c3;

		//Calc Wr_fk
		float Wr_f3 = Dr_f3/(Dr_f3 + Dp_f3 + epsilon);

		//Calc Wfk_c
		float Wf3_c = Df3_c == 0.f ? 0.f : Df3_c/(Dr_c + Dp_c + Df_c);

		//Calc beta[2]
		temp = 1-((1+(0.1*t))*Wr_f3);
		beta[2] = temp > 0.f ? temp*Wf3_c : 0;

	//Feature 4: World position X
		//Random parameter 1: LensU
		float Dr1_f4 = calculateMutualInformation(featureWorldPosX, randomLensU);
		//Random parameter 2: LensV
		float Dr2_f4 = calculateMutualInformation(featureWorldPosX, randomLensV);
		//Random parameter 3: Time
		float Dr3_f4 = calculateMutualInformation(featureWorldPosX, randomTime);

		//Calc total Dr_f1
		float Dr_f4 = Dr1_f4 + Dr2_f4 + Dr3_f4;

		//Position parameter 1: X
		float Dp1_f4 = calculateMutualInformation(featureWorldPosX, posX);
		//Position parameter 2: Y
		float Dp2_f4 = calculateMutualInformation(featureWorldPosX, posY);
		//Calc total Dp_c1
		float Dp_f4 = Dp1_f4 + Dp2_f4;

		//Calc Dfk_c
		float Df4_c = Df4_c1 + Df4_c2 + Df4_c3;

		//Calc Wr_fk
		float Wr_f4 = Dr_f4/(Dr_f4 + Dp_f4 + epsilon);

		//Calc Wfk_c
		float Wf4_c = Df4_c == 0.f ? 0.f : Df4_c/(Dr_c + Dp_c + Df_c);

		//Calc beta[3]
		temp = 1-((1+(0.1*t))*Wr_f4);
		beta[3] = temp > 0.f ? temp*Wf4_c : 0;

	//Feature 5: World position Y
		//Random parameter 1: LensU
		float Dr1_f5 = calculateMutualInformation(featureWorldPosY, randomLensU);
		//Random parameter 2: LensV
		float Dr2_f5 = calculateMutualInformation(featureWorldPosY, randomLensV);
		//Random parameter 3: Time
		float Dr3_f5 = calculateMutualInformation(featureWorldPosY, randomTime);

		//Calc total Dr_f1
		float Dr_f5 = Dr1_f5 + Dr2_f5 + Dr3_f5;

		//Position parameter 1: X
		float Dp1_f5 = calculateMutualInformation(featureWorldPosY, posX);
		//Position parameter 2: Y
		float Dp2_f5 = calculateMutualInformation(featureWorldPosY, posY);
		//Calc total Dp_c1
		float Dp_f5 = Dp1_f5 + Dp2_f5;

		//Calc Dfk_c
		float Df5_c = Df5_c1 + Df5_c2 + Df5_c3;

		//Calc Wr_fk
		float Wr_f5 = Dr_f5/(Dr_f5 + Dp_f5 + epsilon);

		//Calc Wfk_c
		float Wf5_c = Df5_c == 0.f ? 0.f : Df5_c/(Dr_c + Dp_c + Df_c);

		//Calc beta[4]
		temp = 1-((1+(0.1*t))*Wr_f5);
		beta[4] = temp > 0.f ? temp*Wf5_c : 0;

	//Feature 6: World position Z
		//Random parameter 1: LensU
		float Dr1_f6 = calculateMutualInformation(featureWorldPosZ, randomLensU);
		//Random parameter 2: LensV
		float Dr2_f6 = calculateMutualInformation(featureWorldPosZ, randomLensV);
		//Random parameter 3: Time
		float Dr3_f6 = calculateMutualInformation(featureWorldPosZ, randomTime);

		//Calc total Dr_f1
		float Dr_f6 = Dr1_f6 + Dr2_f6 + Dr3_f6;

		//Position parameter 1: X
		float Dp1_f6 = calculateMutualInformation(featureWorldPosZ, posX);
		//Position parameter 2: Y
		float Dp2_f6 = calculateMutualInformation(featureWorldPosZ, posY);
		//Calc total Dp_c1
		float Dp_f6 = Dp1_f6 + Dp2_f6;

		//Calc Dfk_c
		float Df6_c = Df6_c1 + Df6_c2 + Df6_c3;

		//Calc Wr_fk
		float Wr_f6 = Dr_f6/(Dr_f6 + Dp_f6 + epsilon);

		//Calc Wfk_c
		float Wf6_c = Df6_c == 0.f ? 0.f : Df6_c/(Dr_c + Dp_c + Df_c);

		//Calc beta[5]
		temp = 1-((1+(0.1*t))*Wr_f6);
		beta[5] = temp > 0.f ? temp*Wf6_c : 0;

	/*std::cout << "alpha[0]: " << alpha[0] << std::endl;
	std::cout << "alpha[1]: " << alpha[1] << std::endl;
	std::cout << "alpha[2]: " << alpha[2] << std::endl;
	std::cout << "beta[0]: " << beta[0] << std::endl;
	std::cout << "beta[1]: " << beta[1] << std::endl;
	std::cout << "beta[2]: " << beta[2] << std::endl;
	std::cout << "beta[3]: " << beta[3] << std::endl;
	std::cout << "beta[4]: " << beta[4] << std::endl;
	std::cout << "beta[5]: " << beta[5] << std::endl;*/
}
void RPF::filterColorSamples(std::vector<RPFSample> &outputNeighboorhood, std::vector<float> &alpha, std::vector<float> &beta, std::vector<Tuple3f> &copyColors, std::vector<Tuple3f> &newFilteredColors){

}

/*******************************************
 * Sample 2d area with normal distribution *
 *******************************************/

void RPF::getRandom2DSamples(double sigma_p, int baseX, int baseY, int boxSize, int n, int numberOfSamples,
			std::vector<RPFPixel> &input,
			std::vector<int> &outputXCoords, std::vector<int> &outputYCoords, std::vector<int> &outputSampleCoords){
	//Calculate the chances of each sample within the box.
	std::vector<double> normalChances = *(new vector<double>());
	normalChances.reserve(boxSize*boxSize*samplesPerPixel);
	for (int j = baseY-n; j < baseY+n+1; ++j) {
		if(j < 0){
			continue;
		} else if(j > yRes-1){
			break;
		}
		//std::cout << "j: " << j << std::endl;
		for (int i = baseX-n; i < baseX+n+1; ++i) {
			if(i < 0){
				continue;
			} else if(i > xRes-1){
				break;
			}
			//std::cout << "i: " << i << std::endl;
			int pixelIndex = j*xRes +i;
			//laat de pixel x,y zelf weg
			if(j == baseY && i == baseX){
				for (int s = 0; s < samplesPerPixel; ++s) {
					normalChances.push_back(0.0);
					//cout << "before: x: " << i << ", y: " << j << ", s: " << s << ": " << 0.0 << endl;
				}
				continue;
			}
			vector<RPFSample> pixelSamples = input[pixelIndex].rpfsamples;
			for (int s = 0; s < samplesPerPixel; ++s) {
				double  chance = calcNormalChance(pixelSamples[s].imageX, pixelSamples[s].imageY, (double)baseX+0.5, (double)baseY+0.5, sigma_p);
				normalChances.push_back(chance);
				//cout << "call with: xCoord: " << xCoords[index] << ", ycoord: " << yCoords[index] << ", xMean: " << (double)x+0.5 << ", yMean: " << (double)y+0.5 << ", sigma: " << sigma_p << endl;
				//std::cout << "added: (x: " << i << ", y: " << j << ", s: " << s << ": " << chance << ")" << std::endl;
				//std::cout << "image: (x: " << input[pixelIndex].rpfsamples[s].imageX << ", y: " << input[pixelIndex].rpfsamples[s].imageY << std::endl;
			}
		}
	}
	//std::cout << normalChances.size()<< std::endl;
	//std::cout << "eerste total: " << calcTotalVector(normalChances) << std::endl;
	//std::cout << "chances size: " << normalChances.size() << std::endl;
	//Normalize the chances because now they dont add up to 1 (it is a contiuous domain and the middle pixel is left out).
	normalizeChancesVector(normalChances, calcTotalVector(normalChances));

	boost::mt19937 rng (std::time(0));
	boost::uniform_real<float> u(0.0f, 1.0f);
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);

	std::vector<int> randomSamples = *(new vector<int>());
	randomSamples.reserve(numberOfSamples);
	double chanceOfLastSelectedSample = 0.0;
	for (int i = 0; i < numberOfSamples; ++i) {
		double total = 1.0 - chanceOfLastSelectedSample;
		if(total <= 0.000000001){
			break;
		}
		normalizeChancesVector(normalChances, total);
		//std::cout << "total: " << calcTotalVector(normalChances) << std::endl;
		double randomNumber = gen();
		//std::cout << "random: " << randomNumber << std::endl;
		int selectedSample = findRandomSample(normalChances, randomNumber);
		randomSamples.push_back(selectedSample);
		chanceOfLastSelectedSample = normalChances[selectedSample];
		normalChances[selectedSample] = 0.0;
	}

	//Transform the index of the selected samples into an x, y and sample coordinate.
	int xSize = boxSize;
	int startX = baseX-n;
	int startY = baseY-n;
	if((baseX-n) < 0){
		xSize = baseX+n;
		startX = 0;
	} else if((baseX+n) > (xRes-1)){
		xSize = xRes -baseX + n;
	}
	if((baseY-n) < 0){
		startY = 0;
	}
	//std::cout << "x-n: " << (baseX-n) << std::endl;
	//std::cout << "xSize: " << xSize << std::endl;
	//std::cout << "startX: " << startX << std::endl;
	//std::cout << "startY: " << startY << std::endl;
	for (unsigned int i = 0; i < randomSamples.size(); ++i) {
		int index = randomSamples[i];
		int s = index%samplesPerPixel;
		int i = (index/samplesPerPixel)%xSize;
		int j = (index/(xSize*samplesPerPixel));
		outputXCoords.push_back(i + startX);
		outputYCoords.push_back(j + startY);
		outputSampleCoords.push_back(s);
	}
}

double RPF::calcNormalChance(float xCoord, float yCoord, double xMean, double yMean, double sigma){
	//cout<< "sigma: " << sigma << endl;
	//cout<< "pi: " << M_PI << endl;

	//cout<< "multConst: " << multConst << endl;
	double twoSigmaSquared = 2*sigma*sigma;
	double multConst = 1/(M_PI*twoSigmaSquared);
	//cout<< "twoSigmaSquared: " << twoSigmaSquared << endl;
	double chance = multConst*exp(-(pow(xCoord-xMean,2)+pow(yCoord-yMean,2))/twoSigmaSquared);
	//cout<< "chance: " << chance << endl;
	return chance;
}

void RPF::normalizeChancesVector(std::vector<double> &chances, double totalChance){
	for (unsigned int i = 0; i < chances.size(); ++i) {
		chances[i] = chances[i]/totalChance;
	}
}

double RPF::calcTotalVector(std::vector<double> &chances){
	double total = 0.0;
	for (unsigned int i = 0; i < chances.size(); ++i) {
		total += chances[i];
	}
	return total;
}

int RPF::findRandomSample(std::vector<double> &chances, double randomNumber){
	double totalSoFar = 0.0;
	for (unsigned int i = 0; i < chances.size(); ++i) {
		totalSoFar += chances[i];
		if(randomNumber < totalSoFar){
			//cout << "found: " << i << " totalsofar: " << totalSoFar << endl;
			return i;
		}
	}
	return -1;
}

/**********************************
 * Mutual Information Calculation *
 **********************************/

int RPF::quantizeAndPositiveVector(std::vector<float> &vect, std::vector<int> &output){
	if(vect.size() <= 0){
		std::cout << "Trying to get min of empty vector" << std::endl;
	}
	int min, max, temp;
	if(vect[0]>0){
		min = max = int(vect[0]+0.5f);
	} else{
		min = max = int(vect[0]-0.5f);
	}
	for (unsigned int i = 0; i < vect.size(); ++i) {
		temp = (vect[i]>0)?(int)(vect[i]+0.5f):(int)(vect[i]-0.5f);
		min = (min < temp) ? min: temp;
		max = (max > temp) ? max: temp;
		//std::cout << "temp: " << temp << std::endl;
		output.push_back(temp);
	}
	//std::cout << "min: " << min << std::endl;
	//std::cout << "max: " << max << std::endl;
	for (unsigned int i = 0; i < output.size(); ++i) {
		output[i] -= min;
	}

	return (max-min+1);
}

void RPF::getHistogram(std::vector<int> &Xbins, int xNumberOfStates, std::vector<float> &histX){
	for (int i = 0; i < xNumberOfStates; ++i) {
		histX.push_back(0.f);
	}
	unsigned int length = Xbins.size();
	for (unsigned int i = 0; i < length; ++i) {
		histX[Xbins[i]] += 1.f;
	}
	//Return probabilities instead of counts
	for (unsigned int i = 0; i < histX.size(); ++i) {
		histX[i] /= length;
	}
}

void RPF::getJointHistogram(std::vector<int> &Xbins, int xNumberOfStates, std::vector<int> &Ybins, int yNumberOfStates, std::vector<std::vector<float> > &histXY){
	//initialize the 2D histogram with zeros
	for (int i = 0; i < yNumberOfStates; ++i) {
		std::vector<float> row;
		for (int j = 0; j < xNumberOfStates; ++j) {
			row.push_back(0.f);
		}
		histXY.push_back(row);
	}
	//fill the histogram with the number of times an int exists.
	unsigned int length = Xbins.size();
	for (unsigned int i = 0; i < length; ++i) {
		histXY[Ybins[i]][Xbins[i]] += 1;
	}
	for (int i = 0; i < yNumberOfStates; ++i) {
		for (int j = 0; j < xNumberOfStates; ++j) {
			histXY[i][j] /= length;
		}
	}
}

float RPF::calculateMutualInformation(std::vector<float> &X, std::vector<float> &Y){
	//Mu(X,Y) = sum(X(x),sum(Y(y), p(x,y)*log(p(x,y)/(p(x)*p(y)))) )

	if(X.size() != Y.size()){
		std::cout << "calculateMutualInformation called with vectors of different length" << std::endl;
		return -1.f;
	}
	// Calculate pX and pY for calculating p(x) and p(y).
	std::vector<int> *Xbins = new std::vector<int>();
	std::vector<int> *Ybins = new std::vector<int>();
	int nStatesX = quantizeAndPositiveVector(X, *Xbins);
	int nStatesY = quantizeAndPositiveVector(Y, *Ybins);


	/*for (unsigned int x = 0; x < X.size(); ++x) {
		std::cout << "positive x: " << (*Xbins)[x] << std::endl;
	}
	for (unsigned int y = 0; y < Y.size(); ++y) {
		std::cout << "positive y: " << (*Ybins)[y] << std::endl;
	}
	std::cout << "nStatesX: " << nStatesX << std::endl;
	std::cout << "nStatesY: " << nStatesY << std::endl;*/

	std::vector<float> *histX = new std::vector<float>();
	getHistogram(*Xbins, nStatesX, *histX);
	std::vector<float> *histY = new std::vector<float>();
	getHistogram(*Ybins, nStatesY, *histY);

	/*for (unsigned int i = 0; i < (*histX).size(); ++i) {
		std::cout << "histX " << i << ": " << (*histX)[i] << std::endl;
	}
	for (unsigned int i = 0; i < (*histY).size(); ++i) {
		std::cout << "histY " << i << ": " << (*histY)[i] << std::endl;
	}*/

	//Calculate pXY for calculating p(x,y).
	std::vector<std::vector<float> > *histXY = new std::vector<std::vector<float> >();
	getJointHistogram(*Xbins, nStatesX, *Ybins, nStatesY, *histXY);

	/*for (int i = 0; i < nStatesY; ++i) {
		for (int j = 0; j < nStatesX; ++j) {
			std::cout << "histXY (" << i << ", " << j << "): " << (*histXY)[i][j] << std::endl;
		}
	}*/

	//Calculate Mu(X,Y)
	float muXY = 0.f;
	for (int i = 0; i < nStatesY; ++i) {
		for (int j = 0; j < nStatesX; ++j) {
			if((*histXY)[i][j] !=0.f && (*histX)[j] != 0.f && (*histY)[i]){
				muXY += (*histXY)[i][j]*log((*histXY)[i][j]/((*histX)[j]*(*histY)[i]));
			}
		}
	}
	muXY /= log(2);
	return muXY;
}


