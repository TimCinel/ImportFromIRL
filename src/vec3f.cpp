#include "vec3f.h"

vec3f operator+(const vec3f &v1, const vec3f &v2) {
	return vec3f(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

bool operator==(const vec3f &lhs, const float &rhs) {
	return rhs == (float)lhs;
}