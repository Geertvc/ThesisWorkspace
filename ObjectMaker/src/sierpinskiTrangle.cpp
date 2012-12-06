/*
 * sierpinskiTrangle.cpp
 *
 *  Created on: Nov 1, 2012
 *      Author: thypo
 */

#include <iostream>
#include <fstream>
#include <math.h>
#include "Triangle.hpp"
#include "Vertex.hpp"
using namespace std;

Vertex *basicVertices;
Triangle *basicTriangles;

void sierpinksiTriangle4(int depth, const char* fileName){
	float l = 0.5;

	basicVertices = new Vertex[5];
	basicVertices[0] = Vertex (-l, -l, 0);
	basicVertices[1] = Vertex (l, l, 0);
	basicVertices[2] = Vertex (l, -l, 0);
	basicVertices[3] = Vertex (-l, l, 0);
	basicVertices[4] = Vertex (0, 0, l/sqrt(2.0));

	/*basicTriangles = new Triangle[6];
	basicTriangles[0] = Triangle (&basicVertices[0],&basicVertices[1],&basicVertices[2]);
	basicTriangles[1] = Triangle (&basicVertices[0],&basicVertices[3],&basicVertices[1]);
	basicTriangles[2] = Triangle (&basicVertices[0],&basicVertices[2],&basicVertices[4]);
	basicTriangles[3] = Triangle (&basicVertices[2],&basicVertices[1],&basicVertices[4]);
	basicTriangles[4] = Triangle (&basicVertices[1],&basicVertices[3],&basicVertices[4]);
	basicTriangles[5] = Triangle (&basicVertices[3],&basicVertices[0],&basicVertices[4]);*/



	ofstream file;
	file.open(fileName);
	cout << "Writing to " << fileName << endl;
	for (int i = 0; i < 5; ++i) {
		file << "v " << basicVertices[i].toString() << endl;
	}

	/*file << "v 1 1 1" << endl;
	file << "v 1 1 -1" << endl;
	file << "v 1 -1 1" << endl;
	file << "v 1 -1 -1" << endl;
	file << "v -1 1 1" << endl;*/
	file << "f 0 1 2" << endl;
	file << "f 0 3 1" << endl;
	file << "f 0 2 4" << endl;
	file << "f 2 1 4" << endl;
	file << "f 1 3 4" << endl;
	file << "f 3 0 4" << endl;
	file.close();
}

void cube(const char* fileName){
	ofstream file;
	file.open(fileName);
	cout << "Writing to " << fileName << endl;
	file << "v 1 1 1" << endl;
	file << "v 1 1 -1" << endl;
	file << "v 1 -1 1" << endl;
	file << "v 1 -1 -1" << endl;
	file << "v -1 1 1" << endl;
	file << "v -1 1 -1" << endl;
	file << "v -1 -1 1" << endl;
	file << "v -1 -1 -1" << endl;
	file << "f 1 3 4 2" << endl;
	file << "f 5 7 8 6" << endl;
	file << "f 1 5 6 2" << endl;
	file << "f 3 7 8 4" << endl;
	file << "f 1 5 7 3" << endl;
	file << "f 2 6 8 4" << endl;
	file.close();
}


