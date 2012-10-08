#ifndef CULLPLANE_H
#define CULLPLANE_H

#include "vec3f.h"
#include "GeometryGenerator.h"
#include "RenderModel.h"

class CullPlane : public GeometryGenerator {
private:
	vec3f origNormal, normal;
	vec3f origOffset, offset;
    vec3f origSquare[4], square[4];
	vec3f totalRotation;
	float d;
public:

    CullPlane() : 
        normal(vec3f(1.0, 0.0, 0.0)), 
        origNormal(vec3f(1.0, 0.0, 0.0)), 
        offset(vec3f()),
        origOffset(vec3f()),
		totalRotation(vec3f())
		{ this->init(); }

    void init();
    void rotate(vec3f &amount);
	bool cullPoint(vec3f &point);

	//override GeometryGenerator
	virtual void generate(RenderModel *callingModel, int resolution);

};

#endif
