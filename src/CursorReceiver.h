#ifndef CURSORRECEIVER_H
#define CURSORRECEIVER_H

#if defined(_WIN32)
#   include <SDL_mouse.h>
#else
#	include <SDL/SDL_mouse.h>
#endif

class CursorReceiver {
public:
	int lastMouseX;
	int lastMouseY;

    void mouseMove(int x, int y) {
        this->move(x - this->lastMouseX, y - this->lastMouseY);
        this->lastMouseX = x;
        this->lastMouseY = y;
    }

    void mouseDown(int button, int state, int x, int y) {
        this->click(
                (SDL_BUTTON_LEFT == button && state), 
                (SDL_BUTTON_RIGHT == button && state)
                );
    }

    virtual void move(int dx, int dy) = 0;
    virtual void click(bool left, bool right) = 0;
    virtual void reset() = 0;

};

#endif
