#ifndef VEC3F_H
#define VEC3F_H

#define _USE_MATH_DEFINES
#include <cmath>

class vec3f {
public:
	vec3f() : x(0.0), y(0.0), z(0.0) {}
	vec3f(float x, float y, float z) : x(x), y(y), z(z) {}
	float x, y, z;

	operator float() const {
		return (float)(sqrt(pow(this->x,2) + pow(this->y,2) + pow(this->z,2)));
	}

};

vec3f operator+(const vec3f &v1, const vec3f &v2);
bool operator==(const vec3f &lhs, const float &rhs);

#endif