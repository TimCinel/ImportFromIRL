#ifndef ABSTRACTKINECTINTERFACE_H
#define ABSTRACTKINECTINTERFACE_H

#include "KinectReceiver.h"

class AbstractKinectInterface {
public:
	virtual bool connectToKinect() = 0;
	virtual bool processDepth(KinectReceiver *kr) = 0;
};

#endif