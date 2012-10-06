#ifndef VERTEXBUFFEROBJECTMODEL_H
#define VERTEXBUFFEROBJECTMODEL_H

#include "RenderModel.h"
#include "vec3f.h"

/* will store the vertices and normals in a vertex array, then
 * create a Vertex Buffer Object (VBO) from the arrays. The
 *  given shape is then drawn using the VBO */

class VertexBufferObjectModel : public RenderModel {
private:
	vec3f *vertices, *normals;
	int currentIndex;

	unsigned int vertexBuffer, normalBuffer;

public:
	VertexBufferObjectModel(GeometryGenerator *shape, int resolution, int geometry);
	~VertexBufferObjectModel();

	void initialise(GeometryGenerator *shape, int resolution, int geometry);

	void draw();

	void specifyPoint(vec3f *vertex, vec3f *normal);
	void specifySize(int size);

};

#endif
