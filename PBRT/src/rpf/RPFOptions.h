/*
 * RPFOptions.h
 *
 *  Created on: Mar 26, 2013
 *      Author: thypo
 */

#ifndef RPFOPTIONS_H_
#define RPFOPTIONS_H_

struct RPFOptions {
	RPFOptions() { normalFeature = worldCoordFeature = false;
    			sigma8Squared = 0.02;
				}
    bool normalFeature;
    bool worldCoordFeature;
    float sigma8Squared;
    vector<int> iterations;
    char *rpfOutfile;
};


#endif /* RPFOPTIONS_H_ */
