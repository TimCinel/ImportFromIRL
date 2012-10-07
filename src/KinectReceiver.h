#ifndef KINECTRECEIVER_H
#define KINECTRECEIVER_H

#pragma once

#include "GeometryGenerator.h"
#include "CullPlane.h"

#include <vector>

class KinectReceiver : public GeometryGenerator {
private:
	vec3f *verts;
	vec3f *norms;
	int *vertTriMap;
	unsigned int *tris;
	vec3f *triNorms;

	std::vector<CullPlane> planes;

	int width, height, triCount;
	int pos;

public:
	KinectReceiver();
	~KinectReceiver();

	void initialiseImage(int width, int height);
	void resetPointer();

	void addPlane(CullPlane plane);
	std::vector<CullPlane> getPlanes();

	void addPoint(unsigned short depth);

	//inherited from GeometryGenerator
	virtual void generate(RenderModel *callingModel, int resolution);

private:
	void cleanUp();

	void specifyTriangle(int y1, int x1, 
						 int y2, int x2, 
						 int y3, int x3);

	void drawTriangle(RenderModel *callingModel, int triangleNum);
};

#endif