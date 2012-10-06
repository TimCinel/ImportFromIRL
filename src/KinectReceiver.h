#ifndef KINECTRECEIVER_H
#define KINECTRECEIVER_H

#pragma once

#include "GeometryGenerator.h"
#include "vec3f.h"

class KinectReceiver : public GeometryGenerator {
public:
	KinectReceiver();
	~KinectReceiver();

	void initialiseImage(int width, int height);
	void resetPointer();

	void addPoint(unsigned short depth);

	//inherited from GeometryGenerator
	virtual void generate(RenderModel *callingModel, int resolution);

private:
	vec3f **depth;
	int width, height;

	//keep track of current point
	int x, y;
};

#endif