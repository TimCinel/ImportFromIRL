#include "ImmediateModel.h"
#include "GeometryGenerator.h"

#if defined(_WIN32)
#	include <windows.h>
#	include <GL/gl.h>
#elif defined(__APPLE__)
#	include <OpenGL/gl.h>
#else
#	include <GL/gl.h>
#endif

ImmediateModel::ImmediateModel(GeometryGenerator *shape, int resolution, int geometry) {
	this->initialise(shape, resolution, geometry);
}

void ImmediateModel::initialise(GeometryGenerator *shape, int resolution, int geometry) {
	this->shape = shape;
	this->resolution = resolution;
	this->geometry = geometry;
}

void ImmediateModel::draw() {
	
	glBegin(this->geometry);

	this->shape->generate(this, this->resolution);

	glEnd();

}

void ImmediateModel::specifyPoint(vec3f *vertex, vec3f *normal) {

	/* immediate */
	glNormal3f(normal->x, normal->y, normal->z);
	glVertex3f(vertex->x, vertex->y, vertex->z);

}

void ImmediateModel::specifySize(int size) {
	/* don't care! */
}
