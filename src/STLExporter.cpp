#include "STLExporter.h"

#include <cstdio>
#include <cstring>

#include <iostream>;
using namespace std;

bool STLExporter::exportToFile(vector<ObjectFragment> &fragments, const char *fileName) {

    FILE *fp = fopen(fileName, "wb");

    if (NULL == fp)
        return false;

    //use this to write to file
    unsigned char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);


    //HEADER
    int targetSize = 80;
    int written = fwrite(buffer, 1, targetSize, fp);

    if (written != targetSize) {
        cout << "HEADER\n";
        return false;
    }

    //NUMBER OF TRIANGLES
    unsigned int triangles = 0;

    for (int i = 0; i < fragments.size(); i++)
        triangles += (fragments[i].tris.size() / 3);

    memcpy(buffer, &triangles, sizeof(unsigned int));
    targetSize = 4;
    written = fwrite(buffer, 1, targetSize, fp);

    if (written != targetSize) {
        cout << "TRIANGLE COUNT\n";
        return false;
    }

    //transform vertices before spitting out data
    FragmentExporter::processFragments(fragments);

    //convenient pointer that allows us to treat the buffer as an array of float[3]s
    vec3f *points = (vec3f *)buffer;
    unsigned short attributes = 0;

    //TRIANGLES
    for (int i = 0; i < fragments.size(); i++) {

        int totalTriPoints = fragments[i].tris.size();

        vec3f *verts = &(fragments[i].verts[0]);
        unsigned int *tris = &(fragments[i].tris[0]);



        int triPoint = 0;
        while (triPoint < totalTriPoints) {
            //each triangle

            //don't provide facet normal, let import software do it
            points[0] = vec3f();

            //should work for pos = 1, 2, 3
            points[1] = verts[tris[triPoint++]];
            points[2] = verts[tris[triPoint++]];
            points[3] = verts[tris[triPoint++]];

            //don't worry about attributes - they should be ZERO still

            targetSize = 50;
            written = fwrite(buffer, 1, targetSize, fp);

            if (written != targetSize) {
                cout << "TRIANGLE OUTPUT\n";
                return false;
            }

        }
    }

    fclose(fp);

    return true;

}
