#ifndef FRAGMENTEXPORTER_H
#define FRAGMENTEXPORTER_H

#include "ObjectFragment.h"
#include <vector>

using namespace std;

class FragmentExporter {
public:
    virtual bool exportToFile(vector<ObjectFragment> &fragments, const char *fileName) = 0;
};

#endif
