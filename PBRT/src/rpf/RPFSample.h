/*
 * RPFSample.h
 *
 *  Created on: Dec 11, 2012
 *      Author: thypo
 */

#ifndef RPFSAMPLE_H_
#define RPFSAMPLE_H_

class RPFSample {
public:
	RPFSample();
	RPFSample& operator=( RPFSample& rhs );
	~RPFSample(){}
	float Lrgb[3];
};

#endif /* RPFSAMPLE_H_ */
