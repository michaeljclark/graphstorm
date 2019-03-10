// See LICENSE for license details.

#include "EGGraphStormWidgets.h"
#include "EGGraphStormWidgetsGLUT.h"

#if TARGET_OS_MAC
#include <GLUT/glut.h>
#else
#include <GL/freeglut_std.h>
#include <GL/freeglut_ext.h>
#endif

/* EGGraphStormWidgetsGLUT */

EGGraphStormWidgetsGLUT EGGraphStormWidgetsGLUT::glutApp;

EGGraphStormWidgetsGLUT::EGGraphStormWidgetsGLUT()
{
    widgets = EGGraphStormWidgetsPtr(new EGGraphStormWidgets());
}

EGGraphStormWidgetsGLUT::~EGGraphStormWidgetsGLUT()
{
    glutDestroyWindow(glutWindowId);
}

void EGGraphStormWidgetsGLUT::init()
{
    // create window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(800, 600);
    glutWindowId = glutCreateWindow("GraphStorm");
    glEnable(GL_MULTISAMPLE);
 
    widgets->init();
    
    glutDisplayFunc(glutDisplay);
    glutReshapeFunc(glutReshape);
    glutKeyboardFunc(glutKey);
    glutSpecialFunc(glutSpecial);
    glutMouseFunc(glutMouse);
    glutMotionFunc(glutMotion);
    glutPassiveMotionFunc(glutPassiveMotion);
    
    // start timer events
    glutTimerFunc(1000/HZ, glutTimer, 0);
}

void EGGraphStormWidgetsGLUT::glutMouse(int button, int state, int x, int y)
{
    EGEventQualifier q = NULL;
    EGint b = 0;
    switch (state) {
        case GLUT_DOWN: q = EGMouseEvent::PRESSED;   break;
        case GLUT_UP:   q = EGMouseEvent::RELEASED;  break;
    }
    switch (button) {
        case GLUT_LEFT_BUTTON:   b = EGMouseEvent::LEFT_BUTTON;    break;
        case GLUT_MIDDLE_BUTTON: b = EGMouseEvent::MIDDLE_BUTTON;  break;
        case GLUT_RIGHT_BUTTON:  b = EGMouseEvent::RIGHT_BUTTON;   break;
    }
    
    EGMouseEvent evtPressed(q, x, y, b);
    if (EGEventManager::postEvent(&evtPressed)) {
        return;
    }
}

void EGGraphStormWidgetsGLUT::glutDisplay()
{
    glutApp.widgets->draw();
    glutSwapBuffers();
}

void EGGraphStormWidgetsGLUT::glutReshape(int w, int h)
{
    glutApp.widgets->reshape(w, h);
}

void EGGraphStormWidgetsGLUT::glutTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(1000/HZ, glutTimer, 0);
}

void EGGraphStormWidgetsGLUT::glutMotion(int x, int y)
{
    EGMouseEvent evtMotion(EGMouseEvent::MOTION, x, y);
    if (EGEventManager::postEvent(&evtMotion)) {
        return;
    }
}

void EGGraphStormWidgetsGLUT::glutPassiveMotion(int x, int y)
{
    EGMouseEvent evtMotion(EGMouseEvent::MOTION, x, y);
    if (EGEventManager::postEvent(&evtMotion)) {
        return;
    }
}

void EGGraphStormWidgetsGLUT::glutKey(unsigned char inkey, int x, int y)
{
    EGKeyEvent evtPressed(EGKeyEvent::PRESSED, x, y, inkey, 0, glutGetModifiers());
    EGEventManager::postEvent(&evtPressed);
    
    EGKeyEvent evtReleased(EGKeyEvent::RELEASED, x, y, inkey, 0, glutGetModifiers());
    EGEventManager::postEvent(&evtReleased);
}

void EGGraphStormWidgetsGLUT::glutSpecial(int key, int x, int y)
{
    EGint c = 0;
    
    switch (key) {
        case GLUT_KEY_F1:        c = EGKeyEvent::KEYCODE_F1;        break;
        case GLUT_KEY_F2:        c = EGKeyEvent::KEYCODE_F2;        break;
        case GLUT_KEY_F3:        c = EGKeyEvent::KEYCODE_F3;        break;
        case GLUT_KEY_F4:        c = EGKeyEvent::KEYCODE_F4;        break;
        case GLUT_KEY_F5:        c = EGKeyEvent::KEYCODE_F5;        break;
        case GLUT_KEY_F6:        c = EGKeyEvent::KEYCODE_F6;        break;
        case GLUT_KEY_F7:        c = EGKeyEvent::KEYCODE_F7;        break;
        case GLUT_KEY_F8:        c = EGKeyEvent::KEYCODE_F8;        break;
        case GLUT_KEY_F9:        c = EGKeyEvent::KEYCODE_F9;        break;
        case GLUT_KEY_F10:       c = EGKeyEvent::KEYCODE_F10;       break;
        case GLUT_KEY_F11:       c = EGKeyEvent::KEYCODE_F11;       break;
        case GLUT_KEY_F12:       c = EGKeyEvent::KEYCODE_F12;       break;
        case GLUT_KEY_LEFT:      c = EGKeyEvent::KEYCODE_LEFT;      break;
        case GLUT_KEY_UP:        c = EGKeyEvent::KEYCODE_UP;        break;
        case GLUT_KEY_RIGHT:     c = EGKeyEvent::KEYCODE_RIGHT;     break;
        case GLUT_KEY_DOWN:      c = EGKeyEvent::KEYCODE_DOWN;      break;
        case GLUT_KEY_PAGE_UP:   c = EGKeyEvent::KEYCODE_PAGE_UP;   break;
        case GLUT_KEY_PAGE_DOWN: c = EGKeyEvent::KEYCODE_PAGE_DOWN; break;
        case GLUT_KEY_HOME:      c = EGKeyEvent::KEYCODE_HOME;      break;
        case GLUT_KEY_END:       c = EGKeyEvent::KEYCODE_END;       break;
        case GLUT_KEY_INSERT:    c = EGKeyEvent::KEYCODE_INSERT;    break;
    }
    
    EGKeyEvent evtPressed(EGKeyEvent::PRESSED, x, y, 0, c, glutGetModifiers());
    EGEventManager::postEvent(&evtPressed);
    
    EGKeyEvent evtReleased(EGKeyEvent::RELEASED, x, y, 0, c, glutGetModifiers());
    EGEventManager::postEvent(&evtReleased);
}

extern "C" int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    EGGraphStormWidgetsGLUT::glutApp.init();
    glutMainLoop();
}

#if defined (_WIN32) && !defined (_CONSOLE)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char *argv[] = { "gswidgets.exe", NULL };
    int argc = 1;
    return main(argc, argv);
}

#endif
