#include "CullPlane.h"

#include "GeometryOperations.h"

void CullPlane::init() {
    this->square[0] = vec3f(0.0,1.0,1.0);
    this->square[1] = vec3f(0.0,-1.0,1.0);
    this->square[2] = vec3f(0.0,-1.0,-1.0);
    this->square[3] = vec3f(0.0,1.0,-1.0);
}

void CullPlane::rotate(vec3f amount) {
    this->normal = GeometryOperations::rotate3D(&(this->normal), &amount);

    for (int i = 0; i < 4; i++)
        this->square[i] = GeometryOperations::rotate3D(&(this->square[i]), &amount);
}

void CullPlane::generate(RenderModel *callingModel, int resolution) {

	callingModel->specifySize(6);

    callingModel->specifyPoint(&(this->square[0]), &(this->normal));
    callingModel->specifyPoint(&(this->square[1]), &(this->normal));
    callingModel->specifyPoint(&(this->square[2]), &(this->normal));

    callingModel->specifyPoint(&(this->square[2]), &(this->normal));
    callingModel->specifyPoint(&(this->square[3]), &(this->normal));
    callingModel->specifyPoint(&(this->square[0]), &(this->normal));

}
