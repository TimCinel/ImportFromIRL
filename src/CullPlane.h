#ifndef CULLPLANE_H
#define CULLPLANE_H

#pragma once

#include "vec3f.h"
#include "GeometryGenerator.h"
#include "PanTarget.h"

class CullPlane : public ObjectModel, public PanTarget {
private:
	vec3f origNormal, normal;
	vec3f offset;
    vec3f origSquare[4], square[4];
	vec3f totalRotation;
	float d;
public:

    CullPlane() : 
        normal(vec3f(1.0, 0.0, 0.0)), 
        origNormal(vec3f(1.0, 0.0, 0.0)), 
        offset(vec3f()),
		totalRotation(vec3f())
		{ this->init(); }

    void init();

    void rotate(vec3f amount);

    //PanTarget
    virtual void pan(vec3f amount);

	bool cullPoint(vec3f point);

	//override ObjectModel
	virtual void drawThis();

private:
	void calculateD();

};

#endif
