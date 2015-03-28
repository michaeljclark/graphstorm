/*
 *  EGGraphStormGLUT.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphStormGLUT_H
#define EGGraphStormGLUT_H

class EGGraphApp;
typedef std::shared_ptr<EGGraphApp> EGGraphAppPtr;


/* EGGraphStormGLUT */

class EGGraphStormGLUT
{
public:
    static const EGuint HZ = 25;

    static EGGraphAppPtr graphapp;
    static EGint glutWindowId;

    EGGraphStormGLUT();
    virtual ~EGGraphStormGLUT();

    void init();
        
    static void exit();
    static void display();
    static void reshape(int w, int h);
    static void mouse(int button, int state, int x, int y);
    static void idle();
    static void timer(int value);
    static void motion(int x, int y);
    static void passiveMotion(int x, int y);
    static void key(unsigned char inkey, int x, int y);
    static void special(int key, int x, int y);
};

#endif
