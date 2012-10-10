#include "PlaneCursorReceiver.h"

const float PlaneCursorReceiver::sensitivity = 0.03f;

void PlaneCursorReceiver::move(int dx, int dy) {
    if (this->yRotating && NULL != this->plane)
        this->plane->rotate(vec3f(0.0f, dx * sensitivity, 0.0f));
    if (this->zRotating && NULL != this->plane)
        this->plane->rotate(vec3f(0.0f, 0.0f, dx * sensitivity));
}

void PlaneCursorReceiver::click(bool left, bool right) {
    this->yRotating = left;
    this->zRotating = right;
}
