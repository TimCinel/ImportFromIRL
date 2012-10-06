#ifndef WINDOWSKINECTINTERFACE_H
#define WINDOWSKINECTINTERFACE_H

#pragma once

#include "AbstractKinectInterface.h"
#include "NuiApi.h"

class WindowsKinectInterface : public AbstractKinectInterface {
public:

    WindowsKinectInterface();
    ~WindowsKinectInterface();

	/* overriding abstract functions */
    virtual bool connectToKinect();
	virtual bool processDepth(KinectReceiver *kr);

    USHORT *getDepthData() { return this->depthData; };
    int getDepthWidth() { return this->depthWidth; };
    int getDepthHeight() { return this->depthHeight; };

private:
    INuiSensor *kinectSensor;

    HANDLE      kinectDepthStreamHandle;
    HANDLE      kinectNextFrameEvent;

    bool        isConnected;

    USHORT      *depthData;

    static const int depthWidth = 320;
    static const int depthHeight = 240;
};

#endif