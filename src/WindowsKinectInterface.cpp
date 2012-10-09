//#include <stdafx.h>
#include "WindowsKinectInterface.h"

WindowsKinectInterface::WindowsKinectInterface() :
    kinectSensor(NULL),
    kinectDepthStreamHandle(NULL),
    kinectNextFrameEvent(NULL),
    isConnected(false),
    dumping(false),
    dumpFP(NULL)
{
    this->depthData = new unsigned short[DEPTH_WIDTH * DEPTH_HEIGHT];
    memset(this->depthData, 0, sizeof(unsigned short)*DEPTH_WIDTH*DEPTH_HEIGHT);
    memset(this->dumpFile, 0, DUMP_FILE_LEN);
}

WindowsKinectInterface::~WindowsKinectInterface() {

    //shutdown kinect
    if (this->kinectSensor)
        this->kinectSensor->NuiShutdown();

    //close event handle
    if (this->kinectNextFrameEvent != INVALID_HANDLE_VALUE)
        CloseHandle(this->kinectNextFrameEvent);

    //free depth memory
    if (NULL != this->depthData)
        delete[] this->depthData;

}

/// Create the first connected Kinect found 
bool WindowsKinectInterface::connectToKinect() {
    INuiSensor * pNuiSensor;
    HRESULT hr;

    int iSensorCount = 0;
    hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
        return false;

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i) {

        //create sensor i
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);

        if (FAILED(hr))
            continue;

		//check status of sensor
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr) {
			//all good
            this->kinectSensor = pNuiSensor;
            break;
        }

		//no good - check next sensor
        pNuiSensor->Release();
    }

    if (NULL != this->kinectSensor) {

		//initialise sensor for depth
        hr = this->kinectSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH); 

        if (SUCCEEDED(hr)) {
            //create an event to handle "depth data available" notifications
            this->kinectNextFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

			//open image stream and start receiving frames
            hr = this->kinectSensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_DEPTH,
                (DEPTH_WIDTH == 640 ? NUI_IMAGE_RESOLUTION_640x480 : NUI_IMAGE_RESOLUTION_320x240),
                0,
                2,
                this->kinectNextFrameEvent,
                &this->kinectDepthStreamHandle);
        }
    }

    if (NULL == this->kinectSensor || FAILED(hr))
        return false;

    return true;
}

bool WindowsKinectInterface::processDepth(KinectReceiver *kr) {
    HRESULT hr;
    NUI_IMAGE_FRAME imageFrame;

    //get the depth frame
    hr = this->kinectSensor->NuiImageStreamGetNextFrame(this->kinectDepthStreamHandle, 0, &imageFrame);

    if (FAILED(hr))
        return false;

    INuiFrameTexture* pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;

    //lock the frame data
    pTexture->LockRect(0, &LockedRect, NULL, 0);

    // Make sure we have somewhere to store the data
    if (NULL == this->depthData)
        return false;

    // Make sure we've received valid data
    if (LockedRect.Pitch != 0) {
        USHORT *pixelData = this->depthData;

        const USHORT *depthBuffer = (const USHORT *)LockedRect.pBits;
        const USHORT *depthBufferEnd = depthBuffer + (DEPTH_WIDTH * DEPTH_HEIGHT);

		//reset receiver
		kr->initialiseImage(DEPTH_WIDTH, DEPTH_HEIGHT);

        while (depthBuffer < depthBufferEnd) {
			kr->addPoint(NuiDepthPixelToDepth(*depthBuffer));

            if (this->dumping)
                *pixelData = NuiDepthPixelToDepth(*depthBuffer);

            pixelData++;
            depthBuffer++;
        }

        if (this->dumping && NULL != this->dumpFP) {
            //dump the depth data to file
            //pffffppfpppptthpfp (that's the sound of dumping)
            fwrite(this->depthData, sizeof(unsigned short), DEPTH_WIDTH * DEPTH_HEIGHT, this->dumpFP);
        }

    }

    //unlock the frame
    pTexture->UnlockRect(0);

    //release the frame
    this->kinectSensor->NuiImageStreamReleaseFrame(this->kinectDepthStreamHandle, &imageFrame);

	return true;
}

bool WindowsKinectInterface::endDump() {
	this->dumping = false;

    if (NULL != this->dumpFP)
        fclose(this->dumpFP);
    this->dumpFP = NULL;

	return true;
}

bool WindowsKinectInterface::startDump(char *filename) {
	this->dumping = true;
    strcpy(this->dumpFile, filename);

    //wb for write binary. just w doesn't work in Vindowze...
    this->dumpFP = fopen(this->dumpFile, "wb");

	return true;
}