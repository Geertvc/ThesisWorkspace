/*
 * Tuple3f.h
 *
 *  Created on: Jan 7, 2013
 *      Author: thypo
 */

#ifndef TUPLE3F_H_
#define TUPLE3F_H_

namespace std {

class Tuple3f {
public:
	Tuple3f(): x(0.f), y(0.f), z(0.f){}
	Tuple3f(const float x, const float y, const float z): x(x), y(y), z(z){}
	~Tuple3f();
	float x,y,z;
};

} /* namespace std */
#endif /* TUPLE3F_H_ */
