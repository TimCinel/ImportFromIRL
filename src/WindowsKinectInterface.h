#ifndef WINDOWSKINECTINTERFACE_H
#define WINDOWSKINECTINTERFACE_H

#pragma once

#include "AbstractKinectInterface.h"
#include "NuiApi.h"

#include <cstdio>

class WindowsKinectInterface : public AbstractKinectInterface {
private:
    static const int DEPTH_WIDTH = 320, DEPTH_HEIGHT = 240, DEPTH_BYTES = 240;
	static const int COLOUR_WIDTH = DEPTH_WIDTH, COLOUR_HEIGHT = DEPTH_HEIGHT;
    static const int DUMP_FILE_LEN = 240;

    INuiSensor*			kinectSensor;

    HANDLE				kinectDepthStreamHandle, kinectColourStreamHandle;
    HANDLE      		kinectNextDepthFrameEvent, kinectNextColourFrameEvent;

    bool				isConnected;

    bool				dumping;
	char				dumpFile[DUMP_FILE_LEN];
    FILE*       		dumpFP;

    unsigned short*     depthData;
    unsigned char*		colourData;
    long*				colourCoords;


public:
    WindowsKinectInterface();
    ~WindowsKinectInterface();

	/* overriding abstract functions */
    virtual bool connectToKinect();
	virtual bool processFrame(KinectReceiver *kr);

	virtual bool startDump(char *filename);
	virtual bool endDump();

    USHORT *getDepthData() { return this->depthData; };
    int getDepthWidth() { return DEPTH_WIDTH; };
    int getDepthHeight() { return DEPTH_HEIGHT; };
};

#endif