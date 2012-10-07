#ifndef VEC3F_H
#define VEC3F_H

#define _USE_MATH_DEFINES
#include <cmath>

class vec3f {
public:
	vec3f() : x(0.0), y(0.0), z(0.0) {}
	vec3f(float x, float y, float z) : x(x), y(y), z(z) {}
	float x, y, z;

	bool operator==(float &rhs) {
		return rhs == (float)(*this);
	}

	operator float() {
		return sqrt(pow(this->x,2) + pow(this->y,2) + pow(this->z,2));
	}
};

#endif