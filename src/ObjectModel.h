#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include "vec3f.h"

#if defined(_WIN32)
#	include <windows.h>
#	include <GL/gl.h>
#elif defined(__APPLE__)
#	include <OpenGL/gl.h>
#else
#	include <GL/gl.h>
#endif

#include <vector>
using namespace std;

class ObjectModel {
private:
	vector<ObjectModel *> children;
public:
	vec3f position, rotation;

	virtual ~ObjectModel();

	//override not recommended
	void draw();

	//can be overriden if required
	virtual void drawChildren();

	//must be defined
	virtual void drawThis() = 0;

    vec3f *getPosition() {return &(this->position); }
    vec3f *getRotation() {return &(this->rotation); }
};

#endif
