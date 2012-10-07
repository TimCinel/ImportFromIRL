#include "GeometryOperations.h"

#include "../lib/eigen/Eigen/Dense";

using namespace Eigen;
using namespace std;

vec3f GeometryOperations::rotate3D(vec3f *point, vec3f *rotation) {
	float x = rotation->x, y = rotation->y, z = rotation->z;

	Vector3f v = Vector3f(point->x, point->y, point->z);
	Matrix3f r = Matrix3f();

	r <<	cos(y)*cos(z),	-cos(x)*sin(z)+sin(x)*sin(y)*cos(z),	sin(x)*sin(z)+cos(x)*sin(y)*cos(z),
			cos(y)*sin(z),	cos(x)*cos(z)+sin(x)*sin(y)*sin(z),		-sin(x)*cos(z)+cos(x)*sin(y)*sin(z),
			-sin(y),		sin(x)*cos(y),							cos(x)*cos(y);
			

	Vector3f result = r*v;

	return vec3f(result[0], result[1], result[2]);
}