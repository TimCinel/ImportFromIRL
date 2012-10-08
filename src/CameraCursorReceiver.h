#ifndef CAMERACURSORRECEIVER_H
#define CAMERACURSORRECEIVER_H

#include "CursorReceiver.h"
#include "Camera.h"

class CameraCursorReceiver : public CursorReceiver {
private:
    Camera *camera;
public:
    CameraCursorReceiver(Camera *camera) : camera(camera) {}

    virtual void move(int dx, int dy);
    virtual void click(bool left, bool right);
};

#endif
