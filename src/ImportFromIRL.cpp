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
#include "GeometryGenerator.h"
#include "KinectReceiver.h"
#include "CullPlane.h"

#include "Camera.h"
#include "vec3f.h"

#include <vector>
using namespace std;


//defaults
#define DEFAULT_LIGHTING		true
#define DEFAULT_WIREFRAME		false
#define DEFAULT_OSD				true

//application globals
AppSettings settings;
Camera camera;
float currentFramerate;
int windowWidth;
int windowHeight;
int lastMouseX = 0;
int lastMouseY = 0;
unsigned long animationClock = 0;

AbstractKinectInterface *kinect = NULL;
KinectReceiver *captureReceiver = NULL;
RenderModel *method = NULL;

vector<KinectReceiver *> frames;

void setRenderOptions() {

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);

	if (settings.renderOptions[RENDER_LIGHTING])
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);


    glDisable(GL_CULL_FACE);

	glPolygonMode(
		GL_FRONT_AND_BACK, 
		settings.renderOptions[RENDER_WIREFRAME] ? GL_LINE : GL_FILL
		);
}

void init() {

#ifndef _WIN32
	int argc = 0;  /* fake glutInit args */
	char *argv = (char *)"";
	glutInit(&argc, &argv);
#endif

	glewInit();
	
	glClearColor(0, 0, 0, 0);

	glEnable(GL_DEPTH_TEST);
	
	memset(&settings, 0, sizeof(AppSettings));

	settings.x = settings.y = settings.z = 0;
	settings.renderOptions[RENDER_LIGHTING] = DEFAULT_LIGHTING;
	settings.renderOptions[RENDER_WIREFRAME] = DEFAULT_WIREFRAME;
	settings.renderOptions[RENDER_OSD] = DEFAULT_OSD;
	settings.running = true;

	memset(&camera, 0, sizeof(Camera));
	camera.sensitivity = 0.3f;
	camera.zoom = 2.0f;

}

void reshape(int width, int height) {
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


void drawOSD() {
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

void display() {
	//clear the colour and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_COLOR_MATERIAL);

	glLoadIdentity();

	//camera transformations
	glTranslatef(0.0f, 0.0f, -camera.zoom);
	glRotatef(camera.rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(camera.rotY, 0.0f, 1.0f, 0.0f);
	
	setRenderOptions();

    //prepare to draw
    glPushMatrix();

	switch (settings.state) {
		case STATE_WORKSPACE:
			//iterate through and draw the frames

			break;

		case STATE_CAPTURE:
		case STATE_CAPTURE_DUMP:
			//just display from the device or a dump
			if (NULL == captureReceiver)
				captureReceiver = new KinectReceiver();

			if (NULL == method)
				method = new ImmediateModel(captureReceiver, 0, GL_TRIANGLES);

			//pan object
			glTranslatef(settings.x, settings.y, settings.z);

			if (settings.running && NULL != kinect)
				//get new depth data
				kinect->processDepth(captureReceiver);

			if (NULL != method)
				//draw!
				method->draw();

			break;
		case STATE_EDIT:
			break;
		default:
			break;
	}

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

	switch (settings.state)  {
		case STATE_CAPTURE:
		case STATE_WORKSPACE:
		case STATE_EDIT:
			if (camera.rotating) {
				camera.rotY += dx * camera.sensitivity;
				camera.rotX += dy * camera.sensitivity;
			} 
			if (camera.zooming) {
				camera.zoom -= dy * camera.zoom * camera.sensitivity * 0.03f;
			}
		default:
			break;
	};

	lastMouseX = x;
	lastMouseY = y;
}

void changeMode(ApplicationStates newState) {
	if (settings.state == newState)
		return;

	if (STATE_CAPTURE_DUMP == settings.state) {
		cout << "Can't change mode - currently dumping.\n";
	}

	if (NULL != method) {
		delete method;
		method = NULL;
	}

	settings.state = newState;

	cout << "Application changed state: " << newState << "\n";
}

void openSource(string arguments) {

	//out with the old
	if (NULL != kinect)
		delete kinect;
	kinect = NULL;

	if (NULL != captureReceiver)
		delete captureReceiver;
	captureReceiver = NULL;

	if (NULL != method)
		delete method;
	method = NULL;

	//in with the new
	if ("none" == arguments) {
		//no capture

		cout << "No source selected.\n";

	} else if ("kinect" == arguments) {
		//capture from actual kinect

		kinect = new WindowsKinectInterface();

		if (kinect->connectToKinect())  {
			cout << "Using Kinect Sensor as source.\n";
		} else {
			cout << "Failed to connect to Kinect Sensor\n";
			delete kinect;
			kinect = NULL;
		}
	} else {
		//capture from dump file

		kinect = new DummyKinectInterface(320, 240, (char *)arguments.c_str());

		if (kinect->connectToKinect()) {
			cout << "Using dump file \"" << arguments << "\" as source.\n";
		} else {
			cout << "Failed to load dump file\n";
			delete kinect;
			kinect = NULL;
		}
	}
}

void processCommand(char *command) {
	string cmd = string(command);

	if ('\0' == command[0])
		return;

	//strip trailing whitespace
	while(cmd.length() > 1 && cmd[cmd.length() - 1] == '\n' || cmd[cmd.length() - 1] == '\r')
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

	if ("quit" == directive)
		quit();
	else if ("mode" == directive)
		if ("capture" == arguments)
			changeMode(STATE_CAPTURE);
		else if("edit" == arguments) 
			changeMode(STATE_EDIT);
		else if ("workspace" == arguments) 
			changeMode(STATE_WORKSPACE);
		else
			cout << "Invalid mode \"" + arguments + "\"" << ".\n";

	if (STATE_CAPTURE == settings.state) {

		if ("source" == directive) {
			//switch depth and colour source
			openSource(arguments);
		} else if ("stash" == directive) {
			//save the current frame
			if (NULL != captureReceiver) {
				KinectReceiver *frame = new KinectReceiver(captureReceiver);
				frames.push_back(frame);

				cout << "Frame #" << frames.size() << " stashed.\n";
			} else {
				cout << "Failed to stash the frame.\n";
			}

		} else if ("dump" == directive) {
			//record depth and colour data to file
			if (NULL != kinect && kinect->startDump((char *)arguments.c_str())) {
				cout << "Started dump to \"" << arguments << "\".\n";
				settings.state = STATE_CAPTURE_DUMP;
			} else {
				cout << "Failed to start dump to \"" << arguments << "\".\n";
			}
		}

	} else if (STATE_CAPTURE_DUMP == settings.state) {
		//any input is taken as "stop dump" command
		if (NULL != kinect && kinect->endDump())
			cout << "Stopped dumping.\n";
		else
			cout << "Dump stop failed. Might be worth restarting...\n";

		settings.state = STATE_CAPTURE;
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
	else if (key == SDLK_o) {
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
    if (NULL != kinect) {
        delete kinect;
        kinect = NULL;
    }

	if (NULL != captureReceiver) {
		delete captureReceiver;
        captureReceiver = NULL;
    }

}