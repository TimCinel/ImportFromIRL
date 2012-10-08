#ifndef IMMEDIATEMODEL_H
#define IMMEDIATEMODEL_H

#include "RenderModel.h"
#include "CullPlane.h"

#include <vector>
using namespace std;

/* will render the given shape in Immediate mode, the vertices and 
 * normals are calculated every time*/

class ImmediateModel : public RenderModel {
private:
	vector<CullPlane> planes;
public:
	ImmediateModel(GeometryGenerator *shape, int resolution, int geometry);

	virtual void initialise(GeometryGenerator *shape, int resolution, int geometry);
	virtual void drawGeometry();
	virtual void specifyPoint(vec3f *vertex, vec3f *normal);
	virtual void specifySize(int size);

};

#endif
