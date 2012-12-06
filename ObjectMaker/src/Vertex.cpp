/*
 * Vertex.cpp
 *
 *  Created on: Nov 8, 2012
 *      Author: thypo
 */

//#include <iostream>
//#include <stdio.h>
#include <string>
#include <sstream>
#include "Vertex.hpp"
using namespace std;

	Vertex::Vertex(){
		coords = new float[3];
		coords[0] = 0.0;
		coords[1] = 0.0;
		coords[2] = 0.0;
	}

	Vertex::Vertex(float x, float y, float z){
		coords = new float[3];
		coords[0] = x;
		coords[1] = y;
		coords[2] = z;
	}

	float Vertex::getX(){
		return coords[0];
	}

	float Vertex::getY(){
		return coords[1];
	}

	float Vertex::getZ(){
		return coords[2];
	}

	void Vertex::setX(float x){
		coords[0] = x;
	}

	void Vertex::setY(float y){
		coords[1] = y;
	}

	void Vertex::setZ(float z){
		coords[2] = z;
	}

	//Returns a string containing "x y z"
	const char *Vertex::toString(){
		ostringstream os;
		os << coords[0];
		string result = os.str();
		return result.c_str();
	}


