#include "DummyKinectInterface.h"

#include <stdio.h>

DummyKinectInterface::DummyKinectInterface(int width, int height, const char *dumpFile) : 
	dumpFile(dumpFile),
	dumpFP(NULL),
	depthData(NULL),
	colourData(NULL),
	width(width),
	height(height)
{}

DummyKinectInterface::~DummyKinectInterface() {
	if (NULL != this->depthData)
		delete [] this->depthData;

	if (NULL != this->colourData)
		delete [] this->colourData;

	if (NULL == this->dumpFP)
		fclose(this->dumpFP);
}

// overriding abstract functions
bool DummyKinectInterface::connectToKinect() {
	//open dummy file
	this->dumpFP = fopen(this->dumpFile, "rb");

	return (NULL != this->dumpFP);
}

bool DummyKinectInterface::processFrame(KinectReceiver *kr) {
	//just return dummy data from current buffer

	//reset the receiver
	kr->initialiseImage(this->width, this->height);

	//make some space for the depth data
	int depthItems = this->width * this->height;
	int colourItems = this->width * this->height * COLOUR_BYTES;

	if (NULL == this->depthData)
		this->depthData = new unsigned short[depthItems];

	if (NULL == this->colourData)
		this->colourData = new unsigned char[colourItems];

	//fetch the next frame of depth data
	int attempt = 0;

	while (true) {

		int depthRead = fread(
			this->depthData, 
			sizeof(unsigned short), 
			depthItems, 
			this->dumpFP
			);

		int colourRead = fread(
			this->colourData, 
			sizeof(unsigned char), 
			colourItems, 
			this->dumpFP
			);

		if (depthItems == depthRead && colourItems == colourRead) 
			//successfully read the expected number of items
			break;
		else if (attempt++ < 1) 
			//read fewer items than expected (potentially zero)
			//probably reached the end of the dump file - rewind!
			rewind(this->dumpFP);
		else
			//failed second time
			return false;
	}

	kr->addPoints(depthData, colourData);

	return true;

}
