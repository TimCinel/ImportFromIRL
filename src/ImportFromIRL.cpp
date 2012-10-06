#include "ImportFromIRL.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

#include <GL/glew.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#define snprintf _snprintf_s

#include "sdlbase.h"

#include "AbstractKinectInterface.h"
#include "WindowsKinectInterface.h"

#include "RenderModel.h"
#include "ImmediateModel.h"
#include "VertexBufferObjectModel.h"

#include "GeometryGenerator.h"
#include "TorusGen.h"

#include "Camera.h"
#include "vec3f.h"


/* Setting  Defaults */
#define DEFAULT_WIREFRAME		false
#define DEFAULT_OSD				true
#define DEFAULT_TESSELATION		64
#define DEFAULT_METHOD			METHOD_VERTEX_BUFFER_OBJECT	

/* Scene globals */
AppSettings settings;
Camera camera;
float currentFramerate;
int windowWidth;
int windowHeight;
int lastMouseX = 0;
int lastMouseY = 0;
unsigned long animationClock = 0;

AbstractKinectInterface *kinect = NULL;
KinectReceiver *receiver = NULL;
RenderModel *method = NULL;


/* Update opengl state to match flags in renderOptions */
void setRenderOptions()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glPolygonMode(
		GL_FRONT_AND_BACK, 
		settings.renderOptions[RENDER_WIREFRAME] ? GL_LINE : GL_FILL
		);
}

/* Called once at program start */
void init()
{

#ifndef _WIN32
	int argc = 0;  /* fake glutInit args */
	char *argv = (char *)"";
	glutInit(&argc, &argv);
#endif

    kinect = new WindowsKinectInterface();
    kinect->connectToKinect();

	glewInit();
	
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	
	memset(&settings, 0, sizeof(AppSettings));

	settings.renderOptions[RENDER_WIREFRAME] = DEFAULT_WIREFRAME;
	settings.renderOptions[RENDER_OSD] = DEFAULT_OSD;
	settings.tesselation = DEFAULT_TESSELATION;
	settings.selectedMethod = DEFAULT_METHOD;

	memset(&camera, 0, sizeof(Camera));
	camera.sensitivity = 0.3f;
	camera.zoom = 2.0f;

}

/* Called once at start and again on window resize */
void reshape(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	
	/* Portion of viewport to render to */
	glViewport(0, 0, width, height);
	
	/* Calc aspect ratio */
	float aspect = width / (float)height;
	
	/* Begin editing projection matrix */
	glMatrixMode(GL_PROJECTION);
	
	/* Clear previous projection */
	glLoadIdentity();
	
	/* Generate perspective projection matrix */
	gluPerspective(75.0f, aspect, 0.01f, 100.0f);
	
	/* Restore modelview as current matrix */
	glMatrixMode(GL_MODELVIEW);
}


void drawOSD()
{
	char *bufp;
	char buffer[32];
	
	/* Backup previous "enable" state */
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	/* Create a temporary orthographic projection, matching
	 * window dimensions, and push it onto the stack */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	
	/* draw framerate */
	glPushMatrix();
	glLoadIdentity(); /* clear current modelview (ie. from display) */

	snprintf(buffer, sizeof(buffer), "FR(fps): %d", (int)currentFramerate);
	glRasterPos2i(10, 30);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	snprintf(buffer, sizeof(buffer), "FR(ms):  %.2f", (1000.0 / currentFramerate));
	glRasterPos2i(10, 10);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();  /* pop projection */
	glMatrixMode(GL_MODELVIEW);

	/* Restore "enable" state */
	glPopAttrib();
}

void display()
{
	static float noAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	static float whiteDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
	static float whiteSpecular[] = {1.0f, 1.0f, 1.0f};

	static float materialDiffuse[] = {0.7f, 0.7f, 0.7f, 1.0f}; /* Brighter as surface faces light */
	static float materialSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f}; /* Highlight, direct reflection from light */
	static float materialShininess = 64.0f; /* 1 to 128, higher gives sharper highlight */
	
	if (NULL == receiver)
		receiver = new KinectReceiver();

	if (NULL == method)
		method = new ImmediateModel(receiver, 0, GL_TRIANGLES);

	/* Clear the colour and depth buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/* Camera transformations */
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -camera.zoom);
	glRotatef(camera.rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(camera.rotY, 0.0f, 1.0f, 0.0f);
	

	/* Draw scene */
	setRenderOptions();

	/* disable all lights */
	for (int i = 0; i < 8; i++)
		glDisable(GL_LIGHT0 + i);

	/* first light is positional, placed at 3.0 along the Z axis */
	float lightPosition[] = {0.0f, 0.0f, 3.0f, 1.0f};

	/* enable requested lights */
	if (settings.lights > 0) {

        /* create and enable first light */
		glLightfv(GL_LIGHT0, GL_AMBIENT, noAmbient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuse);

		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glEnable(GL_LIGHT0);

		/* make all other lights directional */
		lightPosition[3] = 0.0f;
    }

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);

	/* rotation rate (rotations per ms) */
	const float rate = 0.0005;

	/* object grid spread factor */
	const float spread = 2.1;
	/* object grid offset */
	float offset = (settings.duplication - 1)*spread - (settings.duplication - 1)*spread / 2;


    /* prepare to draw */
    glPushMatrix();


	/* get new depth data */
    kinect->processDepth(receiver);

	/* draw! */
    method->draw();

	/* 
	static const float pi = 3.14159265f;
    static const float l_max = -2.0;
    static const float wide_angle = (57 / 360.0f) * 2.0f*pi;
    static const float high_angle = (43 / 360.0f) * 2.0f*pi;

    static const float wide_offset = -wide_angle / 2;
    static const float high_offset = -high_angle / 2;

    //ignore first byte
    static const USHORT max_depth = ((USHORT)(-1)) >> 4;
    glBegin(GL_POINTS);

    float x, y, z, scaleFactor;

    USHORT *currentDepth = kinect->getDepthData();

    for (int v = 0; v < kinect->getDepthHeight(); v++) {
        for (int u = 0; u < kinect->getDepthWidth(); u++) {

            if (0 == *currentDepth) {
                //skip unknown vertices
                currentDepth++;
                continue;
            }

            x = l_max * sin(wide_offset + ((float)u / kinect->getDepthWidth()) * wide_angle);
            y = l_max * sin(high_offset + ((float)v / kinect->getDepthHeight()) * high_angle);
            z = l_max * 
                cos(wide_offset + ((float)u / kinect->getDepthWidth()) * wide_angle) * 
                cos(high_offset + ((float)v / kinect->getDepthHeight()) * high_angle);

            //scale 
            scaleFactor = (float)*currentDepth / max_depth;

#if 0
            if (0 == *currentDepth)
                scaleFactor = 1.0;
#endif

            x *= scaleFactor;
            y *= scaleFactor;
            z *= scaleFactor;

            glVertex3f(x, y, z);

            currentDepth++;
        }
    }


    glEnd();
	*/

    glPopMatrix();

	if (settings.renderOptions[RENDER_OSD])
		drawOSD();


}

/* Called continuously. dt is time between frames in seconds */
void update(float dt) {
	static float fpsTime = 0.0f;
	static int fpsFrames = 0;
	static float autoTime;

	fpsTime += dt;
	fpsFrames += 1;

	if (fpsTime > 1.0f) {
		currentFramerate = fpsFrames / fpsTime;
		fpsTime = 0.0f;
		fpsFrames = 0;
	}

	animationClock += (long)(dt * 1000);

}

void mouseDown(int button, int state, int x, int y) {
	if (button == SDL_BUTTON_LEFT)
		camera.rotating = (state == 1);
	if (button == SDL_BUTTON_RIGHT)
		camera.zooming = (state == 1);
}

void mouseMove(int x, int y) {
	int dx = x - lastMouseX;
	int dy = y - lastMouseY;
	if (camera.rotating)
	{
		camera.rotY += dx * camera.sensitivity;
		camera.rotX += dy * camera.sensitivity;
	}
	if (camera.zooming)
	{
		camera.zoom -= dy * camera.zoom * camera.sensitivity * 0.03f;
	}
	lastMouseX = x;
	lastMouseY = y;
}

void keyDown(int key) {
	bool deleteMethod = false, deleteShape = false;

	if (key == SDLK_c)
		quit();
	else if (key == SDLK_d)
		printf("Windw dimensions: %dx%d\n", windowWidth, windowHeight);
	else if (key == SDLK_f)
		printf("Frame rate: %f\n", currentFramerate);
	else if (key == SDLK_m) { 
		settings.selectedMethod = (RenderMethod)((settings.selectedMethod + 1) % NUM_RENDER_METHOD);
		deleteMethod = true;
	} else if (key == SDLK_o) {
		settings.renderOptions[RENDER_OSD] = !settings.renderOptions[RENDER_OSD];
		printf("OSD: %s\n", (settings.renderOptions[RENDER_OSD] ? "ON" : "OFF"));
	} else if (key == SDLK_F4) {
		settings.renderOptions[RENDER_WIREFRAME] = !settings.renderOptions[RENDER_WIREFRAME];
		printf("Wireframe: %s\n", (settings.renderOptions[RENDER_WIREFRAME] ? "ON" : "OFF"));
	}

}

void keyUp(int key) {
}

void cleanup() {
    if (NULL != kinect) 
        delete kinect;

	if (NULL != receiver)
		delete receiver;

}