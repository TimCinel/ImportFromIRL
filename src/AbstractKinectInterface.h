#ifndef ABSTRACTKINECTINTERFACE_H
#define ABSTRACTKINECTINTERFACE_H

#include "KinectReceiver.h"

class AbstractKinectInterface {
public:
    static const int COLOUR_BYTES = 4;

	virtual bool connectToKinect() = 0;
	virtual bool processFrame(KinectReceiver *kr) = 0;

	//can simply return false if not allowed
	virtual bool startDump(char *filename) = 0;
	virtual bool endDump() = 0;
};

#endif