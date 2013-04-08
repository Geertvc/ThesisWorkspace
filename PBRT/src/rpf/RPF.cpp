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
#include <cmath>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include "progressreporter.h"


RPF::RPF(RPFOptions &rpfOptions){
	//These values will be set when running applyFilter
	xRes = 0;
	yRes = 0;
	samplesPerPixel = 0;

	/*********************
	 * Parameters of RPF *
	 *********************/

	//Scene features
	numberOfSceneFeatures = 6;
	//Values for sample comparison with sigma
	normalSigmaFactor = 3;
	worldCoordinatesSigmaFactor = 30;

	//Epsilon
	epsilon = 0.00001f;
	//Sigma8Squared is 0.02 for noisy scenes and 0.002 for all others
	sigma8Squared = rpfOptions.sigma8Squared;

	normalFeature = rpfOptions.normalFeature;
	worldCoordFeature = rpfOptions.worldCoordFeature;

	printf("normalFeature set to  %s \n",(rpfOptions.normalFeature)?"true":"false");
	printf("worldCoordFeature set to  %s \n",(rpfOptions.worldCoordFeature)?"true":"false");
	printf("sigma8Squared set to  %f \n", rpfOptions.sigma8Squared);

	//The filtering stages.
	//TODO change back into and loop back to 4 instead to 1: {55, 35, 17, 7};
	//box.push_back(55);
	//box.push_back(35);
	//box.push_back(17);
	//box.push_back(7);

	box = rpfOptions.iterations;

	if(box.size() == 0){
		printf("number of iterations set to  1 as default \n");
		box.push_back(7);
	} else{
		unsigned int u = rpfOptions.iterations.size();
		printf("number of iterations set to  %u \n", u);
		for (unsigned int var = 0; var < rpfOptions.iterations.size(); ++var) {
			printf("iterations %u set to  %d \n", var, rpfOptions.iterations[var]);
		}
	}
}

void RPF::applyFilter(std::vector<RPFPixel> &input, float *xyz, int xResolution, int yResolution, int samplesPerPixel){
	this->xRes = xResolution;
	this->yRes = yResolution;
	this->samplesPerPixel = samplesPerPixel;

	long int numberOfSamples = xRes*yRes*samplesPerPixel;

	//copy the color of all samples to c'
	std::vector<Tuple3f> copyColors (numberOfSamples);
	for (int y = 0; y < yRes; ++y) {
		for (int x = 0; x < xRes; ++x) {
			int pixelIndex = y*xRes + x;
			for (int s = 0; s < samplesPerPixel; ++s) {
				float *colors = input[pixelIndex].rpfsamples[s].Lrgb;
				Tuple3f *tuple = new Tuple3f(colors[0], colors[1], colors[2]);
				Assert(((pixelIndex*samplesPerPixel) + s) < numberOfSamples);
				copyColors[(pixelIndex*samplesPerPixel) + s] = *tuple;
				delete tuple;
			}
		}
	}

	/*std::cout << "scenefeatures" << numberOfSceneFeatures << std::endl;
	std::cout << "epsilon" << epsilon << std::endl;
	std::cout << "sigma8squared" << sigma8Squared << std::endl;
	std::cout << "samplesPerPixel" << samplesPerPixel << std::endl;
	std::cout << "xRes" << xRes << std::endl;
	std::cout << "yRes" << yRes << std::endl;*/



	std::vector<Tuple3f> newFilteredColors (numberOfSamples);

	//std::cout << "length newFilteredColors: " << numberOfSamples << std::endl;
	//std::cout << "length copyColors: " << numberOfSamples << std::endl;

	//filter the image 4 times with different neighboorhood sizes.
	for (int t = 0; t < (int) box.size(); ++t) {
		int b = box[t];
		//Max number of samples
		int M = b*b*samplesPerPixel/2;
		std::cout << "Start RPF filtering with box width " << b << std::endl;
		ProgressReporter reporter(yRes, "Filtering");


		//Main forloop over all pixels.
		#pragma omp parallel for schedule(dynamic,1) shared(input, b, M, t, copyColors, newFilteredColors)
		for (int y = 0; y < yRes; ++y) {
			for (int x = 0; x < xRes; ++x) {
				//pre-process samples
				std::vector<RPFSample> outputNeighboorhood;// = new std::vector<RPFSample>();
				std::vector<int> neighboorhoodSampleIndices;// = new std::vector<int>();

				preProcessSamples(input, b, M, x, y, outputNeighboorhood, neighboorhoodSampleIndices);
				Assert(outputNeighboorhood.size() == neighboorhoodSampleIndices.size());
				// compute feature weights
				std::vector<float> alpha (3);
				std::vector<float> beta (numberOfSceneFeatures);

				float Wr_c = computeFeatureWeights(t, outputNeighboorhood, alpha, beta);
				//filter color samples
				filterColorSamples(Wr_c, x, y, outputNeighboorhood, neighboorhoodSampleIndices, alpha, beta, copyColors, newFilteredColors);


				//print part of copyColors
				//int i = y*xRes + x;
				//std::cout << copyColors[i].x << "," << copyColors[i].y << "," << copyColors[i].z << std::endl;

			}
			reporter.Update();
		}
		reporter.Done();

		//copy the new filtered colors c'' to c'
		for (int i = 0; i < numberOfSamples; ++i) {
			copyColors[i].x = newFilteredColors[i].x;
			copyColors[i].y = newFilteredColors[i].y;
			copyColors[i].z = newFilteredColors[i].z;
			//std::cout << newFilteredColors[i].x << "," << newFilteredColors[i].x << "," << newFilteredColors[i].x << std::endl;
		}
	}

	float invSamplesPerPixel = 1.f/((float) samplesPerPixel);
	float *totalPixelColor = new float[3];
	for (int y = 0; y < yRes; ++y) {
		for (int x = 0; x < xRes; ++x) {

			int pixelIndex = y*xRes + x;
			totalPixelColor[0] = 0.f;
			totalPixelColor[1] = 0.f;
			totalPixelColor[2] = 0.f;
			for (int s = 0; s < samplesPerPixel; ++s) {
				int index = pixelIndex*samplesPerPixel + s;
				totalPixelColor[0] += copyColors[index].x;
				totalPixelColor[1] += copyColors[index].y;
				totalPixelColor[2] += copyColors[index].z;
			}
			xyz[3*pixelIndex    ] = totalPixelColor[0]*invSamplesPerPixel;
			xyz[3*pixelIndex + 1] = totalPixelColor[1]*invSamplesPerPixel;
			xyz[3*pixelIndex + 2] = totalPixelColor[2]*invSamplesPerPixel;

			//std::cout << xyz[3*pixelIndex] << ", " << xyz[3*pixelIndex+1] << ", " << xyz[3*pixelIndex+2] << std::endl;
		}
	}
	delete[] totalPixelColor;
}

void RPF::preProcessSamples(std::vector<RPFPixel> &input, int b, int M, int x, int y, std::vector<RPFSample> &outputNeighboorhood,
		std::vector<int> &neighboorhoodSampleIndices){
	double sigma_p = b/4.0;

	//Add all the samples of the pixel P(x,y) to the outputNeighboorhood
	vector<RPFSample> pixelSamples = input[y*xRes + x].rpfsamples;
	for (int i = 0; i < samplesPerPixel; ++i) {
		outputNeighboorhood.push_back(pixelSamples[i]);
		neighboorhoodSampleIndices.push_back(i);
	}

	/*************************************************************************************************
	 * Compute mean and standard deviation of scenefeature of samples in pixel P(x,y) for clustering *
	 *************************************************************************************************/

	//Normal
	float meanNx = 0.f;
	float meanNy = 0.f;
	float meanNz = 0.f;
	float sigmaNx = 0.f;
	float sigmaNy = 0.f;
	float sigmaNz = 0.f;
	if(normalFeature){
		for (int i = 0; i < samplesPerPixel; ++i) {
				meanNx += outputNeighboorhood[i].nx;
				meanNy += outputNeighboorhood[i].ny;
				meanNz += outputNeighboorhood[i].nz;
		}
		meanNx /= samplesPerPixel;
		meanNy /= samplesPerPixel;
		meanNz /= samplesPerPixel;


		//standard deviation = sqrt(sum((x_i-mean)^2)/N)
		for (int i = 0; i < samplesPerPixel; ++i) {
				sigmaNx += pow(outputNeighboorhood[i].nx-meanNx,2);
				sigmaNy += pow(outputNeighboorhood[i].ny-meanNy,2);
				sigmaNz += pow(outputNeighboorhood[i].nz-meanNz,2);
		}
		sigmaNx = sqrt(sigmaNx/samplesPerPixel);
		sigmaNy = sqrt(sigmaNy/samplesPerPixel);
		sigmaNz = sqrt(sigmaNz/samplesPerPixel);
	}

	//World Coordinates
	float meanWx = 0.f;
	float meanWy = 0.f;
	float meanWz = 0.f;
	float sigmaWx = 0.f;
	float sigmaWy = 0.f;
	float sigmaWz = 0.f;
	if(worldCoordFeature){
		for (int i = 0; i < samplesPerPixel; ++i) {
			meanWx += outputNeighboorhood[i].wx;
			meanWy += outputNeighboorhood[i].wy;
			meanWz += outputNeighboorhood[i].wz;
		}
		meanWx /= samplesPerPixel;
		meanWy /= samplesPerPixel;
		meanWz /= samplesPerPixel;

		for (int i = 0; i < samplesPerPixel; ++i) {
			sigmaWx += pow(outputNeighboorhood[i].wx-meanWx,2);
			sigmaWy += pow(outputNeighboorhood[i].wy-meanWy,2);
			sigmaWz += pow(outputNeighboorhood[i].wz-meanWz,2);
		}
		sigmaWx = sqrt(sigmaWx/samplesPerPixel);
		sigmaWy = sqrt(sigmaWy/samplesPerPixel);
		sigmaWz = sqrt(sigmaWz/samplesPerPixel);
	}


	//add samples to outputNeighboorhood
	//The number of extra samples to add is the max number M minus the # of samples from this pixel = samplesPerPixel
	int numberOfExtraSamples = M-samplesPerPixel;

	std::vector<int> randomXCoords;
	std::vector<int> randomYCoords;
	std::vector<int> randomSampleCoords;

	unsigned int actualNumberOfExtraSamples = getRandom2DSamples(sigma_p, x, y, b, (b-1)/2, numberOfExtraSamples, input, randomXCoords, randomYCoords, randomSampleCoords);

	double diff;
	for (unsigned int q = 0; q < actualNumberOfExtraSamples; ++q) {
		//Select a random sample from samples inside the box but outside the pixel P(x,y) with distribution based on sigma_p
		int sampleX = randomXCoords[q];
		int sampleY = randomYCoords[q];
		int pixelIndex = sampleY*xRes + sampleX;

		Assert(pixelIndex < input.size());
		RPFSample selectedSample = input[pixelIndex].rpfsamples[randomSampleCoords[q]];

		//perform clustering

		if(normalFeature){
			//Nx
			diff = fabs(selectedSample.nx - meanNx);
			if((diff > normalSigmaFactor*sigmaNx) && (diff > 0.1 || sigmaNx > 0.1)){
				//std::cout << "con Nx" << std::endl;
				continue;
			}
			//Ny
			diff = fabs(selectedSample.ny - meanNy);
			if((diff > normalSigmaFactor*sigmaNy) && (diff > 0.1 || sigmaNy > 0.1)){
				//std::cout << "con Ny" << std::endl;
				continue;
			}
			//Nz
			diff = fabs(selectedSample.nz - meanNz);
			if((diff > normalSigmaFactor*sigmaNz) && (diff > 0.1 || sigmaNz > 0.1)){
				//std::cout << "con Nz" << std::endl;
				continue;
			}
		}
		if(worldCoordFeature){
			//Wx
			diff = fabs(selectedSample.wx - meanWx);
			if((diff > worldCoordinatesSigmaFactor*sigmaWx) && (diff > 0.1 || sigmaWx > 0.1)){
				//std::cout << "con Wx" << std::endl;
				continue;
			}
			//Wy
			diff = fabs(selectedSample.wy - meanWy);
			if((diff > worldCoordinatesSigmaFactor*sigmaWy) && (diff > 0.1 || sigmaWy > 0.1)){
				//std::cout << "con Wy" << std::endl;
				continue;
			}
			//Wz
			diff = fabs(selectedSample.wz - meanWz);
			if((diff > worldCoordinatesSigmaFactor*sigmaWz) && (diff > 0.1 || sigmaWz > 0.1)){
				//std::cout << "con Wz" << std::endl;
				continue;
			}
		}
		//If the execution reaches this statement, add the sample.
		outputNeighboorhood.push_back(selectedSample);
		neighboorhoodSampleIndices.push_back(randomSampleCoords[q]);

	}
	int actualNeighboorhoodSize = outputNeighboorhood.size();


	/************************************************************************
	 * Compute mean and standard deviation of all samples in neighboorhood. *
	 ************************************************************************/
	//Normal
	if(normalFeature){
		meanNx = 0.f, meanNy = 0.f, meanNz = 0.f;
		for (int i = 0; i < actualNeighboorhoodSize; ++i) {
				meanNx += outputNeighboorhood[i].nx;
				meanNy += outputNeighboorhood[i].ny;
				meanNz += outputNeighboorhood[i].nz;
		}
		meanNx /= actualNeighboorhoodSize;
		meanNy /= actualNeighboorhoodSize;
		meanNz /= actualNeighboorhoodSize;

		//standard deviation = sqrt(sum(x_i-mean)/N)
		sigmaNx = 0.f, sigmaNy = 0.f, sigmaNz = 0.f;
		for (int i = 0; i < actualNeighboorhoodSize; ++i) {
				sigmaNx += pow(outputNeighboorhood[i].nx-meanNx,2);
				sigmaNy += pow(outputNeighboorhood[i].ny-meanNy,2);
				sigmaNz += pow(outputNeighboorhood[i].nz-meanNz,2);
		}
		sigmaNx = sqrt(sigmaNx/((float)actualNeighboorhoodSize));
		sigmaNy = sqrt(sigmaNy/((float)actualNeighboorhoodSize));
		sigmaNz = sqrt(sigmaNz/((float)actualNeighboorhoodSize));

		//Compute normalized vector for each sample by removing mean and dividing by standard deviation
		for (int i = 0; i < actualNeighboorhoodSize; ++i) {
				outputNeighboorhood[i].nx = sigmaNx == 0.f ? 0.f : (outputNeighboorhood[i].nx-meanNx)/sigmaNx;
				outputNeighboorhood[i].ny = sigmaNy == 0.f ? 0.f : (outputNeighboorhood[i].ny-meanNy)/sigmaNy;
				outputNeighboorhood[i].nz = sigmaNz == 0.f ? 0.f : (outputNeighboorhood[i].nz-meanNz)/sigmaNz;
		}
	}

	//World Coordinates
	if(worldCoordFeature){
		meanWx = 0.f, meanWy = 0.f, meanWz = 0.f;
		for (int i = 0; i < actualNeighboorhoodSize; ++i) {
			meanWx += outputNeighboorhood[i].wx;
			meanWy += outputNeighboorhood[i].wy;
			meanWz += outputNeighboorhood[i].wz;
		}
		meanWx /= actualNeighboorhoodSize;
		meanWy /= actualNeighboorhoodSize;
		meanWz /= actualNeighboorhoodSize;

		sigmaWx = 0.f, sigmaWy = 0.f, sigmaWz = 0.f;
		for (int i = 0; i < actualNeighboorhoodSize; ++i) {
			sigmaWx += pow(outputNeighboorhood[i].wx-meanWx,2);
			sigmaWy += pow(outputNeighboorhood[i].wy-meanWy,2);
			sigmaWz += pow(outputNeighboorhood[i].wz-meanWz,2);
		}
		sigmaWx = sqrt(sigmaWx/((float)actualNeighboorhoodSize));
		sigmaWy = sqrt(sigmaWy/((float)actualNeighboorhoodSize));
		sigmaWz = sqrt(sigmaWz/((float)actualNeighboorhoodSize));

		//Compute normalized vector for each sample by removing mean and dividing by standard deviation
		for (int i = 0; i < actualNeighboorhoodSize; ++i) {
			outputNeighboorhood[i].wx = sigmaWx == 0.f ? 0.f : (outputNeighboorhood[i].wx-meanWx)/sigmaWx;
			outputNeighboorhood[i].wy = sigmaWy == 0.f ? 0.f : (outputNeighboorhood[i].wy-meanWy)/sigmaWy;
			outputNeighboorhood[i].wz = sigmaWz == 0.f ? 0.f : (outputNeighboorhood[i].wz-meanWz)/sigmaWz;
		}
	}
}

float RPF::computeFeatureWeights(int t, std::vector<RPFSample> &outputNeighboorhood, std::vector<float> &alpha, std::vector<float> &beta){

	/********************************
	 *                              *
	 * CREATE THE NORMALIZED ARRAYS *
	 *                              *
	 ********************************/
	//!!!!! IF YOU ADD A NEW FEATURE DON'T FORGET TO ADJUST THE NUMBEROFSCENEFEATURE FIELD. !!!!!

	//Make arrays for all the mutual informations we have to calculate.
	unsigned int length = outputNeighboorhood.size();

	/*************************
	 * Color channels,       *
	 * Random parameters and *
	 * Position parameters   *
	 *************************/

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


	//Mean variables
	float meanCol1 = 0.f, meanCol2 = 0.f, meanCol3 = 0.f;
	float meanRanLensU = 0.f, meanRanLensV = 0.f, meanRanTime = 0.f;
	float meanPX = 0.f, meanPY = 0.f;

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

		meanCol1 += colorChan1[i];
		meanCol2 += colorChan2[i];
		meanCol3 += colorChan3[i];

		meanRanLensU += randomLensU[i];
		meanRanLensV += randomLensV[i];
		meanRanTime += randomTime[i];

		meanPX += posX[i];
		meanPY += posY[i];
	}

	meanCol1 /= length;
	meanCol2 /= length;
	meanCol3 /= length;

	meanRanLensU /= length;
	meanRanLensV /= length;
	meanRanTime /= length;

	meanPX /= length;
	meanPY /= length;

	//standard deviation = sqrt(sum((x_i-mean)^2)/N)
	float sigmaCol1 = 0.f, sigmaCol2 = 0.f, sigmaCol3 = 0.f;
	float sigmaRanLensU = 0.f, sigmaRanLensV = 0.f, sigmaRanTime = 0.f;
	float sigmaPX = 0.f, sigmaPY = 0.f;


	for (unsigned int i = 0; i < length; ++i) {
		sigmaCol1 += pow((double) (colorChan1[i]-meanCol1),2);
		sigmaCol2 += pow((double) (colorChan2[i]-meanCol2),2);
		sigmaCol3 += pow((double) (colorChan3[i]-meanCol3),2);

		sigmaRanLensU += pow((double) (randomLensU[i]-meanRanLensU),2);
		sigmaRanLensV += pow((double) (randomLensV[i]-meanRanLensV),2);
		sigmaRanTime += pow((double) (randomTime[i]-meanRanTime),2);

		sigmaPX += pow((double) (posX[i]-meanPX),2);
		sigmaPY += pow((double) (posY[i]-meanPY),2);
	}
	sigmaCol1 = sqrt(sigmaCol1/length);
	sigmaCol2 = sqrt(sigmaCol2/length);
	sigmaCol3 = sqrt(sigmaCol3/length);

	sigmaRanLensU = sqrt(sigmaRanLensU/length);
	sigmaRanLensV = sqrt(sigmaRanLensV/length);
	sigmaRanTime = sqrt(sigmaRanTime/length);

	sigmaPX = sqrt(sigmaPX/length);
	sigmaPY = sqrt(sigmaPY/length);


	//Compute normalized vector for each sample by removing mean and dividing by standard deviation
	for (unsigned int i = 0; i < length; ++i) {

		colorChan1[i] = sigmaCol1 == 0.f ? 0.f : (colorChan1[i]-meanCol1)/sigmaCol1;
		colorChan2[i] = sigmaCol2 == 0.f ? 0.f : (colorChan2[i]-meanCol2)/sigmaCol2;
		colorChan3[i] = sigmaCol3 == 0.f ? 0.f : (colorChan3[i]-meanCol3)/sigmaCol3;

		randomLensU[i] = sigmaRanLensU == 0.f ? 0.f : (randomLensU[i]-meanRanLensU)/sigmaRanLensU;
		randomLensV[i] = sigmaRanLensV == 0.f ? 0.f : (randomLensV[i]-meanRanLensV)/sigmaRanLensV;
		randomTime[i] = sigmaRanTime == 0.f ? 0.f : (randomTime[i]-meanRanTime)/sigmaRanTime;

		posX[i] = sigmaPX == 0.f ? 0.f : (posX[i]-meanPX)/sigmaPX;
		posY[i] = sigmaPY == 0.f ? 0.f : (posY[i]-meanPY)/sigmaPY;
	}

	/******************
	 * Scene features *
	 ******************/

	//Normal

	std::vector<float> featureNormalX (length);
	std::vector<float> featureNormalY (length);
	std::vector<float> featureNormalZ (length);

	if(normalFeature){
		float meanNx = 0.f, meanNy = 0.f, meanNz = 0.f;

		for (unsigned int i = 0; i < length; ++i) {
				featureNormalX[i] = outputNeighboorhood[i].nx;
				featureNormalY[i] = outputNeighboorhood[i].ny;
				featureNormalZ[i] = outputNeighboorhood[i].nz;
				meanNx += featureNormalX[i];
				meanNy += featureNormalY[i];
				meanNz += featureNormalZ[i];
		}

		meanNx /= length;
		meanNy /= length;
		meanNz /= length;

		float sigmaNx = 0.f, sigmaNy = 0.f, sigmaNz = 0.f;

		for (unsigned int i = 0; i < length; ++i) {
			sigmaNx += pow((double) (featureNormalX[i]-meanNx),2);
			sigmaNy += pow((double) (featureNormalY[i]-meanNy),2);
			sigmaNz += pow((double) (featureNormalZ[i]-meanNz),2);
		}

		sigmaNx = sqrt(sigmaNx/length);
		sigmaNy = sqrt(sigmaNy/length);
		sigmaNz = sqrt(sigmaNz/length);

		for (unsigned int i = 0; i < length; ++i) {
			featureNormalX[i] = sigmaNx == 0.f ? 0.f : (featureNormalX[i]-meanNx)/sigmaNx;
			featureNormalY[i] = sigmaNy == 0.f ? 0.f : (featureNormalY[i]-meanNy)/sigmaNy;
			featureNormalZ[i] = sigmaNz == 0.f ? 0.f : (featureNormalZ[i]-meanNz)/sigmaNz;
		}
	}

	//World Coordinates

	std::vector<float> featureWorldPosX (length);
	std::vector<float> featureWorldPosY (length);
	std::vector<float> featureWorldPosZ (length);

	if(worldCoordFeature){
		float meanWx = 0.f, meanWy = 0.f, meanWz = 0.f;

		//fill the arrays with the values of the neighboorhood samples
		for (unsigned int i = 0; i < length; ++i) {
			featureWorldPosX[i] = outputNeighboorhood[i].wx;
			featureWorldPosY[i] = outputNeighboorhood[i].wy;
			featureWorldPosZ[i] = outputNeighboorhood[i].wz;
			meanWx += featureWorldPosX[i];
			meanWy += featureWorldPosY[i];
			meanWz += featureWorldPosZ[i];
		}

		meanWx /= length;
		meanWy /= length;
		meanWz /= length;

		float sigmaWx = 0.f, sigmaWy = 0.f, sigmaWz = 0.f;

		for (unsigned int i = 0; i < length; ++i) {
			sigmaWx += pow((double) (featureWorldPosX[i]-meanWx),2);
			sigmaWy += pow((double) (featureWorldPosY[i]-meanWy),2);
			sigmaWz += pow((double) (featureWorldPosZ[i]-meanWz),2);
		}

		sigmaWx = sqrt(sigmaWx/length);
		sigmaWy = sqrt(sigmaWy/length);
		sigmaWz = sqrt(sigmaWz/length);

		for (unsigned int i = 0; i < length; ++i) {
			featureWorldPosX[i] = sigmaWx == 0.f ? 0.f : (featureWorldPosX[i]-meanWx)/sigmaWx;
			featureWorldPosY[i] = sigmaWy == 0.f ? 0.f : (featureWorldPosY[i]-meanWy)/sigmaWy;
			featureWorldPosZ[i] = sigmaWz == 0.f ? 0.f : (featureWorldPosZ[i]-meanWz)/sigmaWz;
		}
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
		float Df1_c1 = normalFeature ? calculateMutualInformation(colorChan1, featureNormalX) : 0.f;
		//Scene feature 2: Normal Y
		float Df2_c1 = normalFeature ? calculateMutualInformation(colorChan1, featureNormalY) : 0.f;
		//Scene feature 3: Normal Z
		float Df3_c1 = normalFeature ? calculateMutualInformation(colorChan1, featureNormalZ) : 0.f;
		//Scene feature 4: World Position X
		float Df4_c1 = worldCoordFeature ? calculateMutualInformation(colorChan1, featureWorldPosX) : 0.f;
		//Scene feature 5: World Position Y
		float Df5_c1 = worldCoordFeature ? calculateMutualInformation(colorChan1, featureWorldPosY) : 0.f;
		//Scene feature 6: World Position Z
		float Df6_c1 = worldCoordFeature ? calculateMutualInformation(colorChan1, featureWorldPosZ) : 0.f;
		//Calc total Df_c1
		float Df_c1 = Df1_c1 + Df2_c1 + Df3_c1 + Df4_c1 + Df5_c1 + Df6_c1;

		//Calculate Wr_c1 with Dr_c1 and Dp_c1
		float Wr_c1 = Dr_c1/(Dr_c1 + Dp_c1 + epsilon);
		//std::cout << "Dr: " << Dr_c1 << " Dp: " << Dp_c1 << " Wr: " << Wr_c1 << std::endl;
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
		float Df1_c2 = normalFeature ? calculateMutualInformation(colorChan2, featureNormalX) : 0.f;
		//Scene feature 2: Normal Y
		float Df2_c2 = normalFeature ? calculateMutualInformation(colorChan2, featureNormalY) : 0.f;
		//Scene feature 3: Normal Z
		float Df3_c2 = normalFeature ? calculateMutualInformation(colorChan2, featureNormalZ) : 0.f;
		//Scene feature 4: World Position X
		float Df4_c2 = worldCoordFeature ? calculateMutualInformation(colorChan2, featureWorldPosX) : 0.f;
		//Scene feature 5: World Position Y
		float Df5_c2 = worldCoordFeature ? calculateMutualInformation(colorChan2, featureWorldPosY) : 0.f;
		//Scene feature 6: World Position Z
		float Df6_c2 = worldCoordFeature ? calculateMutualInformation(colorChan2, featureWorldPosZ) : 0.f;
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
		float Df1_c3 = normalFeature ? calculateMutualInformation(colorChan3, featureNormalX) : 0.f;
		//Scene feature 2: Normal Y
		float Df2_c3 = normalFeature ? calculateMutualInformation(colorChan3, featureNormalY) : 0.f;
		//Scene feature 3: Normal Z
		float Df3_c3 = normalFeature ? calculateMutualInformation(colorChan3, featureNormalZ) : 0.f;
		//Scene feature 4: World Position X
		float Df4_c3 = worldCoordFeature ? calculateMutualInformation(colorChan3, featureWorldPosX) : 0.f;
		//Scene feature 5: World Position Y
		float Df5_c3 = worldCoordFeature ? calculateMutualInformation(colorChan3, featureWorldPosY) : 0.f;
		//Scene feature 6: World Position Z
		float Df6_c3 = worldCoordFeature ? calculateMutualInformation(colorChan3, featureWorldPosZ) : 0.f;
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

	if(normalFeature){
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
	} else{
		beta[0] = 0.f;
		beta[1] = 0.f;
		beta[2] = 0.f;
	}

	if(worldCoordFeature){
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
	} else{
		beta[3] = 0.f;
		beta[4] = 0.f;
		beta[5] = 0.f;
	}

	/*std::cout << "alpha[0]: " << alpha[0] << std::endl;
	std::cout << "alpha[1]: " << alpha[1] << std::endl;
	std::cout << "alpha[2]: " << alpha[2] << std::endl;
	std::cout << "beta[0]: " << beta[0] << std::endl;
	std::cout << "beta[1]: " << beta[1] << std::endl;
	std::cout << "beta[2]: " << beta[2] << std::endl;
	std::cout << "beta[3]: " << beta[3] << std::endl;
	std::cout << "beta[4]: " << beta[4] << std::endl;
	std::cout << "beta[5]: " << beta[5] << std::endl;*/

	return (Wr_c1 + Wr_c2 + Wr_c3)/3;
}
void RPF::filterColorSamples(float Wr_c, int x, int y, std::vector<RPFSample> &outputNeighboorhood, std::vector<int> &neighboorhoodSampleIndices, std::vector<float> &alpha, std::vector<float> &beta, std::vector<Tuple3f> &copyColors, std::vector<Tuple3f> &newFilteredColors){
	float sigmaSquared = 8*sigma8Squared/samplesPerPixel;
	float sigmaCAndFSquared = sigmaSquared/pow(1-Wr_c,2);
	//std::cout << sigmaCAndFSquared << std::endl;
	//std::cout << Wr_c << std::endl;
	//Calc -1/2sigmaC/FSquared in advance
	float internalExpCoefficient = -1/(2*sigmaCAndFSquared);

	//Loop over all samples i in Pixel P(x,y)
	int pixelIndex = (y*xRes + x)*samplesPerPixel;
	for (int i = 0; i < samplesPerPixel; ++i) {
		float col1 = 0.f, col2 = 0.f, col3 = 0.f;
		float weight = 0.f;

		float ci_1 = outputNeighboorhood[i].Lrgb[0];
		float ci_2 = outputNeighboorhood[i].Lrgb[1];
		float ci_3 = outputNeighboorhood[i].Lrgb[2];
		float fi_NX = normalFeature ? outputNeighboorhood[i].nx : 0.f;
		float fi_NY = normalFeature ? outputNeighboorhood[i].ny : 0.f;
		float fi_NZ = normalFeature ? outputNeighboorhood[i].nz : 0.f;
		float fi_WPX = worldCoordFeature ? outputNeighboorhood[i].wx : 0.f;
		float fi_WPY = worldCoordFeature ? outputNeighboorhood[i].wy : 0.f;
		float fi_WPZ = worldCoordFeature ? outputNeighboorhood[i].wz : 0.f;

		//std::cout << outputNeighboorhood[i].nx << std::endl;

		//Loop over all samples j in N
		for (unsigned int j = 0; j < outputNeighboorhood.size(); ++j) {
			float cj_1 = outputNeighboorhood[j].Lrgb[0];
			float cj_2 = outputNeighboorhood[j].Lrgb[1];
			float cj_3 = outputNeighboorhood[j].Lrgb[2];
			float fj_NX = normalFeature ? outputNeighboorhood[j].nx : 0.f;
			float fj_NY = normalFeature ? outputNeighboorhood[j].ny : 0.f;
			float fj_NZ = normalFeature ? outputNeighboorhood[j].nz : 0.f;
			float fj_WPX = worldCoordFeature ? outputNeighboorhood[j].wx : 0.f;
			float fj_WPY = worldCoordFeature ? outputNeighboorhood[j].wy : 0.f;
			float fj_WPZ = worldCoordFeature ? outputNeighboorhood[j].wz : 0.f;

			//Calc wij using alpha and beta
			float wi_jColorWeightedExponential
							= exp(
									internalExpCoefficient*(
											(alpha[0]*pow(ci_1-cj_1,2)) +
											(alpha[1]*pow(ci_2-cj_2,2)) +
											(alpha[2]*pow(ci_3-cj_3,2))
									)
								);
			float wi_jFeatureWeightedExponential
							= exp(
									internalExpCoefficient*(
											(beta[0]*pow(fi_NX - fj_NX,2)) +
											(beta[1]*pow(fi_NY - fj_NY,2)) +
											(beta[2]*pow(fi_NZ - fj_NZ,2)) +
											(beta[3]*pow(fi_WPX - fj_WPX,2)) +
											(beta[4]*pow(fi_WPY - fj_WPY,2)) +
											(beta[5]*pow(fi_WPZ - fj_WPZ,2))
									)
								);
			float wi_j = wi_jColorWeightedExponential*wi_jFeatureWeightedExponential;


			/*if(i != j){
				std::cout << wi_j << std::endl;
			}*/

			//Haal de (reeds gefilterde) kleur uit copyColors op om te filteren.
			int colorToFilterX = Floor2Int(outputNeighboorhood[j].imageX);
			int colorToFilterY = Floor2Int(outputNeighboorhood[j].imageY);
			int colorToFilterSampleIndex = neighboorhoodSampleIndices[j];
			int colorToFilterIndex = ((colorToFilterY*xRes + colorToFilterX)*samplesPerPixel) + colorToFilterSampleIndex;
			float colToFilterX = copyColors[colorToFilterIndex].x;
			float colToFilterY = copyColors[colorToFilterIndex].y;
			float colToFilterZ = copyColors[colorToFilterIndex].z;

			col1 += (wi_j*colToFilterX);
			col2 += (wi_j*colToFilterY);
			col3 += (wi_j*colToFilterZ);

			weight += wi_j;

			//std::cout << weight << std::endl;
		}
		//std::cout << weight << std::endl;
		col1 /= weight;
		col2 /= weight;
		col3 /= weight;

		Assert((i+pixelIndex) < newFilteredColors.size());
		newFilteredColors[i+pixelIndex].x = col1;
		newFilteredColors[i+pixelIndex].y = col2;
		newFilteredColors[i+pixelIndex].z = col3;

					/*if(colToFilterX == col1){
						std::cout << "the same 1" << std::endl;
					}
					if(colToFilterY == col2){
						std::cout << "the same 2" << std::endl;
					}
					if(colToFilterZ == col3){
						std::cout << "the same 3" << std::endl;
					}*/
		//std::cout << colToFilterX << "," << colToFilterY << "," << colToFilterZ << std::endl;
		//std::cout << col1 << "," << col2 << "," << col3 << std::endl;
	}
	//TODO handle HDR issues
}

/*******************************************
 * Sample 2d area with normal distribution *
 *******************************************/

unsigned int RPF::getRandom2DSamples(double sigma_p, int baseX, int baseY, int boxSize, int n, int numberOfSamples,
			std::vector<RPFPixel> &input,
			std::vector<int> &outputXCoords, std::vector<int> &outputYCoords, std::vector<int> &outputSampleCoords){


	//Calculate the chances of each sample within the box.
	std::vector<double> normalChances;// = *(new vector<double>());
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
			//laat de pixel x,y zelf weg
			if(j == baseY && i == baseX){
				for (int s = 0; s < samplesPerPixel; ++s) {
					normalChances.push_back(0.0);
					//cout << "before: x: " << i << ", y: " << j << ", s: " << s << ": " << 0.0 << endl;
				}
				continue;
			}
			int pixelIndex = j*xRes +i;
			for (int s = 0; s < samplesPerPixel; ++s) {
				Assert(pixelIndex < input.size());
				double  chance = calcNormalChance(input[pixelIndex].rpfsamples[s].imageX, input[pixelIndex].rpfsamples[s].imageY, (double)baseX+0.5, (double)baseY+0.5, sigma_p);
				normalChances.push_back(chance);
				//cout << "call with: xCoord: " << xCoords[index] << ", ycoord: " << yCoords[index] << ", xMean: " << (double)x+0.5 << ", yMean: " << (double)y+0.5 << ", sigma: " << sigma_p << endl;
				//std::cout << "added: (x: " << i << ", y: " << j << ", s: " << s << ": " << chance << ")" << std::endl;
				//std::cout << "image: (x: " << input[pixelIndex].rpfsamples[s].imageX << ", y: " << input[pixelIndex].rpfsamples[s].imageY << std::endl;
			}
		}
	}

	//Normalize the chances because now they dont add up to 1 (it is a contiuous domain and the middle pixel is left out).
	normalizeChancesVector(normalChances, calcTotalVector(normalChances));


	boost::mt19937 rng (std::time(0));
	boost::uniform_real<float> u(0.0f, 1.0f);
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);

	std::vector<int> randomSamples;// = *(new vector<int>());
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
		//rekening met de -1 die hieruit kan komen!!!
		if(selectedSample == -1){
			std::cout << "Random sample not found (-1 result), thus continue with the rest" << std::endl;
			continue;
		}
		//Add the sample to the list
		randomSamples.push_back(selectedSample);
		//Keep the chance of this selected sample as it is needed in the next iteration
		Assert(selectedSample < normalChances.size());
		chanceOfLastSelectedSample = normalChances[selectedSample];
		//Zet de kans op 0 zodat die nimeer gebruikt wordt.
		normalChances[selectedSample] = 0.0;
	}

	//Transform the index of the selected samples into an x, y and sample coordinate.
	int xSize = boxSize;
	int startX = baseX-n;
	int startY = baseY-n;
	if((baseX-n) < 0){
		xSize = baseX+n+1;
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
	for (unsigned int m = 0; m < randomSamples.size(); ++m) {
		int index = randomSamples[m];
		int s = index%samplesPerPixel;
		int i = (index/samplesPerPixel)%xSize;
		int j = (index/(xSize*samplesPerPixel));

		Assert((i+startX) < xRes);
		Assert((j+startY) < yRes);
		Assert(s < samplesPerPixel);

		/*outputXCoords[m] = i + startX;
		outputYCoords[m] = j + startY;
		outputSampleCoords[m] = s;*/
		//the place was already allocated but other places further were used..
		//Now the place is not allocated (a new empty vector is made) and values are added to the vector

		outputXCoords.push_back(i + startX);
		outputYCoords.push_back(j + startY);
		outputSampleCoords.push_back(s);
	}
	return randomSamples.size();
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
	Assert(vect.size() > 0);
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
		Assert(Xbins[i] < histX.size());
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
		Assert(Ybins[i] < histXY.size());
		histXY[Ybins[i]][Xbins[i]] += 1.f;
	}
	for (int i = 0; i < yNumberOfStates; ++i) {
		for (int j = 0; j < xNumberOfStates; ++j) {
			histXY[i][j] /= length;
		}
	}
}

float RPF::calculateMutualInformation(std::vector<float> &X, std::vector<float> &Y){
	//Mu(X,Y) = sum(X(x),sum(Y(y), p(x,y)*log(p(x,y)/(p(x)*p(y)))) )

	//You cannot calculate the mutual information of 2 vectors with different length.
	Assert(X.size() == Y.size());

	// Calculate pX and pY for calculating p(x) and p(y).
	std::vector<int> Xbins;// = new std::vector<int>();
	std::vector<int> Ybins;// = new std::vector<int>();
	int nStatesX = quantizeAndPositiveVector(X, Xbins);
	int nStatesY = quantizeAndPositiveVector(Y, Ybins);

	std::vector<float> histX;// = new std::vector<float>();
	getHistogram(Xbins, nStatesX, histX);
	std::vector<float> histY;// = new std::vector<float>();
	getHistogram(Ybins, nStatesY, histY);

	//Calculate pXY for calculating p(x,y).
	std::vector<std::vector<float> > histXY;// = new std::vector<std::vector<float> >();
	getJointHistogram(Xbins, nStatesX, Ybins, nStatesY, histXY);

	//Calculate Mu(X,Y)
	float muXY = 0.f;
	for (int i = 0; i < nStatesY; ++i) {
		for (int j = 0; j < nStatesX; ++j) {
			if(histXY[i][j] !=0.f && histX[j] != 0.f && histY[i] != 0.f){
				muXY += histXY[i][j]*log(histXY[i][j]/(histX[j]*histY[i]));
			}
		}
	}
	muXY /= log(2);
	return muXY;
}


