/*
 * Vertex.hpp
 *
 *  Created on: Nov 8, 2012
 *      Author: thypo
 */

#ifndef VERTEX_HPP_
#define VERTEX_HPP_

//#include <string>
//#include <sstream>
//#include <iostream>

class Vertex{
public:
	Vertex();
	Vertex(float x, float y, float z);
	float getX();
	void setX(float x);
	float getY();
	void setY(float y);
	float getZ();
	void setZ(float z);
	const char *toString();
	~Vertex(){ delete[] coords;}
private:
	float *coords;
};


#endif /* VERTEX_HPP_ */
