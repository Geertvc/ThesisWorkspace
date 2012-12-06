/*
 * RPFCollector.h
 *
 *  Created on: Dec 6, 2012
 *      Author: thypo
 */

#ifndef RPFCOLLECTOR_H_
#define RPFCOLLECTOR_H_

class RPFCollector {
public:
	RPFCollector(int xRes, int yRes);
	void setColorValues(float *values);
	float *getColorValues();
	~RPFCollector(){
		delete[] colorValues;
	}
private:
	int nbPixels;
	float *colorValues;
};


#endif /* RPFCOLLECTOR_H_ */
