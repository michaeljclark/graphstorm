// See LICENSE for license details.

#pragma once

class EGGraphStormWidgetsGLUT
{
public:
    static const EGuint HZ = 25;
    
    static EGGraphStormWidgetsGLUT glutApp;
    
    EGGraphStormWidgetsPtr widgets;
    EGint glutWindowId;
    
    EGGraphStormWidgetsGLUT();
    virtual ~EGGraphStormWidgetsGLUT();
    
    void init();
    
    static void glutDisplay();
    static void glutReshape(int w, int h);
    static void glutMouse(int button, int state, int x, int y);
    static void glutTimer(int value);
    static void glutMotion(int x, int y);
    static void glutPassiveMotion(int x, int y);
    static void glutKey(unsigned char inkey, int x, int y);
    static void glutSpecial(int key, int x, int y);
};
