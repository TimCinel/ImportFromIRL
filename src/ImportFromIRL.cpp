#include "ImportFromIRL.h"

#include <cmath>
#include <cstring>
#include <cstdio>

#include <string>
#include <iostream>

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
#include "DummyKinectInterface.h"

#include "RenderModel.h"
#include "ImmediateModel.h"
#include "VertexBufferObjectModel.h"

#include "GeometryGenerator.h"
#include "KinectReceiver.h"

#include "Camera.h"
#include "vec3f.h"

using namespace std;


/* Setting  Defaults */
#define DEFAULT_LIGHTING		true
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

	//GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	//GLfloat mat_shininess[] = { 50.0 };
	//GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	//GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	//GLfloat lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };

	//glClearColor(0.0, 0.0, 0.0, 0.0);


	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_position);

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);

	if (settings.renderOptions[RENDER_LIGHTING])
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);


    glEnable(GL_CULL_FACE);

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

	kinect = new DummyKinectInterface(320, 240, "../data/dump.dat");
    kinect->connectToKinect();

	glewInit();
	
	glClearColor(0, 0, 0, 0);

	glEnable(GL_DEPTH_TEST);
	
	memset(&settings, 0, sizeof(AppSettings));

	settings.x = settings.y = settings.z = 0;
	settings.renderOptions[RENDER_LIGHTING] = DEFAULT_LIGHTING;
	settings.renderOptions[RENDER_WIREFRAME] = DEFAULT_WIREFRAME;
	settings.renderOptions[RENDER_OSD] = DEFAULT_OSD;
	settings.tesselation = DEFAULT_TESSELATION;
	settings.selectedMethod = DEFAULT_METHOD;
	settings.running = true;

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
	if (NULL == receiver)
		receiver = new KinectReceiver();

	if (NULL == method)
		method = new ImmediateModel(receiver, 0, GL_TRIANGLES);

	/* Clear the colour and depth buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_COLOR_MATERIAL);

	glLoadIdentity();

	/* Camera transformations */
	glTranslatef(0.0f, 0.0f, -camera.zoom);
	glRotatef(camera.rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(camera.rotY, 0.0f, 1.0f, 0.0f);
	

	/* motion */
	glTranslatef(settings.x, settings.y, settings.z);

	/* Draw scene */
	setRenderOptions();

    /* prepare to draw */
    glPushMatrix();


	if (settings.running) {
		/* get new depth data */
		kinect->processDepth(receiver);
	}

	/* draw! */
    method->draw();

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

void processCommand(char *command) {
	string cmd = string(command);

	//strip trailing whitespace
	while(cmd[cmd.length() - 1] == '\n' || cmd[cmd.length() - 1] == '\r')
		cmd.erase(cmd.length() - 1, 1);

	int spacePos = cmd.find_first_of(' ');

	string directive, arguments;
	if (spacePos != cmd.npos) {
		directive = cmd.substr(0, spacePos);
		arguments = cmd.substr(spacePos + 1, cmd.length() - spacePos - 1);
	} else {
		directive = string(cmd);
	}

	printf("cmd: %s\n", command);
	cout << "directive: \"" << directive << "\"\n";
	cout << "arguments: \"" << arguments << "\"\n";

	if ("quit" == directive) {
		quit();
	} else if ("addplane" == directive) {

	}
}

void keyDown(int key) {
	bool deleteMethod = false, deleteShape = false;
	static const unsigned int MAX_COMMAND_LEN = 64;
	

	char command[MAX_COMMAND_LEN];

	if (key == SDLK_q)
		quit();
	else if (key == SDLK_c) {
		//read command from command line
		cout << "Enter a command (Press ? for help)\n";

 		if (NULL != fgets((char *)command, MAX_COMMAND_LEN - 1, stdin))
			processCommand(command);
		else
			fputs("Failed to read command.\n", stdout);

	} else if (key == SDLK_w)
		settings.z += 0.05;
	else if (key == SDLK_a)
		settings.x -= 0.05;
	else if (key == SDLK_s)
		settings.z -= 0.05;
	else if (key == SDLK_d)
		settings.x += 0.05;
		//printf("Windw dimensions: %dx%d\n", windowWidth, windowHeight);
	else if (key == SDLK_SPACE)
		settings.running = !settings.running;
	else if (key == SDLK_f)
		printf("Frame rate: %f\n", currentFramerate);
	else if (key == SDLK_l)  
		settings.renderOptions[RENDER_LIGHTING] = !settings.renderOptions[RENDER_LIGHTING];
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