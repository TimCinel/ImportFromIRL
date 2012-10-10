#ifndef CAMERA_H
#define CAMERA_H

class Camera {
public:
	int rotating;
	int zooming;
	float rotX, rotY;
	float zoom;
	float sensitivity;

    static const float DEFAULT_ZOOM;
    static const float DEFAULT_SENSITIVITY;

    Camera() : 
        rotating(0), 
        zooming(0), 
        rotX(0), 
        rotY(0), 
        zoom(0), 
        sensitivity(0)
        {}
};

#endif
