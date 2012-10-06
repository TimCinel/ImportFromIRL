#ifndef IMPORT_FROM_IRL_H
#define IMPORT_FROM_IRL_H

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

typedef struct t_AppSettings {
	unsigned int tesselation;
	unsigned int duplication;
	
	float x, y, z;

	RenderMethod selectedMethod;

	bool renderOptions[NUM_RENDER_OPTIONS];
	bool running;

} AppSettings;

#endif
