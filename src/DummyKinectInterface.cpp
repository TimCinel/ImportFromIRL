#include "DummyKinectInterface.h"

#include <stdio.h>

DummyKinectInterface::DummyKinectInterface(int width, int height, char *dumpFile) : 
	dumpFile(dumpFile),
	dumpFP(NULL),
	width(width),
	height(height)
{}

DummyKinectInterface::~DummyKinectInterface() {
	if (NULL == this->dumpFP) {
		fclose(this->dumpFP);
		this->dumpFP = NULL;
	}
}

// overriding abstract functions
bool DummyKinectInterface::connectToKinect() {
	//open dummy file
	this->dumpFP = fopen(this->dumpFile, "rb");

	return (NULL != this->dumpFP);
}

bool DummyKinectInterface::processDepth(KinectReceiver *kr) {
	//just return dummy data from current buffer

	//reset the receiver
	kr->initialiseImage(this->width, this->height);

	//make some space for the depth data
	int numItems = this->width * this->height;
	unsigned short *depthData;
	depthData = new unsigned short[numItems];

	//fetch the next frame of depth data
	int attempt = 0;

	while (true) {
		int read = fread(depthData, sizeof(unsigned short), numItems, this->dumpFP);
		if (numItems == read) 
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

	for (int i = 0; i < numItems; i++)
		kr->addPoint(depthData[i]);

	delete [] depthData;

}