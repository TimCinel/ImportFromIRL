#include "PlaneCursorReceiver.h"

const float PlaneCursorReceiver::sensitivity = 0.03f;

void PlaneCursorReceiver::move(int dx, int dy) {
	if (this->rotating)
		this->plane.rotate(vec3f(0.0f, dy * sensitivity, dx * sensitivity));
}

void PlaneCursorReceiver::click(bool left, bool right) {
	this->rotating = left;
}