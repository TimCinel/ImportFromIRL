#ifndef PLANECURSORRECEIVER_H
#define PLANECURSORRECEIVER_H

#include "CursorReceiver.h"
#include "CullPlane.h"

class PlaneCursorReceiver : public CursorReceiver {
private:
	bool yRotating, zRotating;
	bool xzPanning, yPanning;
	static const float sensitivity;

public:
	CullPlane *plane;
	int mode;

	PlaneCursorReceiver(CullPlane *plane, int mode) :
		plane(plane),
		mode(mode),
		yRotating(false),
		zRotating(false) {}

	PlaneCursorReceiver() :
		plane(NULL),
		mode(ROTATE_MODE),
		yRotating(false),
		zRotating(false) {}

    virtual void move(int dx, int dy);
    virtual void click(bool left, bool right);

	static const int ROTATE_MODE = 0;
	static const int PAN_MODE = 1;

};

#endif