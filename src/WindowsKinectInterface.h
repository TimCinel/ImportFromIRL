#ifndef WINDOWSKINECTINTERFACE_H
#define WINDOWSKINECTINTERFACE_H

#pragma once

#include "AbstractKinectInterface.h"
#include "NuiApi.h"

#include <cstdio>

class WindowsKinectInterface : public AbstractKinectInterface {
private:
    static const int DEPTH_WIDTH = 320;
    static const int DEPTH_HEIGHT = 240;
    static const int DUMP_FILE_LEN = 240;

    INuiSensor *kinectSensor;

    HANDLE      kinectDepthStreamHandle;
    HANDLE      kinectNextFrameEvent;

    bool        isConnected;

    bool        dumping;
	char		dumpFile[DUMP_FILE_LEN];
    FILE        *dumpFP;

    USHORT      *depthData;


public:
    WindowsKinectInterface();
    ~WindowsKinectInterface();

	/* overriding abstract functions */
    virtual bool connectToKinect();
	virtual bool processDepth(KinectReceiver *kr);

	virtual bool startDump(char *filename);
	virtual bool endDump();

    USHORT *getDepthData() { return this->depthData; };
    int getDepthWidth() { return DEPTH_WIDTH; };
    int getDepthHeight() { return DEPTH_HEIGHT; };
};

#endif