#include "PlaneCursorReceiver.h"

const float PlaneCursorReceiver::sensitivity = 0.03f;

void PlaneCursorReceiver::move(int dx, int dy) {
	if (ROTATE_MODE == this->mode) {
		if (this->yRotating && NULL != this->plane)
			this->plane->rotate(vec3f(0.0f, dx * sensitivity, 0.0f));
		if (this->zRotating && NULL != this->plane)
			this->plane->rotate(vec3f(0.0f, 0.0f, dx * sensitivity));
	} else {
		if (this->xzPanning && NULL != this->plane)
			this->plane->pan(vec3f(dx * sensitivity, 0.0f, dy * sensitivity));
		if (this->yPanning && NULL != this->plane)
			this->plane->pan(vec3f(0.0f,dy * sensitivity, 0.0f));
	}
}

void PlaneCursorReceiver::click(bool left, bool right) {
	if (ROTATE_MODE == this->mode) {
		this->yRotating = left;
		this->zRotating = right;
	} else {
		this->xzPanning = left;
		this->yPanning = right;
	}
}