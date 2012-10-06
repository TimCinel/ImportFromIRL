#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

class RenderModel;

class GeometryGenerator {
public:
	RenderModel *callingModel;
	virtual void generate(RenderModel *callingModel, int resolution) = 0;
};

#endif