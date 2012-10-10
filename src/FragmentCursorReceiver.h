#ifndef FRAGMENTCURSORRECEIVER_H
#define FRAGMENTCURSORRECEIVER_H

#include "CursorReceiver.h"
#include "ObjectFragment.h"

class FragmentCursorReceiver : public CursorReceiver {
private:
    bool xRotating, yRotating;
    static const float sensitivity;

public:
    ObjectFragment *fragment;

    FragmentCursorReceiver(ObjectFragment *fragment) :
        fragment(fragment),
        xRotating(false),
        yRotating(false) {}

    FragmentCursorReceiver() :
        fragment(NULL),
        xRotating(false),
        yRotating(false) {}

    virtual void move(int dx, int dy);
    virtual void click(bool left, bool right);
    virtual void reset();

};

#endif
