#ifndef VERTEXARRAYMODEL_H
#define VERTEXARRAYMODEL_H

#include "RenderModel.h"
#include "vec3f.h"

/* will store the vertices and normals in a vertex array, then
 * use the vertex array to draw the given shape */

class VertexArrayModel : public RenderModel {
private:
	vec3f *vertices, *normals;
	int currentIndex;

public:
	VertexArrayModel(GeometryGenerator *shape, int resolution, int geometry);
	~VertexArrayModel();

	void initialise(GeometryGenerator *shape, int resolution, int geometry);

	void draw();

	void specifyPoint(vec3f *vertex, vec3f *normal);
	void specifySize(int size);

};

#endif
