#include "FragmentCursorReceiver.h"

const float FragmentCursorReceiver::sensitivity = 0.03f;

void FragmentCursorReceiver::move(int dx, int dy) {
    if (this->xRotating && NULL != this->fragment) {
        this->fragment->rotation.x += dx * sensitivity;
    }
    if (this->yRotating && NULL != this->fragment) {
        this->fragment->rotation.y += dx * sensitivity;
    }
}

void FragmentCursorReceiver::click(bool left, bool right) {
    this->xRotating = left;
    this->yRotating = right;
}

void FragmentCursorReceiver::reset() {

    if (NULL != this->fragment) {
        //just reset the positions
        this->fragment->rotation = vec3f();
        this->fragment->position = vec3f();
    }

}
