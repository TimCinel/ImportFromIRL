#ifndef STLEXPORTER_H
#define STLEXPORTER_H

#include "FragmentExporter.h"

class STLExporter : FragmentExporter {
public:
    static const int BUFFER_SIZE = 512;

    virtual bool exportToFile(vector<ObjectFragment> &fragments, const char *fileName);
};

#endif
