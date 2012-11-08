/*
 * TestImage.hpp
 *
 *  Created on: Nov 6, 2012
 *      Author: thypo
 */

#ifndef TESTIMAGE_HPP_
#define TESTIMAGE_HPP_

class TestImage{
public:
	int xRes;
	int yRes;
	TestImage(int xRes, int yRes);
	double getValue(int i, int j);
	void setValue(int i, int j, double value);
	void print();
	~TestImage();
private:
	double **data;
};


#endif /* TESTIMAGE_HPP_ */
