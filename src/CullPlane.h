#ifndef CULLPLANE_H
#define CULLPLANE_H

#include "vec3f.h"

class CullPlane {
private:
	vec3f normal;
public:
	CullPlane(vec3f normal) : normal(normal) {};
};

#endif