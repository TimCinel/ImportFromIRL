#ifndef DUMMYKINECTINTERFACE_H
#define DUMMYKINECTINTERFACE_H

#pragma once

#include "AbstractKinectInterface.h"
#include <stdio.h>

class DummyKinectInterface : public AbstractKinectInterface {
private:
	char *dumpFile;
	FILE *dumpFP;

	unsigned short *depthData;
	unsigned char *colourData;

	int width;
	int height;

public:
    DummyKinectInterface(int width, int height, char *dumpFile);
    ~DummyKinectInterface();

	/* overriding abstract functions */
    virtual bool connectToKinect();
	virtual bool processFrame(KinectReceiver *kr);

	virtual bool startDump(char *filename) { return false; }
	virtual bool endDump() {return false; }
};

#endif