#include "ImportFromIRL.h"

#include <cmath>
#include <cstring>
#include <cstdio>

#include <string>
#include <iostream>

#include <GL/glew.h>

#if defined(_WIN32)
#   include <windows.h>
#   include <GL/gl.h>
#   include <GL/glut.h>
#   include <SDL_keyboard.h>
#   include <SDL_mouse.h>
#   define snprintf _snprintf_s
#else
#   include <GL/gl.h>
#   include <GL/glut.h>
#   include <SDL/SDL_keyboard.h>
#   include <SDL/SDL_mouse.h>
#endif

#include "sdlbase.h"

#if defined(_WIN32)
#include "WindowsKinectInterface.h"
#endif

#include "AbstractKinectInterface.h"
#include "DummyKinectInterface.h"

#include "ObjectModel.h"
#include "KinectReceiver.h"
#include "CullPlane.h"
#include "ObjectFragment.h"

#include "Camera.h"
#include "CameraCursorReceiver.h"
#include "PlaneCursorReceiver.h"
#include "vec3f.h"

#include <vector>
using namespace std;


//defaults
static const bool DEFAULT_LIGHTING  = true;
static const bool DEFAULT_WIREFRAME = false;
static const bool DEFAULT_OSD       = true;

static const float TRANSLATE_DELTA  = 0.2;

//application globals
AppSettings settings;
Camera camera;
CameraCursorReceiver camCursor(&camera);
PlaneCursorReceiver planeCursor;


float currentFramerate;
int windowWidth;
int windowHeight;
int lastMouseX = 0;
int lastMouseY = 0;
unsigned long animationClock = 0;

//reused stuff
AbstractKinectInterface *kinect = NULL;

//capture stuff
KinectReceiver *captureReceiver = NULL;

//edit and workspace stuff
vector<KinectReceiver> frames;
vector<ObjectFragment> fragments;
KinectReceiver *currentFrame = NULL;
CullPlane *currentPlane = NULL;

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

    settings.renderOptions[RENDER_LIGHTING] = DEFAULT_LIGHTING;
    settings.renderOptions[RENDER_WIREFRAME] = DEFAULT_WIREFRAME;
    settings.renderOptions[RENDER_OSD] = DEFAULT_OSD;
    settings.running = true;

    memset(&camera, 0, sizeof(Camera));
    camera.sensitivity = 0.3f;
    camera.zoom = 1.0f;


    //CUSTOM INITIALISATION
    settings.state = STATE_CAPTURE;
    kinect = new DummyKinectInterface(640, 480, "../data/first_colour.dat");
    if (!kinect->connectToKinect())
        kinect = NULL;

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

void drawAxis() {
    float zero[] = {0.0, 0.0, 0.0};

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);

    glColor3f(1.0,0.0,0.0);
    glVertex3fv(zero);
    glVertex3f(1.0, 0.0, 0.0);

    glColor3f(0.0,1.0,0.0);
    glVertex3fv(zero);
    glVertex3f(0.0, 1.0, 0.0);

    glColor3f(0.0,0.0,1.0);
    glVertex3fv(zero);
    glVertex3f(0.0, 0.0, 1.0);

    glColor3f(1.0,1.0,1.0);

    glEnd();

    glPopAttrib();

}

void drawOSD() {
    char *bufp;
    char buffer[32];
    
    /* Backup previous "enable" state */
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);

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
    vector<ObjectModel *> renderItems;
    vector<ObjectModel *> deleteItems;

    if (STATE_WORKSPACE == settings.state) {
        //iterate through and draw the frames
        settings.cursorReceiver = &camCursor;
        settings.panTarget = NULL;
        
        //draw all fragments
        for (int i = 0; i < fragments.size(); i++)
            renderItems.push_back(&(fragments[i]));


    } else if (STATE_CAPTURE == settings.state || STATE_CAPTURE_DUMP == settings.state) {
        //just display from the device or a dump

        if (NULL == captureReceiver)
            captureReceiver = new KinectReceiver();

        if (settings.running && NULL != kinect)
            //get new depth data
            kinect->processFrame(captureReceiver);

        //things to draw
        renderItems.push_back(captureReceiver);

        settings.cursorReceiver = &camCursor;
        settings.panTarget = captureReceiver;

    } else if (
                0 != frames.size() &&
                (STATE_EDIT == settings.state ||
                STATE_EDIT_PLANE_ROTATE == settings.state ||
                STATE_EDIT_PLANE_PAN == settings.state)
                ) {

        if (settings.selectedFrame >= frames.size())
            settings.selectedFrame = 0;

        if (currentFrame != &frames[settings.selectedFrame]) {
            currentFrame = &frames[settings.selectedFrame];
        }

        //display planes and cull points
        currentFrame->showPlanes = true;
        currentFrame->cullPoints = true;

        settings.cursorReceiver = &camCursor;
        settings.panTarget = currentFrame;
        
        settings.primaryAdjustTarget = &(settings.selectedFrame);
        settings.secondaryAdjustTarget = &(settings.selectedFrame);

        //things to draw
        renderItems.push_back(currentFrame);

        if (
            (STATE_EDIT_PLANE_ROTATE == settings.state ||
            STATE_EDIT_PLANE_PAN == settings.state) &&
            currentFrame->getPlanes()->size() > 0) {
            //we're in plane rotate mode and we have at least one plane

            if (settings.selectedPlane >= currentFrame->getPlanes()->size())
                settings.selectedPlane = 0;

            settings.primaryAdjustTarget = &(settings.selectedPlane);
            settings.secondaryAdjustTarget = &(settings.selectedPlane);
            
            currentPlane = &(currentFrame->getPlanes()->at(settings.selectedPlane));

            settings.cursorReceiver = &planeCursor;
            planeCursor.plane = currentPlane;

            settings.panTarget = currentPlane;

            if (STATE_EDIT_PLANE_ROTATE == settings.state) {
                planeCursor.mode = PlaneCursorReceiver::ROTATE_MODE;
            } else {
                planeCursor.mode = PlaneCursorReceiver::PAN_MODE;
            }
            
        }
    }


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

    for (unsigned int i = 0; i < renderItems.size(); i++)
        renderItems[i]->draw();

    glPopMatrix();

    drawAxis();

    if (settings.renderOptions[RENDER_OSD])
        drawOSD();

    //delete temporary models
    for (unsigned int i = 0; i < deleteItems.size(); i++)
        delete deleteItems[i];
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
    if (NULL != settings.cursorReceiver)
        settings.cursorReceiver->mouseDown(button, state, x, y);
}

void mouseMove(int x, int y) {
    if (NULL != settings.cursorReceiver)
        settings.cursorReceiver->mouseMove(x, y);
}

void changeMode(ApplicationStates newState) {
    if (settings.state == newState)
        return;

    if (STATE_CAPTURE_DUMP == settings.state) {
        cout << "Can't change mode - currently dumping.\n";
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

    //in with the new
    if ("none" == arguments) {
        //no capture

        cout << "No source selected.\n";

#if defined(_WIN32)
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
#endif
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

    if (STATE_WORKSPACE == settings.state) {

    } else if (STATE_CAPTURE == settings.state) {

        if ("source" == directive) {
            //switch depth and colour source
            openSource(arguments);
        } else if ("stash" == directive) {
            //save the current frame
            if (NULL != captureReceiver) {
                //should copy the current frame
                frames.push_back(KinectReceiver(*captureReceiver));

                cout << "Frame #" << frames.size() - 1 << " stashed.\n";
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
    } else if (STATE_EDIT == settings.state) {

        if (NULL == currentFrame) {
            //don't bother with proceeding branches
            //
        } else if ("plane" == directive && "add" == arguments) {

            currentFrame->addPlane(CullPlane());
            cout << "Added plane #" << currentFrame->getPlanes()->size() - 1 <<
                    " to frame #" << settings.selectedFrame << "\n";

        } else if (
                "plane" == directive && "remove" == arguments && 
                !currentFrame->getPlanes()->empty()) {

            if (
                    0 > settings.selectedPlane || 
                    settings.selectedPlane >= currentFrame->getPlanes()->size()
               )
                settings.selectedPlane = 0;

            currentFrame->getPlanes()->erase(
                    currentFrame->getPlanes()->begin() + settings.selectedPlane
                    );

            cout << "Deleted plane #" << settings.selectedPlane << "\n";

        } else if (
                "plane" == directive && 
                "rotate" == arguments && 
                !currentFrame->getPlanes()->empty()
                  ) {
            //user wants to rotate / pan planes
            settings.state = STATE_EDIT_PLANE_ROTATE;

        } else if ("keep" == directive) {
            //user wants to keep this fragment 
            settings.state = STATE_EDIT_PLANE_ROTATE;

            fragments.push_back(ObjectFragment());
            currentFrame->populateFragment(fragments.back());

            cout << "Kept current state of frame #" << settings.selectedFrame << 
                    " in fragment #" << fragments.size() << "\n";

        }
    } else if (
        STATE_EDIT_PLANE_ROTATE == settings.state || 
        STATE_EDIT_PLANE_PAN == settings.state) {
            //simply return to edit state

            settings.cursorReceiver = &camCursor;
            settings.state = STATE_EDIT;
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


    //PANNING
    } else if (key == SDLK_w && NULL != settings.panTarget)
        settings.panTarget->pan(vec3f(0.0, 0.0, -TRANSLATE_DELTA));
    else if (key == SDLK_a && NULL != settings.panTarget)
        settings.panTarget->pan(vec3f(TRANSLATE_DELTA, 0.0, 0.0));
    else if (key == SDLK_s && NULL != settings.panTarget)
        settings.panTarget->pan(vec3f(0.0, 0.0, TRANSLATE_DELTA));
    else if (key == SDLK_d && NULL != settings.panTarget)
        settings.panTarget->pan(vec3f(-TRANSLATE_DELTA, 0.0, 0.0));
    else if (key == SDLK_r && NULL != settings.panTarget)
        settings.panTarget->pan(vec3f(0.0, TRANSLATE_DELTA, 0.0));
    else if (key == SDLK_f && NULL != settings.panTarget)
        settings.panTarget->pan(vec3f(0.0, -TRANSLATE_DELTA, 0.0));


    //SELECTING
    else if (key == SDLK_LESS && NULL != settings.primaryAdjustTarget)
        (*settings.primaryAdjustTarget)--;
    else if (key == SDLK_GREATER && NULL != settings.primaryAdjustTarget)
        (*settings.primaryAdjustTarget)++;
    else if (key == SDLK_COMMA && NULL != settings.secondaryAdjustTarget)
        (*settings.secondaryAdjustTarget)--;
    else if (key == SDLK_PERIOD && NULL != settings.secondaryAdjustTarget)
        (*settings.secondaryAdjustTarget)++;

    //PAUSE / RESUME
    else if (key == SDLK_SPACE)
        settings.running = !settings.running;

    //OTHER
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
