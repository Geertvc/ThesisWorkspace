/*
 * RPF.h
 *
 *  Created on: Jan 8, 2013
 *      Author: thypo
 */

#ifndef RPF_H_
#define RPF_H_

#include <vector>
#include "RPFPixel.h"
#include "Tuple3f.h"

class RPF {
public:
	RPF(RPFOptions &rpfOptions);
	~RPF(){}
	void applyFilter(std::vector<RPFPixel> &input, float *xyz, int xResolution, int yResolution, int samplesPerPixel);
private:
	int samplesPerPixel;
	int xRes, yRes;
	int numberOfSceneFeatures;
	float normalSigmaFactor;
	float worldCoordinatesSigmaFactor;

	float epsilon;
	float sigma8Squared;
	vector<int> box;

	bool normalFeature;
	bool worldCoordFeature;


	void preProcessSamples(std::vector<RPFPixel> &input, int b, int M, int x, int y, std::vector<RPFSample> &outputNeighboorhood, std::vector<int> &neighboorhoodSampleIndices);
	float computeFeatureWeights(int t, std::vector<RPFSample> &outputNeighboorhood, std::vector<float> &alpha, std::vector<float> &beta);
	void filterColorSamples(float Wr_c, int x, int y, std::vector<RPFSample> &outputNeighboorhood, std::vector<int> &neighboorhoodSampleIndices, std::vector<float> &alpha, std::vector<float> &beta, std::vector<Tuple3f> &copyColors, std::vector<Tuple3f> &newFilteredColors);
	/* Method used to sample the 2D area of the box with normal distribution*/
	unsigned int getRandom2DSamples(double sigma_p, int baseX, int baseY, int boxSize, int n, int numberOfSamples,
			std::vector<RPFPixel> &input,
			std::vector<int> &outputXCoords, std::vector<int> &outputYCoords, std::vector<int> &outputSampleCoords);
	/* Methods used to get the random samples */
	double calcNormalChance(float xCoord, float yCoord, double xMean, double yMean, double sigma);
	void normalizeChancesVector(std::vector<double> &chances, double totalChance);
	double calcTotalVector(std::vector<double> &chances);
	int findRandomSample(std::vector<double> &chances, double randomNumber);

	/* Methods to calculate the mutual information */
	int quantizeAndPositiveVector(std::vector<float> &vect, std::vector<int> &output);
	void getHistogram(std::vector<int> &Xbins, int xNumberOfStates, std::vector<float> &histX);
	void getJointHistogram(std::vector<int> &Xbins, int xNumberOfStates, std::vector<int> &Ybins, int yNumberOfStates, std::vector<std::vector<float> > &histXY);
	float calculateMutualInformation(std::vector<float> &X, std::vector<float> &Y);
};

#endif /* RPF_H_ */
