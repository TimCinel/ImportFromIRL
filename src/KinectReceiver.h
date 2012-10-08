#ifndef KINECTRECEIVER_H
#define KINECTRECEIVER_H

#pragma once

#include "GeometryGenerator.h"
#include "CullPlane.h"

#include <vector>
using namespace std;

class KinectReceiver : public GeometryGenerator {
private:
	vector<vec3f> verts;
	vector<vec3f> norms;
	vector<int> vertTriMap;
	vector<unsigned int> tris;
	vector<vec3f> triNorms;

	//model offset
	vec3f translation;
	vector<CullPlane> planes;

	int width, height, triCount;
	int pos;

	static const int MAX_TRIS_PER_VERT = 6;
	static const int TRI_MAP_INVALID_VAL = -1;
	const static float DIFFERENCE_THRESHOLD;

public:
	KinectReceiver();
	KinectReceiver(KinectReceiver *kr);

	~KinectReceiver();

	//inherited from GeometryGenerator
	virtual void generate(RenderModel *callingModel, int resolution);

	//used by AbstractKinectInterface
	void initialiseImage(int width, int height);
	void resetPointer();
	void addPoint(unsigned short depth);

	//culling plane stuff
	void addPlane(CullPlane plane) { this->planes.push_back(plane); }
	vector<CullPlane> *getPlanes() { return &(this->planes); }

	int getWidth() { return this->width; }
	int getHeight() { return this->height; }
	int getTriCount() { return this->triCount; }
	
	vec3f *getTranslation() { return &(this->translation); };

private:
	void init();
	void cleanUp();

	void specifyTriangle(int y1, int x1, 
						 int y2, int x2, 
						 int y3, int x3);

	void drawTriangle(RenderModel *callingModel, int triangleNum);
};

#endif
