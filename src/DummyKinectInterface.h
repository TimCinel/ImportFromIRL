#ifndef DUMMYKINECTINTERFACE_H
#define DUMMYKINECTINTERFACE_H

#pragma once

#include "AbstractKinectInterface.h"
#include <stdio.h>

class DummyKinectInterface : public AbstractKinectInterface {
public:
    DummyKinectInterface(int width, int height, char *dumpFile);
    ~DummyKinectInterface();

	/* overriding abstract functions */
    virtual bool connectToKinect();
	virtual bool processDepth(KinectReceiver *kr);

private:
	char *dumpFile;
	FILE *dumpFP;

	int width;
	int height;
};

#endif