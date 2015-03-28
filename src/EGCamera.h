/*
 *  EGCamera.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGCamera_H
#define EGCamera_H

class EGCamera;
typedef std::shared_ptr<EGCamera> EGCameraPtr;
class EGViewport;
typedef std::shared_ptr<EGViewport> EGViewportPtr;


/* EGCamera */

class EGCamera
{
public:
    EGfloat viewPos[3]; // View position
    EGfloat viewDir[3]; // View direction vector
    EGfloat viewUp[3]; // View up direction
    EGfloat rotPoint[3]; // Point to rotate about
    EGfloat focalLength; // Focal Length along view direction
    EGfloat aperture; // Camera aperture
    EGfloat eyeSep; // Eye separation
    
    void setupRenderState(EGViewportPtr &viewport);
};

#endif
