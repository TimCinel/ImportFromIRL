#ifndef PLANECURSORRECEIVER_H
#define PLANECURSORRECEIVER_H

#include "CursorReceiver.h"
#include "CullPlane.h"

class PlaneCursorReceiver : public CursorReceiver {
private:
    bool yRotating, zRotating;
    static const float sensitivity;

public:
    CullPlane *plane;

    PlaneCursorReceiver(CullPlane *plane) :
        plane(plane),
        yRotating(false),
        zRotating(false) {}

    PlaneCursorReceiver() :
        plane(NULL),
        yRotating(false),
        zRotating(false) {}

    virtual void move(int dx, int dy);
    virtual void click(bool left, bool right);
    virtual void reset();

};

#endif
