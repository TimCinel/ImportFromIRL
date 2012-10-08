#ifndef CULLPLANE_H
#define CULLPLANE_H

#include "vec3f.h"
#include "GeometryGenerator.h"
#include "RenderModel.h"

class CullPlane : public GeometryGenerator {
public:
	vec3f normal;
	vec3f offset;
    vec3f square[4];

	CullPlane(vec3f normal, vec3f offset) : 
        normal(normal), 
        offset(offset) { this->init(); }

    CullPlane() : 
        normal(vec3f(1.0, 0.0, 0.0)), 
        offset(vec3f()) { this->init(); }

    void init();
    void rotate(vec3f amount);
	virtual void generate(RenderModel *callingModel, int resolution);

};

#endif
