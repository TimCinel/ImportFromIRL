#ifndef PLANECURSORRECEIVER_H
#define PLANECURSORRECEIVER_H

#include "CursorReceiver.h"
#include "CullPlane.h"

class PlaneCursorReceiver : public CursorReceiver {
private:
	CullPlane &plane;
	bool rotating;

	static const float sensitivity;

public:
	PlaneCursorReceiver(CullPlane &plane) :
		plane(plane),
		rotating(false) {}

    virtual void move(int dx, int dy);
    virtual void click(bool left, bool right);

};

#endif