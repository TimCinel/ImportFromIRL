#ifndef RENDERMODEL_H
#define RENDERMODEL_H

#include "GeometryGenerator.h"
#include "vec3f.h"

class RenderModel {
public:
	GeometryGenerator *shape;
	int resolution;
	int geometry;
	int size;

	virtual ~RenderModel() {}

	virtual void initialise(GeometryGenerator *shape, int resolution, int geometry) = 0;

	virtual void draw() = 0;

	virtual void specifyPoint(vec3f *vertex, vec3f *normal) = 0;
	virtual void specifySize(int size) = 0;
};

#endif