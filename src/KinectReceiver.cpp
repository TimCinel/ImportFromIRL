#include "KinectReceiver.h"

#include "RenderModel.h"

#include <cstdio>
#include <cmath>

KinectReceiver::KinectReceiver() :
	depth(NULL),
	width(0),
	height(0),
	x(0),
	y(0) { } 

KinectReceiver::~KinectReceiver() {
	//clean up dynamically-allocated memory
	if (NULL != this->depth) {
		for (int y = 0; y < this->height; y++)
			delete [] this->depth[y];

		delete [] this->depth;
		this->depth = NULL;

	}

}

void KinectReceiver::initialiseImage(int width, int height) {

	this->resetPointer();

	if (this->width == width && this->height == height)
		return;

	if (NULL != this->depth) {
		//depths already stored - delete them

		for (int y = 0; y < this->height; y++)
			//delete columns from this row
			delete [] (this->depth[y]);

		//delete rows
		delete [] this->depth;
	}

	this->width = width;
	this->height = height;

	//create rows
	this->depth = new vec3f*[this->height];

	for (int y = 0; y < this->height; y++)
		//create columns for this row
		this->depth[y] = new vec3f[this->width];
}

void KinectReceiver::resetPointer() {
	this->x = 0;
	this->y = 0;
}

void KinectReceiver::addPoint(unsigned short depth) {

	this->x = (this->x + 1) % this->width;
	if (0 == this->x) 
		this->y = (this->y + 1) % this->height;

	vec3f *point = &(this->depth[y][x]);

	static const float pi = 3.14159265f;
    static const float l_max = -2.0;
    static const float wide_angle = (57 / 360.0f) * 2.0f*pi;
    static const float high_angle = (43 / 360.0f) * 2.0f*pi;
    static const float wide_offset = -wide_angle / 2;
    static const float high_offset = -high_angle / 2;
    static const unsigned short max_depth = 0x4000;		

	point->x = l_max * sin(wide_offset + ((float)x / this->width) * wide_angle);
	point->y = l_max * sin(high_offset + ((float)y / this->height) * high_angle);
	point->z = l_max * 
		cos(wide_offset + ((float)x / this->width) * wide_angle) * 
		cos(high_offset + ((float)y / this->height) * high_angle);

	//scale 
	float scaleFactor = (float)depth / max_depth;

	point->x *= scaleFactor;
	point->y *= scaleFactor;
	point->z *= scaleFactor;

}

void KinectReceiver::generate(RenderModel *callingModel, int resolution) {

	//maximum number
	callingModel->specifySize(this->width * this->height);

	for (int y = 0; y < this->height - 1; y++)
		for (int x = 0; x < this->width - 1; x++) {

			this->specifyTriangle(callingModel, y, x, y+1, x+1, y+1, x);
			this->specifyTriangle(callingModel, y, x, y, x+1, y+1, x+1);

		}

}

void KinectReceiver::specifyTriangle(RenderModel *callingModel, 
									 int y1, int x1, 
									 int y2, int x2, 
									 int y3, int x3) 
{

#define THRESHOLD 0.01
	if (
	/*
		*/
		abs(this->depth[y1][x1].z - this->depth[y2][x2].z) > THRESHOLD ||
		abs(this->depth[y2][x2].z - this->depth[y3][x3].z) > THRESHOLD ||
		abs(this->depth[y1][x1].z - this->depth[y3][x3].z) > THRESHOLD ||
		this->depth[y1][x1].z == 0 ||
		this->depth[y2][x2].z == 0 ||
		this->depth[y3][x3].z == 0
	)
		//the triangle's too big
		return;

	callingModel->specifyPoint(&(this->depth[y1][x1]), NULL);
	callingModel->specifyPoint(&(this->depth[y2][x2]), NULL);
	callingModel->specifyPoint(&(this->depth[y3][x3]), NULL);
}

