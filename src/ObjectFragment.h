#ifndef OBJECTFRAGMENT_H
#define OBJECTFRAGMENT_H

#pragma once

#include "ObjectModel.h"

#include <vector>
using namespace std;

class ObjectFragment : public ObjectModel {

public:
    ObjectFragment() {};

    ObjectFragment(vector<vec3f> &verts, vector<vec3f> &norms, 
                   vector<unsigned int> &tris);

    virtual ~ObjectFragment() {};

    vector<vec3f> verts;
    vector<vec3f> norms;
	vector<unsigned int> tris;

	//inherited from ObjectModel
	virtual void drawThis();

};

#endif
