/*
 * Triangle.hpp
 *
 *  Created on: Nov 8, 2012
 *      Author: thypo
 */

#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include "Vertex.hpp"

class Triangle{
public:
	Triangle();
	Triangle(Vertex *a, Vertex *b, Vertex *c);
	Vertex getA();
	void setA(Vertex a);
	Vertex getB();
	void setB(Vertex b);
	Vertex getC();
	void setC(Vertex c);
	const char *toString();
	~Triangle(){ delete[] vertices;}
private:
	Vertex *vertices;
};


#endif /* TRIANGLE_HPP_ */
