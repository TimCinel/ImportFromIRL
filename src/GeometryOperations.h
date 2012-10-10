#ifndef GEOMETRYOPERATIONS_H
#define GEOMETRYOPERATIONS_H

#include "vec3f.h"
#include "../lib/eigen/Eigen/Dense"

using namespace std;
using namespace Eigen;

class GeometryOperations {
public:
	static vec3f rotate3D(vec3f &point, vec3f &rotation);

    static Matrix4f convertModelViewMatrix(float mvmatrix[16]);

    static void transformVertices(
        Matrix4f &transformationMatrix,
        vec3f *vertex, 
        unsigned int numVertices);
};

#endif
