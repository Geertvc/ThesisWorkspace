//============================================================================
// Name        : TestProject.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <vector>
#include "Tuple3f.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
//#include "rpf/RPFCollector.h"
using namespace std;

void Tuple3fTest();
void RPFCollectorTest();
void sizeofTest();
void stringstreamTest();
void maxIntValueTest();
void normalDistTest();
void normalDistTest2D();
void alternativeNormalDist2D();
void mutualInformationTest();

template <class T>
void printArray(T *array, int size);

int main() {
	mutualInformationTest();
	//alternativeNormalDist2D();
	//normalDistTest();
	//normalDistTest2D();
	//Tuple3fTest();
	//RPFCollectorTest();
	//sizeofTest();
	//stringstreamTest();
	//maxIntValueTest();
	return 0;
}

int quantizeAndPositiveVector(std::vector<float> &vect, std::vector<int> &output){
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
	std::cout << "min: " << min << std::endl;
	std::cout << "max: " << max << std::endl;
	for (unsigned int i = 0; i < output.size(); ++i) {
		output[i] -= min;
	}

	return (max-min+1);
}

void getHistogram(std::vector<int> &Xbins, int xNumberOfStates, std::vector<float> &histX){
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

void getJointHistogram(std::vector<int> &Xbins, int xNumberOfStates, std::vector<int> &Ybins, int yNumberOfStates, std::vector<std::vector<float> > &histXY){
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

float calculateMutualInformation(std::vector<float> &X, std::vector<float> &Y){
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


	for (unsigned int x = 0; x < X.size(); ++x) {
		std::cout << "positive x: " << (*Xbins)[x] << std::endl;
	}
	for (unsigned int y = 0; y < Y.size(); ++y) {
		std::cout << "positive y: " << (*Ybins)[y] << std::endl;
	}
	std::cout << "nStatesX: " << nStatesX << std::endl;
	std::cout << "nStatesY: " << nStatesY << std::endl;

	std::vector<float> *histX = new std::vector<float>();
	getHistogram(*Xbins, nStatesX, *histX);
	std::vector<float> *histY = new std::vector<float>();
	getHistogram(*Ybins, nStatesY, *histY);

	for (unsigned int i = 0; i < (*histX).size(); ++i) {
		std::cout << "histX " << i << ": " << (*histX)[i] << std::endl;
	}
	for (unsigned int i = 0; i < (*histY).size(); ++i) {
		std::cout << "histY " << i << ": " << (*histY)[i] << std::endl;
	}
	//Calculate pXY for calculating p(x,y).

	std::vector<std::vector<float> > *histXY = new std::vector<std::vector<float> >();
	getJointHistogram(*Xbins, nStatesX, *Ybins, nStatesY, *histXY);

	for (int i = 0; i < nStatesY; ++i) {
		for (int j = 0; j < nStatesX; ++j) {
			std::cout << "histXY (" << i << ", " << j << "): " << (*histXY)[i][j] << std::endl;
		}
	}

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

void mutualInformationTest(){
	//Fill vectors with random values
	boost::mt19937 rng (std::time(0));
	boost::uniform_real<float> u(-5.0f, 5.0f);
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
	int xSize = 10;
	int ySize = 10;
	std::vector<float> *X = new std::vector<float>();
	std::vector<float> *Y = new std::vector<float>();
	for (int x = 0; x < xSize; ++x) {
		X->push_back(gen());
		std::cout << "x: " << (*X)[x] << std::endl;
	}
	for (int y = 0; y < ySize; ++y) {
		Y->push_back(gen());
		std::cout << "y: " << (*Y)[y] << std::endl;
	}

	float mutuinfo = calculateMutualInformation(*X,*Y);
	std::cout << "mutinfo: " << mutuinfo << std::endl;
}

double calcNormalChance(float xCoord, float yCoord, double xMean, double yMean, double sigma){
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

int findRandomSample(std::vector<double> &chances, double randomNumber){
	double totalSoFar = 0.0;
	for (unsigned int i = 0; i < chances.size(); ++i) {
		totalSoFar += chances[i];
		if(randomNumber < totalSoFar){
			cout << "found: " << i << " totalsofar: " << totalSoFar << endl;
			return i;
		}
	}
	return -1;
}

void normalizeChancesVector(std::vector<double> &chances, double totalChance){
	for (unsigned int i = 0; i < chances.size(); ++i) {
		chances[i] = chances[i]/totalChance;
	}
}

double calcTotalVector(std::vector<double> &chances){
	double total = 0.0;
	for (unsigned int i = 0; i < chances.size(); ++i) {
		total += chances[i];
	}
	return total;
}

void getGaussian2DSamples(double sigma_p, int x, int y, int boxSize, int n, int numberOfExtraSamples, int samplesPerPixel, int xRes, int yRes,
		std::vector<float> &inputXCoords, std::vector<float> &inputYCoords,
		std::vector<int> &outputXCoords, std::vector<int> &outputYCoords, std::vector<int> &outputSampleCoords){
	//Calculate the chances of each sample within the box.
	std::vector<double> normalChances = *(new vector<double>());
	normalChances.reserve(boxSize*boxSize*samplesPerPixel);
	for (int j = y-n; j < y+n+1; ++j) {
		if(j < 0){
			continue;
		} else if(j > yRes-1){
			break;
		}
		cout << "j: " << j << endl;
		for (int i = x-n; i < x+n+1; ++i) {
			if(i < 0){
				continue;
			} else if(i > xRes-1){
				break;
			}
			cout << "i: " << i << endl;
			int pixelIndex = j*xRes +i;
			//laat de pixel x,y zelf weg
			if(j == y && i == x){
				for (int s = 0; s < samplesPerPixel; ++s) {
					normalChances.push_back(0.0);
					//cout << "before: x: " << i << ", y: " << j << ", s: " << s << ": " << 0.0 << endl;
				}
				continue;
			}
			for (int s = 0; s < samplesPerPixel; ++s) {
				int index = pixelIndex*samplesPerPixel + s;
				double  chance = calcNormalChance(inputXCoords[index], inputYCoords[index], (double)x+0.5, (double)y+0.5, sigma_p);
				normalChances.push_back(chance);
				//cout << "call with: xCoord: " << xCoords[index] << ", ycoord: " << yCoords[index] << ", xMean: " << (double)x+0.5 << ", yMean: " << (double)y+0.5 << ", sigma: " << sigma_p << endl;
				cout << "added: (x: " << i << ", y: " << j << ", s: " << s << ": " << chance << ")" << endl;
			}
		}
	}
	cout << "chances size: " << normalChances.size() << endl;
	//Normalize the chances because now they dont add up to 1 (it is a contiuous domain and the middle pixel is left out).
	normalizeChancesVector(normalChances, calcTotalVector(normalChances));

	boost::mt19937 rng (std::time(0));
	boost::uniform_real<float> u(0.0f, 1.0f);
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);

	std::vector<int> randomSamples = *(new vector<int>());
	randomSamples.reserve(normalChances.size()-samplesPerPixel);
	double chanceOfLastSelectedSample = 0.0;
	for (int i = 0; i < numberOfExtraSamples; ++i) {
		double total = 1.0 - chanceOfLastSelectedSample;
		if(total <= 0.000000001){
			break;
		}
		normalizeChancesVector(normalChances, total);
		cout << "total: " << calcTotalVector(normalChances) << endl;
		double randomNumber = gen();
		cout << "random: " << randomNumber << endl;
		int selectedSample = findRandomSample(normalChances, randomNumber);
		randomSamples.push_back(selectedSample);
		chanceOfLastSelectedSample = normalChances[selectedSample];
		normalChances[selectedSample] = 0.0;
	}

	//Transform the index of the selected samples into an x, y and sample coordinate.
	int xSize = boxSize;
	int startX = x-n;
	int startY = y-n;
	if((x-n) < 0){
		xSize = x+n;
		startX = 0;
	} else if((x+n) > (xRes-1)){
		xSize = xRes -x + n;
	}
	if((y-n) < 0){
		startY = 0;
	}
	cout << "x-n: " << (x-n) << endl;
	cout << "xSize: " << xSize << endl;
	cout << "startX: " << startX << endl;
	cout << "startY: " << startY << endl;
	for (unsigned int i = 0; i < randomSamples.size(); ++i) {
		int index = randomSamples[i];
		int s = index%samplesPerPixel;
		int i = (index/samplesPerPixel)%xSize;
		int j = (index/(xSize*samplesPerPixel));
		outputXCoords.push_back(i + startX);
		outputYCoords.push_back(j + startY);
		outputSampleCoords.push_back(s);
	}

	/*int counter = 0;
	for (unsigned int j = 0; j < outputXCoords.size(); ++j) {
		int x1 = outputXCoords[j];
		int y1 = outputYCoords[j];
		int s1 = outputSampleCoords[j];
		for (unsigned int l = j+1; l < outputXCoords.size(); ++l) {
			int x2 = outputXCoords[l];
			int y2 = outputYCoords[l];
			int s2 = outputSampleCoords[l];
			if(x1 == x2 && y1 == y2 & s1 == s2){
				cout << "algorithm is not working correctly. (" << x1 << ", " << y1 << ", " << s1 << ") == (" << x2 << ", " << y2 << ", " << s2 << ")" << endl;
				return;
			} else{
				counter++;
				cout << "ok " << counter << endl;
			}
		}
	}*/
}

void alternativeNormalDist2D(){
	boost::mt19937 rng (std::time(0));
	boost::uniform_real<float> u(0.0f, 1.0f);
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
	//fill random samples
	int xRes = 5;
	int yRes = 5;
	int samplesPerPixel = 2;
	int totalNbOfSamples = xRes*yRes*samplesPerPixel;
	std::vector<float> xCoords = *(new vector<float>());
	std::vector<float> yCoords = *(new vector<float>());
	xCoords.reserve(totalNbOfSamples);
	yCoords.reserve(totalNbOfSamples);
	for (int y = 0; y < yRes; ++y) {
		for (int x = 0; x < xRes; ++x) {
			int pixelIndex = y*xRes + x;
			for (int s = 0; s < samplesPerPixel; ++s) {
				int index = pixelIndex*samplesPerPixel + s;
				xCoords.push_back((float)x + gen());
				yCoords.push_back((float)y + gen());
				//xCoords.push_back((float)x + 0.5f);
				//yCoords.push_back((float)y + 0.5f);
			}
		}
	}
	cout << "number of samples: " << xCoords.size() << endl;
	/*for (unsigned int i = 0; i < xCoords.size(); ++i) {
		cout << "random coord: " << xCoords[i] << ", " << yCoords[i] << endl;
	}*/

	//Choose random samples from these samples
	int b = 5;
	int n = (b-1)/2;
	//change to b*b*samplesPerPixel - samplesPerPixel if you want to select all samples
	int M = b*b*samplesPerPixel/2;
	double sigma_p = b/4.0;

	int x = 3;
	int y = 4;

	std::vector<int> *outputXCoords = new vector<int>();
	std::vector<int> *outputYCoords = new vector<int>();
	std::vector<int> *outputSampleCoords = new vector<int>();
	getGaussian2DSamples(sigma_p, x, y, b, n, M-samplesPerPixel, samplesPerPixel, xRes, yRes, xCoords, yCoords, *outputXCoords, *outputYCoords, *outputSampleCoords);
	cout << "number of found samples: " << (*outputXCoords).size() << endl;


	for (unsigned int i = 0; i < (*outputXCoords).size(); ++i) {
		cout << i << ": (x: " << (*outputXCoords)[i] << ",y: " << (*outputYCoords)[i] << ",s: " << (*outputSampleCoords)[i] << ")" << endl;
	}

	//calculate the normal chances

	/*std::vector<double> normalChances = *(new vector<double>());
	normalChances.reserve(b*b*samplesPerPixel);
	for (int j = y-n; j < y+n+1; ++j) {
		for (int i = x-n; i < x+n+1; ++i) {
			int pixelIndex = j*xRes +i;
			//laat de pixel x,y zelf weg
			if(j == y && i == x){
				for (int s = 0; s < samplesPerPixel; ++s) {
					int index = pixelIndex*samplesPerPixel + s;
					normalChances.push_back(0.0);
					cout << "before: x: " << i << ", y: " << j << ", s: " << s << ": " << 0.0 << endl;
				}
				continue;
			}
			for (int s = 0; s < samplesPerPixel; ++s) {
				int index = pixelIndex*samplesPerPixel + s;
				double  chance = calcNormalChance(xCoords[index], yCoords[index], (double)x+0.5, (double)y+0.5, sigma_p);
				normalChances.push_back(chance);

				//cout << "call with: xCoord: " << xCoords[index] << ", ycoord: " << yCoords[index] << ", xMean: " << (double)x+0.5 << ", yMean: " << (double)y+0.5 << ", sigma: " << sigma_p << endl;
				cout << "before: x: " << i << ", y: " << j << ", s: " << s << ": " << chance << endl;
			}
		}
	}
	cout << "normalChances size: " << normalChances.size() << endl;
	double total = 0.0;
	for (unsigned int k = 0; k < normalChances.size(); ++k) {
		int s = k%samplesPerPixel;
		int i = (k/samplesPerPixel)%b;
		int j = (k/(b*samplesPerPixel));
		//int i = ((k-s)/samplesPerPixel)%b;
		//int i = ((k-(k%samplesPerPixel))/samplesPerPixel)%b;
		//int j = (k-(k%(b*samplesPerPixel)))/(b*samplesPerPixel);
		cout << "after: x: " << (i+x-n) << ", y: " << (j+y-n) << ", s: " << s << ": " << normalChances[k] << endl;
		total += normalChances[k];
	}
	cout << "total chance: " << total << endl;
	//randomly choose a sample and put that boolean on true or remove it.

	normalizeChancesVector(normalChances, calcTotalVector(normalChances));

	std::vector<int> randomSamples = *(new vector<int>());
	randomSamples.reserve(M);
	double chanceOfLastSelectedSample = 0.0;
	for (int i = 0; i < M; ++i) {
		double total = 1.0 - chanceOfLastSelectedSample;
		normalizeChancesVector(normalChances, total);
		cout << "total: " << calcTotalVector(normalChances) << endl;
		double randomNumber = gen();
		cout << "random: " << randomNumber << endl;
		int selectedSample = findRandomSample(normalChances, randomNumber);
		randomSamples.push_back(selectedSample);
		chanceOfLastSelectedSample = normalChances[selectedSample];
		normalChances[selectedSample] = 0.0;
	}

	for (unsigned int i = 0; i < randomSamples.size(); ++i) {
		cout << "selected sample " << i << ": " << randomSamples[i]  << endl;
	}*/
}


double *calcNormalValues(int numberOfSamples, boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > generator){
	double *values = new double[numberOfSamples];
	for (int i = 0; i < numberOfSamples; ++i) {
		values[i] = generator();
	}
	return values;
}

void normalDistTest2D(){

	int samplesPerPixel = 5;
	int b = 55;
	int numberOfSamples = b*b*samplesPerPixel/2;
	int baseX = 20;
	int baseY = 50;
	double sigma = b/4;

	boost::mt19937 rngX;
	boost::normal_distribution<> ndX(baseX, sigma);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_norX(rngX, ndX);

	boost::mt19937 rngY;
	boost::normal_distribution<> ndY(baseY, sigma);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_norY(rngY, ndY);

	double *xCoords = calcNormalValues(numberOfSamples, var_norX);
	double *yCoords = calcNormalValues(numberOfSamples, var_norY);

	for (int i = 0; i < numberOfSamples; ++i) {
		cout << xCoords[i] << ", " << yCoords[i] << endl;
	}
}

void normalDistTest(){
	double mean = 5.0;
	double sigma = 2.0;

	boost::mt19937 rng;
	boost::normal_distribution<> nd(mean, sigma);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);

	int nbTests = 10000;
	double *values = new double[nbTests];
	for (int i = 0; i < nbTests; ++i) {
		double value = var_nor();
		values[i] = value;
	}

	int p[10] = {};
	for (int i=0; i<nbTests; ++i) {
		double number = values[i];
	    if ((number>=0.0)&&(number<=10.0)) ++p[int(number)];
	}

	cout << "normal_distribution (" << mean << ", " << sigma << "):" << endl;
	int nstars = 100;
	for (int i=0; i<10; ++i) {
		std::cout << i << "-" << (i+1) << ": ";
		std::cout << std::string(p[i]*nstars/nbTests,'*') << std::endl;
	}
}

void Tuple3fTest(){
	Tuple3f *tup = new Tuple3f();
	std::cout << "x: " << tup->x << ", y: " << tup->y << ", z: " << tup->z << std::endl;
	Tuple3f *tup2 = new Tuple3f(1.f, 2.f, 3.f);
	std::cout << "x: " << tup2->x << ", y: " << tup2->y << ", z: " << tup2->z << std::endl;
	Tuple3f *tup3 = new Tuple3f(.3f, .2f, .1f);
	std::cout << "x: " << tup3->x << ", y: " << tup3->y << ", z: " << tup3->z << std::endl;
}

void RPFCollectorTest(){
	//RPFCollector test
	int xRes = 5;
	int yRes = 5;
	int size = 3*xRes*yRes;
	//RPFCollector *test = new RPFCollector(xRes,yRes);
	float *values = new float[size];
	int i;
	for (i = 0; i < size; ++i) {
		values[i] = i;
	}
	printArray <float> (values, size);
	//test->setColorValues(values);
	//printArray <float> (test->getColorValues(), size);
}

template <class T>
void printArray(T *array, int size){
	int i;
	for (i = 0; i < size; ++i) {
		cout << i << ": " << array[i] << endl;
	}
}

void sizeofTest(){
	//othertest
	int xRes = 100;
	int yRes = 200;
	float *longArray = new float[3*xRes*yRes];
	cout << "size: " << sizeof(longArray) << endl;

	float longArray2[3*xRes*yRes];
	cout << "size: " << sizeof(longArray2) << endl;
}

void stringstreamTest(){
	//test
	float testValue = 1.2345678910;
	float value2 = 2.3456789;
	float value3 = 3.456;
	stringstream os;
	os << setprecision(numeric_limits<float>::digits10+3);
	os << testValue;
	string result = os.str();

	/*std::string result = boost::lexical_cast<std::string>(testValue);
	result.append(" ");
	result.append(boost::lexical_cast<std::string>(value2));
	result.append(" ");
	result.append(boost::lexical_cast<std::string>(value3));*/
	cout << result; // prints !!!Hello World!!!


	ofstream file;
	file.open("testfile.txt");
	file << "f " << testValue << " " << value2 << " " << value3 << endl;
	file.close();
}

void maxIntValueTest(){
	int i = 2147483647000000000000000000000000000000;
	int j = 0;
	cout << "i: " << i << endl;
	cout << "j: " << j << endl;
}
