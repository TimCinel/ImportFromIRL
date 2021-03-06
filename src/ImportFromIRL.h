#ifndef IMPORT_FROM_IRL_H
#define IMPORT_FROM_IRL_H

#include "vec3f.h"
#include "CursorReceiver.h"
#include "PanTarget.h"

typedef enum t_RenderMethod {
	METHOD_VERTEX_BUFFER_OBJECT,
	METHOD_VERTEX_ARRAY,
	NUM_RENDER_METHOD, /* MUST BE LAST! */
} RenderMethod;

typedef enum t_RenderOptions {
	RENDER_WIREFRAME,
	RENDER_LIGHTING,
	RENDER_OSD,
	NUM_RENDER_OPTIONS /* MUST BE LAST! */
} RenderOptions;

typedef enum t_ApplicationStates {
	STATE_WORKSPACE,
	STATE_WORKSPACE_FRAGMENT_ROTATE,
	STATE_CAPTURE,
	STATE_CAPTURE_DUMP,
	STATE_EDIT,
	STATE_EDIT_PLANE_ROTATE,
	STATE_INVALID
} ApplicationStates;

typedef struct t_AppSettings {
	unsigned int tesselation;
	unsigned int duplication;
	
    CursorReceiver *cursorReceiver;
	PanTarget *panTarget;

	unsigned int *primaryAdjustTarget;
	unsigned int *secondaryAdjustTarget;

	unsigned int selectedFrame;
	unsigned int selectedPlane;
	unsigned int selectedFragment;

	RenderMethod selectedMethod;
	ApplicationStates state;

	bool renderOptions[NUM_RENDER_OPTIONS];
	bool running;

} AppSettings;

#endif
