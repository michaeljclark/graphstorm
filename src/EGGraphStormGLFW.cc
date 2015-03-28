/*
 *  EGGraphStormGLFW.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */


#include "EG.h"
#include "EGGL.h"

#include <GLFW/glfw3.h>

#include "EGGraphApp.h"
#include "EGGraphStormGLFW.h"


/* EGGraphStormGLFW */

static const char class_name[] = "EGGraphStormGLFW";

static double mx, my;

EGGraphAppPtr EGGraphStormGLFW::graphapp;

void EGGraphStormGLFW::run()
{
    glfwSetErrorCallback(error);

    // initialize glfw
    if (!glfwInit())
        exit(9);

    // create window
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, 0);
    glfwWindowHint(GLFW_RESIZABLE, 1);
    glfwWindowHint(GLFW_DECORATED, 1);
    window = glfwCreateWindow(1024, 768, "GraphStorm", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(9);
    }
    glfwSetWindowPos(window, 50, 50);
    glfwShowWindow(window);

    glfwSetKeyCallback(window, key);
    glfwSetScrollCallback(window, scroll);
    glfwSetFramebufferSizeCallback(window, reshape);
    glfwSetMouseButtonCallback(window, mouse_button);
    glfwSetCursorPosCallback(window, mouse_motion);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // init graph app
    graphapp = EGGraphAppPtr(new EGGraphApp());
    graphapp->init();
    graphapp->graphappui = EGGraphAppUIPtr(new EGGraphAppUI(graphapp.get()));
    graphapp->initCL();

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
    graphapp->reshape(width, height);

    while (!glfwWindowShouldClose(window))
    {
        graphapp->draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (graphapp->graphappui->isRunning()) {
            graphapp->layoutImpl->step();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void EGGraphStormGLFW::error(int error, const char* description)
{

}

void EGGraphStormGLFW::key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        EGKeyEvent evtPressed(EGKeyEvent::PRESSED, mx, my, key, scancode, mods);
        EGEventManager::postEvent(&evtPressed);
    } else if (action == GLFW_RELEASE) {
        EGKeyEvent evtReleased(EGKeyEvent::RELEASED, mx, my, key, scancode, mods);
        EGEventManager::postEvent(&evtReleased);
    }
}

void EGGraphStormGLFW::scroll(GLFWwindow* window, double x, double y)
{

}

void EGGraphStormGLFW::reshape(GLFWwindow* window, int width, int height)
{
    graphapp->reshape(width, height);
}

void EGGraphStormGLFW::mouse_button(GLFWwindow* window, int button, int action, int mods)
{
    EGEventQualifier q = NULL;
    EGint b = 0;
    switch (action) {
        case GLFW_PRESS:    q = EGMouseEvent::PRESSED;   break;
        case GLFW_RELEASE:  q = EGMouseEvent::RELEASED;  break;
    }
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:   b = EGMouseEvent::LEFT_BUTTON;    break;
        case GLFW_MOUSE_BUTTON_MIDDLE: b = EGMouseEvent::MIDDLE_BUTTON;  break;
        case GLFW_MOUSE_BUTTON_RIGHT:  b = EGMouseEvent::RIGHT_BUTTON;   break;
    }
    
    EGMouseEvent evtPressed(q, mx, my, b);
    if (EGEventManager::postEvent(&evtPressed)) {
        return;
    }
    
    EGenum buttons = (button == GLFW_MOUSE_BUTTON_LEFT ? EGGraphAppMouseButtonLeft : 0) |
                     (button == GLFW_MOUSE_BUTTON_RIGHT ? EGGraphAppMouseButtonRight : 0);
    if (action == GLFW_PRESS) {
        graphapp->mousePress(mx, my, buttons);
    } else if (action == GLFW_RELEASE) {
        graphapp->mouseRelease(mx, my, buttons);
    }
}

void EGGraphStormGLFW::mouse_motion(GLFWwindow* window, double x, double y)
{
    mx = x;
    my = y;

    EGMouseEvent evtMotion(EGMouseEvent::MOTION, x, y);
    if (EGEventManager::postEvent(&evtMotion)) {
        return;
    }
    
    graphapp->mouseMove(x, y, 0);
}

extern "C" int main(int argc, char **argv)
{
    EGGraphStormGLFW app;

    app.run();
}

#if defined (_WIN32) && !defined (_CONSOLE)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char *argv[] = { "gsglut.exe", NULL };
    int argc = 1;
    return main(argc, argv);
}

#endif
