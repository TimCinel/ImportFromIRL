#include "CullPlane.h"

#include "GeometryOperations.h"

void CullPlane::init() {
    this->square[0] = this->origSquare[0] = vec3f(0.0,1.0,1.0);
    this->square[1] = this->origSquare[1] = vec3f(0.0,-1.0,1.0);
    this->square[2] = this->origSquare[2] = vec3f(0.0,-1.0,-1.0);
    this->square[3] = this->origSquare[3] = vec3f(0.0,1.0,-1.0);

	this->d = -(
		this->offset.x * this->normal.x +
		this->offset.y * this->normal.y +
		this->offset.z * this->normal.z
	);
}

void CullPlane::rotate(vec3f amount) {
	this->totalRotation.x +=  amount.x;
	this->totalRotation.y +=  amount.y;
	this->totalRotation.z +=  amount.z;

	this->normal = GeometryOperations::rotate3D(this->origNormal, this->totalRotation);

    for (int i = 0; i < 4; i++)
		this->square[i] = GeometryOperations::rotate3D(this->origSquare[i], this->totalRotation);

	this->d = -(
		this->offset.x * this->normal.x +
		this->offset.y * this->normal.y +
		this->offset.z * this->normal.z
	);
}

void CullPlane::drawThis() {

	static const unsigned int tris[] = {0, 1, 2, 2, 3, 0};

	for (unsigned int i = 0; i < sizeof(tris) / sizeof(unsigned int); i++) {
		glNormal3f(this->normal.x, this->normal.y, this->normal.z);
		glVertex3f(this->square[i].x, this->square[i].y, this->square[i].z);
	}
}


bool CullPlane::cullPoint(vec3f point) {
	return (point.x * this->normal.x + point.y * this->normal.y + point.z * this->normal.z + d > 0);
}
