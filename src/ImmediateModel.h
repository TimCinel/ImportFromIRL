#ifndef IMMEDIATEMODEL_H
#define IMMEDIATEMODEL_H

#include "RenderModel.h"

/* will render the given shape in Immediate mode, the vertices and 
 * normals are calculated every time*/

class ImmediateModel : public RenderModel {
public:
	ImmediateModel(GeometryGenerator *shape, int resolution, int geometry);

	void initialise(GeometryGenerator *shape, int resolution, int geometry);

	void draw();

	void specifyPoint(vec3f *vertex, vec3f *normal);
	void specifySize(int size);

};

#endif
