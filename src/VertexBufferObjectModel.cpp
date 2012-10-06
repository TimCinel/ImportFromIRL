#include "VertexBufferObjectModel.h"
#include "GeometryGenerator.h"

#include <stdio.h>
#include <GL/glew.h>
#include <windows.h>
#include <GL/gl.h>


VertexBufferObjectModel::VertexBufferObjectModel(GeometryGenerator *shape, int resolution, int geometry) {
	this->initialise(shape, resolution, geometry);
}

VertexBufferObjectModel::~VertexBufferObjectModel() {
	if (NULL != this->vertices) 
		delete[] this->vertices;

	if (NULL != this->normals) 
		delete[] this->normals;

	if (0 != this->vertexBuffer)
		glDeleteBuffers(1, &(this->vertexBuffer));

	if (0 != this->normalBuffer)
		glDeleteBuffers(1, &(this->normalBuffer));
}

void VertexBufferObjectModel::initialise(GeometryGenerator *shape, int resolution, int geometry) {
	this->shape = shape;
	this->resolution = resolution;
	this->geometry = geometry;

	this->vertices = NULL;
	this->normals = NULL;

	this->vertexBuffer = 0;
	this->normalBuffer = 0;

	this->currentIndex = 0;
}

void VertexBufferObjectModel::draw() {
	
	vec3f *normal, *vertex;

	if (NULL == this->vertices || NULL == this->normals) {

        /* first draw - obtain geometry, create vertex arrays and VBOs */

		/* get data */
		this->shape->generate(this, this->resolution);


		/* vertices */
		glGenBuffers(1, &(this->vertexBuffer));
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->size * 3 * sizeof(float), this->vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		/* normals */
		glGenBuffers(1, &(this->normalBuffer));
		glBindBuffer(GL_ARRAY_BUFFER, this->normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->size * 3 * sizeof(float), this->normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	/* provide vertices */
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* provide normals */
	glBindBuffer(GL_ARRAY_BUFFER, this->normalBuffer);
	glNormalPointer(GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* draw */
	glDrawArrays(this->geometry, 0, this->size);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}

void VertexBufferObjectModel::specifyPoint(vec3f *vertex, vec3f *normal) {

	this->vertices[currentIndex] = *vertex;
	this->normals[currentIndex] = *normal;

	currentIndex++;
}

void VertexBufferObjectModel::specifySize(int size) {

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
