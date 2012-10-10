#include "CameraCursorReceiver.h"

void CameraCursorReceiver::click(bool left, bool right) {
    this->camera->rotating = left;
    this->camera->zooming = right;
}

void CameraCursorReceiver::move(int dx, int dy) {
    if (this->camera->rotating) {
        this->camera->rotY += dx * this->camera->sensitivity;
        this->camera->rotX += dy * this->camera->sensitivity;
    } 

    if (this->camera->zooming) {
        this->camera->zoom -= dy * this->camera->zoom * this->camera->sensitivity * 0.03f;
    }

}

void CameraCursorReceiver::reset() {
    //reset rotation
    this->camera->rotX = this->camera->rotY = 0.0;
    this->camera->zoom = Camera::DEFAULT_ZOOM;
}
