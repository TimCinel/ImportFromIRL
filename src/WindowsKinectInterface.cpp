//#include <stdafx.h>
#include "WindowsKinectInterface.h"

WindowsKinectInterface::WindowsKinectInterface() :
    kinectSensor(NULL),
    kinectDepthStreamHandle(NULL),
    kinectColourStreamHandle(NULL),
    kinectNextDepthFrameEvent(NULL),
    kinectNextColourFrameEvent(NULL),
    isConnected(false),
    dumping(false),
    dumpFP(NULL)
{
    this->depthData = new unsigned short[DEPTH_WIDTH * DEPTH_HEIGHT];
    this->colourData = new unsigned char[DEPTH_WIDTH * DEPTH_HEIGHT * COLOUR_BYTES];
	//this->colourCoords = new long[DEPTH_WIDTH * DEPTH_HEIGHT *2];

    //memset(this->depthData, 0, sizeof(unsigned short)*DEPTH_WIDTH*DEPTH_HEIGHT);
    //memset(this->colourData, 0, sizeof(unsigned short)*DEPTH_WIDTH*DEPTH_HEIGHT);
    //memset(this->dumpFile, 0, DUMP_FILE_LEN);
}

WindowsKinectInterface::~WindowsKinectInterface() {

    //shutdown kinect
    if (this->kinectSensor)
        this->kinectSensor->NuiShutdown();

    //close event handle
    if (this->kinectNextDepthFrameEvent != INVALID_HANDLE_VALUE)
        CloseHandle(this->kinectNextDepthFrameEvent);

    //close event handle
    if (this->kinectNextColourFrameEvent != INVALID_HANDLE_VALUE)
        CloseHandle(this->kinectNextColourFrameEvent);

    //free depth memory
    if (NULL != this->depthData)
        delete[] this->depthData;

    //free depth memory
    if (NULL != this->depthData)
		delete[] this->colourData;

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

		//initialise sensor for depth and colour
        hr = this->kinectSensor->NuiInitialize(
			NUI_INITIALIZE_FLAG_USES_DEPTH | 
			NUI_INITIALIZE_FLAG_USES_COLOR); 

        if (SUCCEEDED(hr)) {
            //create an event to handle "depth data available" notifications
            this->kinectNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            this->kinectNextColourFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

			//open depth image stream and start receiving frames
            hr = this->kinectSensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_DEPTH,
                (DEPTH_WIDTH == 640 ? NUI_IMAGE_RESOLUTION_640x480 : NUI_IMAGE_RESOLUTION_320x240),
                0,
                2,
                this->kinectNextDepthFrameEvent,
                &this->kinectDepthStreamHandle);

			//open colour image stream and start receiving frames
            hr = this->kinectSensor->NuiImageStreamOpen(
				NUI_IMAGE_TYPE_COLOR,
                (DEPTH_WIDTH == 640 ? NUI_IMAGE_RESOLUTION_640x480 : NUI_IMAGE_RESOLUTION_320x240),
                0,
                2,
                this->kinectNextColourFrameEvent,
                &this->kinectColourStreamHandle);
        }

		//enable near mode  
		hr = this->kinectSensor->NuiImageStreamSetImageFrameFlags(this->kinectDepthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);

    }

    if (NULL == this->kinectSensor || FAILED(hr))
        return false;

    return true;
}

bool WindowsKinectInterface::processFrame(KinectReceiver *kr) {

    HRESULT hr;
    NUI_IMAGE_FRAME imageFrame;
    NUI_LOCKED_RECT LockedRect;


    //--DEPTH--
    hr = this->kinectSensor->NuiImageStreamGetNextFrame(this->kinectDepthStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
        return false;

    //lock the frame data
    imageFrame.pFrameTexture->LockRect(0, &LockedRect, NULL, 0);

	//copy depth data
	memcpy(
		this->depthData, 
		LockedRect.pBits, 
		min(LockedRect.size, DEPTH_WIDTH * DEPTH_HEIGHT * DEPTH_BYTES)
		);

    //unlock the frame
	hr = imageFrame.pFrameTexture->UnlockRect(0);
    if (FAILED(hr))
        return false;

    //release the frame
    this->kinectSensor->NuiImageStreamReleaseFrame(this->kinectDepthStreamHandle, &imageFrame);
    if (FAILED(hr))
        return false;



	//--COLOUR--
	hr = this->kinectSensor->NuiImageStreamGetNextFrame(this->kinectColourStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
		return false; 
    
    hr = imageFrame.pFrameTexture->LockRect(0, &LockedRect, NULL, 0);
    if (FAILED(hr))
		return false; 

	memcpy(
		this->colourData, 
		LockedRect.pBits, 
		min(LockedRect.size, COLOUR_WIDTH * COLOUR_HEIGHT * COLOUR_BYTES)
		);

    hr = imageFrame.pFrameTexture->UnlockRect(0);
    if (FAILED(hr))
		return false; 

	hr = this->kinectSensor->NuiImageStreamReleaseFrame(this->kinectColourStreamHandle, &imageFrame);
    if (FAILED(hr))
		return false; 


	//reset the receiver object
	kr->initialiseImage(DEPTH_WIDTH, DEPTH_HEIGHT);
	kr->addPoints(depthData, colourData);

	if (this->dumping && NULL != this->dumpFP) {
		//dump the depth data to file
		//pffffppfpppptthpfp (that's the sound of dumping)
		fwrite(
			this->depthData, 
			sizeof(unsigned short), 
			DEPTH_WIDTH * DEPTH_HEIGHT, 
			this->dumpFP
			);

		fwrite(
			this->colourData, 
			sizeof(unsigned char), 
			COLOUR_WIDTH * COLOUR_HEIGHT * COLOUR_BYTES , 
			this->dumpFP
			);
	}

	//release the receiver
    this->kinectSensor->NuiImageStreamReleaseFrame(this->kinectColourStreamHandle, &imageFrame);

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