#ifndef CAMERA_H
#define CAMERA_H

typedef struct t_Camera {
	int rotating;
	int zooming;
	float rotX, rotY;
	float zoom;
	float sensitivity;
} Camera;

#endif