/*
 *  EGViewport.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGViewport_H
#define EGViewport_H

class EGViewport;
typedef std::shared_ptr<EGViewport> EGViewportPtr;


/* EGViewport */

class EGViewport
{
public:
    EGfloat screenRotation;          // current screen rotation in degrees
    EGint screenWidth;               // current window/screen height and width
    EGint screenHeight;              // current window/screen height and width
    EGint actualWidth;               // real window/screen height and width
    EGint actualHeight;              // real window/screen height and width
    EGfloat scaleFactor;             // scale factor
    EGint viewportX;                 // world space viewport X
    EGint viewportY;                 // world space viewport Y
    EGint viewportWidth;             // world space viewport width
    EGint viewportHeight;            // world space viewport height
    EGint screenViewportX;           // screen space viewport X
    EGint screenViewportY;           // screen space viewport Y
    EGint screenViewportWidth;       // screen space viewport width
    EGint screenViewportHeight;      // screen space viewport height

    EGfloat modelViewProjectionMatrix[16];

    EGViewport() : screenRotation(0) {}
};

#endif
