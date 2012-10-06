#include "VertexArrayModel.h"
#include "GeometryGenerator.h"

#include <stdio.h>
#include <windows.h>
#include <GL/gl.h>


VertexArrayModel::VertexArrayModel(GeometryGenerator *shape, int resolution, int geometry) {
	this->initialise(shape, resolution, geometry);
}

VertexArrayModel::~VertexArrayModel() {
	if (NULL != this->vertices) 
		delete[] this->vertices;

	if (NULL != this->normals) 
		delete[] this->normals;
}

void VertexArrayModel::initialise(GeometryGenerator *shape, int resolution, int geometry) {
	this->shape = shape;
	this->resolution = resolution;
	this->geometry = geometry;

	this->vertices = NULL;
	this->normals = NULL;

	this->currentIndex = 0;
}

void VertexArrayModel::draw() {
	
	vec3f *normal, *vertex;

	if (NULL == this->vertices || NULL == this->normals)
		this->shape->generate(this, this->resolution);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, this->vertices);
	glNormalPointer(GL_FLOAT, 0, this->normals);

	glDrawArrays(this->geometry, 0, this->currentIndex);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}

void VertexArrayModel::specifyPoint(vec3f *vertex, vec3f *normal) {

	this->vertices[currentIndex] = *vertex;
	this->normals[currentIndex] = *normal;

	currentIndex++;
}

void VertexArrayModel::specifySize(int size) {

	/* clear any stored values */
	if (NULL != this->vertices) 
		delete[] this->vertices;

	if (NULL != this->normals) 
		delete[] this->normals;

	/* create space for vertices and normals */
	this->vertices = new vec3f[size];
	this->normals = new vec3f[size];

	this->size = size;
	this->currentIndex = 0;

}
