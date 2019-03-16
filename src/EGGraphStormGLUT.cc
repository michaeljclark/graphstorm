// See LICENSE for license details.

#include "EGGraphApp.h"
#include "EGGraphStormGLUT.h"

#if TARGET_OS_MAC
#include <GLUT/glut.h>
#else
#include <GL/freeglut_std.h>
#include <GL/freeglut_ext.h>
#endif

/* EGGraphStormGLUT */

EGGraphAppPtr EGGraphStormGLUT::graphapp;
EGint EGGraphStormGLUT::glutWindowId;

EGGraphStormGLUT::EGGraphStormGLUT() {}

EGGraphStormGLUT::~EGGraphStormGLUT()
{
    glutDestroyWindow(glutWindowId);
}

void EGGraphStormGLUT::init()
{
    // setup context
    glutInitContextVersion(3, 2);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    // create window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1024, 768);
    glutWindowId = glutCreateWindow("GraphStorm");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

    // init graph app
    graphapp = EGGraphAppPtr(new EGGraphApp());
    graphapp->init();
    graphapp->graphappui = EGGraphAppUIPtr(new EGGraphAppUI(graphapp.get()));
    graphapp->initCL();

#if !TARGET_OS_MAC
    glutCloseFunc(exit);
#endif
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);
    
    // spring layout settings
    EGSpringLayoutSettings settings(/* stiffness */        400.0f,
                                    /* repuslision */      400.0f,
                                    /* centerAttraction */ 0.0f, /* repulsion * 0.02 = 8.0f */
                                    /* damping */          0.5f,
                                    /* timeStep */         0.03f,
                                    /* maxSpeed */         1000.0f,
                                    /* maxEnergy */        0.01f);

    // setup graph
    EGGraphModifierList modifierList;
    modifierList.push_back(EGGraphModifierPtr(new EGGraphNodeColorModifier(EGGraphNodeColorTypeConstant, EGColor(1,1,1,1))));
    modifierList.push_back(EGGraphModifierPtr(new EGGraphEdgeColorModifier(EGGraphEdgeColorTypeConstant, EGColor(1,1,1,1))));

    EGGraphLayoutFactory::init();
    graphapp->graphappui->reset();

    // start idle fund and timer events
    glutIdleFunc(idle);
    glutTimerFunc(1000/HZ, timer, 0);
}

void EGGraphStormGLUT::exit()
{
    graphapp->clearGraph();
#if !TARGET_OS_MAC
    glutLeaveMainLoop();
#endif
}

void EGGraphStormGLUT::mouse(int button, int state, int x, int y)
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
    
    EGenum buttons = (button == GLUT_LEFT_BUTTON ? EGGraphAppMouseButtonLeft : 0) |
                     (button == GLUT_RIGHT_BUTTON ? EGGraphAppMouseButtonRight : 0);
    if (state == GLUT_DOWN) {
        graphapp->mousePress(x, y, buttons);
    } else if (state == GLUT_UP) {
        graphapp->mouseRelease(x, y, buttons);
    }
}

void EGGraphStormGLUT::display()
{
    graphapp->draw();
    glutSwapBuffers();
    if (graphapp->graphappui && graphapp->graphappui->isRunning()) {
        glutIdleFunc(idle);
    }
}

void EGGraphStormGLUT::reshape(int w, int h)
{
    graphapp->reshape(w, h);
}

void EGGraphStormGLUT::idle()
{
    if (graphapp->graphappui->isRunning()) {
        graphapp->layoutImpl->step();
    } else {
        glutIdleFunc(NULL);
    }
}

void EGGraphStormGLUT::timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(1000/HZ, timer, 0);
}

void EGGraphStormGLUT::motion(int x, int y)
{
    EGMouseEvent evtMotion(EGMouseEvent::MOTION, x, y);
    if (EGEventManager::postEvent(&evtMotion)) {
        return;
    }
    
    graphapp->mouseMove(x, y, 0);
}

void EGGraphStormGLUT::passiveMotion(int x, int y)
{
    EGMouseEvent evtMotion(EGMouseEvent::MOTION, x, y);
    if (EGEventManager::postEvent(&evtMotion)) {
        return;
    }
}

void EGGraphStormGLUT::key(unsigned char inkey, int x, int y)
{
    EGKeyEvent evtPressed(EGKeyEvent::PRESSED, x, y, inkey, 0, glutGetModifiers());
    EGbool handled = EGEventManager::postEvent(&evtPressed);
    
    EGKeyEvent evtReleased(EGKeyEvent::RELEASED, x, y, inkey, 0, glutGetModifiers());
    handled |= EGEventManager::postEvent(&evtReleased);
    
    if (handled) return;

    switch (inkey) {
        case 'i':
        case 'I':
            graphapp->showInfo = !graphapp->showInfo;
            break;
        case 't':
        case 'T':
            graphapp->showTree = !graphapp->showTree;
            break;
        case 27:
        case 'q':
            exit();
            break;
    }
}

void EGGraphStormGLUT::special(int key, int x, int y)
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
    EGGraphStormGLUT app;
    
    glutInit(&argc, argv);
    app.init();
    glutMainLoop();
}

#if defined (_WIN32) && !defined (_CONSOLE)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char *argv[] = { "gsglut.exe", NULL };
    int argc = 1;
    return main(argc, argv);
}

#endif
