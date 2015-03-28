/*
 *  EGGraphStormGLFW.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphStormGLFW_H
#define EGGraphStormGLFW_H

class EGGraphApp;
typedef std::shared_ptr<EGGraphApp> EGGraphAppPtr;


/* EGGraphStormGLFW */

class EGGraphStormGLFW
{
public:
    static const EGuint HZ = 25;

    static EGGraphAppPtr graphapp;
    
    GLFWwindow* window;
    
    void run();

    static void error(int error, const char* description);
    static void key(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scroll(GLFWwindow* window, double x, double y);
    static void reshape(GLFWwindow* window, int width, int height);
    static void mouse_button(GLFWwindow* window, int button, int action, int mods);
    static void mouse_motion(GLFWwindow* window, double x, double y);
};

#endif
