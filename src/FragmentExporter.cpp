#include "FragmentExporter.h"

#include "GeometryOperations.h"

#if defined(_WIN32)
#	include <windows.h>
#	include <GL/gl.h>
#elif defined(__APPLE__)
#	include <OpenGL/gl.h>
#else
#	include <GL/gl.h>
#endif

void FragmentExporter::processFragments(vector<ObjectFragment> &fragments) {

    float mvmatrix[16];

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    //clean slate
    glLoadIdentity();


    for (int i = 0; i < fragments.size(); i++) {
        //that slate's about to get dirty
        glPushMatrix();

        //the modelview matrix for this particular fragment
        glRotatef(fragments[i].rotation.x, 1.0, 0.0, 0.0);
        glRotatef(fragments[i].rotation.y, 0.0, 1.0, 0.0);
        glRotatef(fragments[i].rotation.z, 0.0, 0.0, 1.0);
        glTranslatef(fragments[i].position.x, fragments[i].position.y, fragments[i].position.z);

        //save the matrix
        glGetFloatv(GL_MODELVIEW_MATRIX, mvmatrix);
        Matrix4f modelViewMatrix = GeometryOperations::convertModelViewMatrix(mvmatrix);


        //use the matrix to transform these vertices
        vec3f *vertices = &(fragments[i].verts[0]);
        GeometryOperations::transformVertices(
                modelViewMatrix,
                vertices,
                fragments[i].verts.size());

        //ok now do the normals
        vec3f *normals = &(fragments[i].norms[0]);
        GeometryOperations::transformVertices(
                modelViewMatrix,
                normals,
                fragments[i].verts.size());


        //yay, clean again
        glPopMatrix();
    }

    //who knows what it was before this? it doesn't matter.
    glPopMatrix();

}
