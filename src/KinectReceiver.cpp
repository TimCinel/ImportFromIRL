#include "KinectReceiver.h"

#include "RenderModel.h"
#include "GeometryOperations.h"

#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>

const float KinectReceiver::DIFFERENCE_THRESHOLD = 0.05f;

KinectReceiver::KinectReceiver() { 
	this->init();
} 

KinectReceiver::~KinectReceiver() {
	this->cleanUp();
}

void KinectReceiver::init() {
	this->cleanUp();
}

void KinectReceiver::initialiseImage(int width, int height) {
	this->cleanUp();

	this->width = width;
	this->height = height;

	vec3f zero(0.0, 0.0, 0.0);

    //initialise the arrays
	this->verts = vector<vec3f>(this->height * this->width, zero);
	this->norms = vector<vec3f>(this->height * this->width, zero);
	this->vertTriMap = vector<int>(this->height * this->width * MAX_TRIS_PER_VERT, TRI_MAP_INVALID_VAL);
	this->tris = vector<unsigned int>((this->height - 1) * (this->width - 1) * MAX_TRIS_PER_VERT, 0);
	this->triNorms = vector<vec3f>((this->height - 1) * (this->width - 1) * 2, zero);

}

void KinectReceiver::resetPointer() {
	this->pos = 0;
	this->triCount = 0;
}

void KinectReceiver::cleanUp() {
	//clean up dynamically-allocated memory
	this->verts.clear();
	this->norms.clear();
	this->vertTriMap.clear();
	this->tris.clear();
	this->triNorms.clear();

	this->triCount = 0;
	this->width = 0;
	this->height = 0;
	this->pos = 0;
}

void KinectReceiver::addPoint(unsigned short depth) {

	vec3f *point = &(this->verts[this->pos++]);

	int x = this->pos % this->width;
	int y = this->pos / this->width;

	static const float pi = 3.14159265f;
    static const float l_max = -2.0;
    static const vec3f offset(0.0, 0.0, 1.0);
    static const float wide_angle = (57 / 360.0f) * 2.0f*pi;
    static const float high_angle = (43 / 360.0f) * 2.0f*pi;
    static const float wide_offset = -wide_angle / 2;
    static const float high_offset = -high_angle / 2;
    static const unsigned short max_depth = 0x1000;		

	point->x = l_max * 
        sin(wide_offset + ((float)x / this->width) * wide_angle);
	point->y = l_max * 
        sin(high_offset + ((float)y / this->height) * high_angle);
	point->z = l_max * 
		cos(wide_offset + ((float)x / this->width) * wide_angle) * 
		cos(high_offset + ((float)y / this->height) * high_angle);

	//scale point based on depth
	float scaleFactor = (float)depth / max_depth;

	point->x = point->x * scaleFactor + offset.x;
	point->y = point->y * scaleFactor + offset.y;
	point->z = point->z * scaleFactor + offset.z;




	if (this->pos == this->height * this->width - 1) {

		//get triangles based on vertexes
		for (int y = 0; y < this->height - 1; y++) {
			for (int x = 0; x < this->width - 1; x++) {
				this->specifyTriangle(y, x, y+1, x+1, y+1, x);
				this->specifyTriangle(y, x, y, x+1, y+1, x+1);
			}
		}

		//get normals for each vertex
		for (int i = 0; i < this->height * this->width; i++) {
			vec3f *normal = &(this->norms[i]);
			normal->x = normal->y = normal->z = 0.0;

			int j = i * 6;
			int jMax = j + 6;

			//average all triangle surface normals associated with this vertex
			while (this->vertTriMap[j] >= 0 && j < jMax) {
				normal->x += this->triNorms[this->vertTriMap[j]].x;
				normal->y += this->triNorms[this->vertTriMap[j]].y;
				normal->z += this->triNorms[this->vertTriMap[j]].z;
				j++;
			}

			//normalize vector
			float magnitude = sqrt(pow(normal->x, 2) + pow(normal->y, 2) + pow(normal->z, 2));
			normal->x /= magnitude;
			normal->y /= magnitude;
			normal->z /= magnitude;

			//next vertex
		}
	}
}

void KinectReceiver::generate(RenderModel *callingModel, int resolution) {

	//number of points
	callingModel->specifySize(this->triCount * 3);

	int triangleNum;
	for (triangleNum = 0; triangleNum < this->triCount - 1; triangleNum++)
		this->drawTriangle(callingModel, triangleNum);
	return;

}

void KinectReceiver::specifyTriangle(int y0, int x0, 
									 int y1, int x1, 
									 int y2, int x2) 
{
	int indices[3];
	vec3f *p[3];
	p[0] = &(this->verts[indices[0] = y0 * this->width + x0]);
	p[1] = &(this->verts[indices[1] = y1 * this->width + x1]);
	p[2] = &(this->verts[indices[2] = y2 * this->width + x2]);

	if (
		//stop LONNNNNG objects
		abs(p[0]->x - p[1]->x) > DIFFERENCE_THRESHOLD ||
		abs(p[0]->y - p[1]->y) > DIFFERENCE_THRESHOLD || 
		abs(p[0]->z - p[1]->z) > DIFFERENCE_THRESHOLD || 
		abs(p[1]->x - p[2]->x) > DIFFERENCE_THRESHOLD || 
		abs(p[1]->y - p[2]->y) > DIFFERENCE_THRESHOLD || 
		abs(p[1]->z - p[2]->z) > DIFFERENCE_THRESHOLD || 
		abs(p[2]->x - p[0]->x) > DIFFERENCE_THRESHOLD || 
		abs(p[2]->y - p[0]->y) > DIFFERENCE_THRESHOLD || 
		abs(p[2]->z - p[0]->z) > DIFFERENCE_THRESHOLD || 
		p[0]->z == 0 || p[1]->z == 0 || p[2]->z == 0
	)
		//the triangle's a PITA
		return;

	//record this triangle
	for (int i = 0; i < 3; i++) {
		int j = indices[i] * 6;
		int jMax = j + 6 - 1;
		while (this->vertTriMap[j] >= 0 && j < jMax)
			j++;

		this->vertTriMap[j] = triCount;
	}


	//calculate surface normal (Cross product of p[0]_\p[1] and p[1]_\p[2])
	vec3f *normal = &(this->triNorms[triCount]);
	normal->x = ((p[0]->y - p[1]->y) * (p[1]->z - p[2]->z) - (p[0]->z - p[1]->z) * (p[1]->y - p[2]->y));
	normal->y = ((p[0]->z - p[1]->z) * (p[1]->x - p[2]->x) - (p[0]->x - p[1]->x) * (p[1]->z - p[2]->z));
	normal->z = ((p[0]->x - p[1]->x) * (p[1]->y - p[2]->y) - (p[0]->y - p[1]->y) * (p[1]->x - p[2]->x));

	int triPos = this->triCount * 3;
	this->tris[triPos + 0] = y0 * this->width + x0;
	this->tris[triPos + 1] = y1 * this->width + x1;
	this->tris[triPos + 2] = y2 * this->width + x2;

	this->triCount++;

}

void KinectReceiver::drawTriangle(RenderModel *callingModel, int triangleNum)  {
	triangleNum *= 3;
	vec3f point = this->verts[this->tris[triangleNum]];

	//cull
	for (int i = 0; i < this->planes.size(); i++)
		if (this->planes[i].cullPoint(point))
			return;

	for (int i = 0; i < 3; i++)
		callingModel->specifyPoint(
			&(this->verts[this->tris[triangleNum + i]]), 
			&(this->norms[this->tris[triangleNum + i]])
		);

}
