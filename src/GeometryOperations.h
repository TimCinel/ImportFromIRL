#ifndef GEOMETRYOPERATIONS_H
#define GEOMETRYOPERATIONS_H

#include "vec3f.h"

using namespace std;

class GeometryOperations {
public:
	static vec3f rotate3D(vec3f &point, vec3f &rotation);
};

#endif