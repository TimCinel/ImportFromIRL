#include "GeometryOperations.h"

#include "../lib/eigen/Eigen/Dense"

using namespace Eigen;
using namespace std;

vec3f GeometryOperations::rotate3D(vec3f &point, vec3f &rotation) {
	float x = rotation.x, y = rotation.y, z = rotation.z;

	Vector3f v = Vector3f(point.x, point.y, point.z);
	Matrix3f r = Matrix3f();

	//3D rotation matrix
	//http://en.wikipedia.org/wiki/Rotation_matrix

	r <<	cos(y)*cos(z),	-cos(x)*sin(z)+sin(x)*sin(y)*cos(z),	sin(x)*sin(z)+cos(x)*sin(y)*cos(z),
			cos(y)*sin(z),	cos(x)*cos(z)+sin(x)*sin(y)*sin(z),		-sin(x)*cos(z)+cos(x)*sin(y)*sin(z),
			-sin(y),		sin(x)*cos(y),							cos(x)*cos(y);
			

	Vector3f result = r*v;

	return vec3f(result[0], result[1], result[2]);
}

Matrix4f GeometryOperations::convertModelViewMatrix(float mvmatrix[16]) {
    Matrix4f modelViewMatrix;

    for (int i = 0; i < 16; i++)
        modelViewMatrix(i) = mvmatrix[i];

    return modelViewMatrix;
}

void GeometryOperations::transformVertices(
        Matrix4f &transformationMatrix,
        vec3f *vertex, 
        unsigned int numVertices
        ) {

    Vector4f vect;

    for (int i = 0; i < numVertices; i++) {
        //convert to 4x1 matrix
        vect = Vector4f(vertex->x, vertex->y, vertex->z, 1.0);

        //multiply
        vect = transformationMatrix * vect;

        //save back to vertex
        vertex->x = vect[0];
        vertex->y = vect[1];
        vertex->z = vect[2];

        //next vertex, please
        vertex++;
    }

}
