/*
 * Triangle.cpp
 *
 *  Created on: Nov 8, 2012
 *      Author: thypo
 */

#include "Vertex.hpp"
#include "Triangle.hpp"
#include <string>
using namespace std;

	Triangle::Triangle(){
		vertices = new Vertex[3];
		vertices[0] = Vertex (0.0, 0.0, 0.0);
		vertices[1] = Vertex (0.0, 0.0, 0.0);
		vertices[2] = Vertex (0.0, 0.0, 0.0);
	}

	Triangle::Triangle(Vertex *a, Vertex *b, Vertex *c){
		vertices = new Vertex[3];
		vertices[0] = *a;
		vertices[1] = *b;
		vertices[2] = *c;
	}

	Vertex Triangle::getA(){
		return vertices[0];
	}

	void Triangle::setA(Vertex a){
		vertices[0] = a;
	}

	Vertex Triangle::getB(){
		return vertices[1];
	}

	void Triangle::setB(Vertex b){
		vertices[1] = b;
	}

	Vertex Triangle::getC(){
		return vertices[2];
	}

	void Triangle::setC(Vertex c){
		vertices[2] = c;
	}

	const char *Triangle::toString(){
		string str (vertices[0].toString());
		str.append(vertices[1].toString());
		str.append(vertices[2].toString());
		return str.c_str();
	}
