#include "ObjectFragment.h"


ObjectFragment::ObjectFragment(vector<vec3f> &verts, vector<vec3f> &norms, 
                   vector<unsigned int> &tris) {

    this->verts = vector<vec3f>(verts);
    this->norms = vector<vec3f>(norms);
    this->tris = vector<unsigned int>(tris);

}

void ObjectFragment::drawThis() {

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &(this->verts[0]));

    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &(this->norms[0]));

    glDrawElements(GL_TRIANGLES, this->tris.size(), GL_UNSIGNED_INT, &(this->tris[0]));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

}
