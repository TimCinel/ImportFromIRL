#include "CullPlane.h"

#include "GeometryOperations.h"

void CullPlane::init() {
    this->square[0] = this->origSquare[0] = vec3f(0.0,5.0,5.0);
    this->square[1] = this->origSquare[1] = vec3f(0.0,-5.0,5.0);
    this->square[2] = this->origSquare[2] = vec3f(0.0,-5.0,-5.0);
    this->square[3] = this->origSquare[3] = vec3f(0.0,5.0,-5.0);

    this->calculateD();
}

void CullPlane::rotate(vec3f amount) {
    this->totalRotation.x +=  amount.x;
    this->totalRotation.y +=  amount.y;
    this->totalRotation.z +=  amount.z;

    this->normal = GeometryOperations::rotate3D(this->origNormal, this->totalRotation);

    for (int i = 0; i < 4; i++)
        this->square[i] = GeometryOperations::rotate3D(this->origSquare[i], this->totalRotation);

    this->calculateD();
}

void CullPlane::pan(vec3f amount) {
    this->offset.x += amount.x;
    this->offset.y += amount.y;
    this->offset.z += amount.z;

    this->calculateD();
}

void CullPlane::drawThis() {

    glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
	glEnable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

    glTranslatef(this->offset.x, this->offset.y, this->offset.z);
    glBegin(GL_QUADS);

    glColor3f(1.0, 0.0, 0.0);
    for (unsigned int i = 0; i < 4; i++) {
        glNormal3f(this->normal.x, this->normal.y, this->normal.z);
        glVertex3f(this->square[i].x, this->square[i].y, this->square[i].z);
    }

    glColor3f(0.0, 1.0, 0.0);
    for (unsigned int i = 0; i < 4; i++) {
        glNormal3f(this->normal.x, this->normal.y, this->normal.z);
        glVertex3f(this->square[3 - i].x, this->square[3 - i].y, this->square[3 - i].z);
    }

    glEnd();

    glPopAttrib();
    glPopMatrix();
}


bool CullPlane::cullPoint(vec3f &point) {
    return (point.x * this->normal.x + point.y * this->normal.y + point.z * this->normal.z + d > 0);
}

void CullPlane::calculateD() {
    this->d = -(
        this->offset.x * this->normal.x +
        this->offset.y * this->normal.y +
        this->offset.z * this->normal.z
    );
}
