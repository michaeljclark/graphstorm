/*
 *  EGLight.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGLight_H
#define EGLight_H

#include "EGVector.h"


/* EGLight */

class EGLight
{
public:
    static const EGfloat POSITION_DEFAULT[4];
    static const EGfloat SPOTDIRECTION_DEFAULT[3];
    static const EGfloat AMBIENT_DEFAULT[4];
    static const EGfloat DIFFUSE_DEFAULT[4];
    static const EGfloat SPECULAR_DEFAULT[4];
    
    EGuint lightN;
    EGbool enabled;
    EGfloat position[4];
    EGfloat spotDirection[3];
    EGfloat spotExponent;
    EGfloat spotCutoff;
    EGfloat ambient[4];
    EGfloat diffuse[4];
    EGfloat specular[4];
    EGfloat constantAttenuation;
    EGfloat linearAttenuation;
    EGfloat quadraticAttenuation;
    
    EGLight(EGint lightN) : lightN(lightN), enabled(true), spotExponent(0.0f), spotCutoff(180.0f),
        constantAttenuation(1.0f), linearAttenuation(0.0f), quadraticAttenuation(0.0f)
    {
        memcpy(position, POSITION_DEFAULT, sizeof(position));
        memcpy(spotDirection, SPOTDIRECTION_DEFAULT, sizeof(spotDirection));
        memcpy(ambient, AMBIENT_DEFAULT, sizeof(ambient));
        memcpy(diffuse, DIFFUSE_DEFAULT, sizeof(diffuse));
        memcpy(specular, SPECULAR_DEFAULT, sizeof(specular));
    }
        
    inline void setPosition(EGfloat x, EGfloat y, EGfloat z, EGfloat w)
        { position[0] = x; position[1] = y; position[2] = z; position[3] = w; }
    inline void setPosition(EGfloat v[4]) { memcpy(position, v, sizeof(position)); }
    inline void setPosition(EGVec4f v4f) { memcpy(position, v4f.vec, sizeof(position)); }
    
    inline void setSpotDirection(EGfloat x, EGfloat y, EGfloat z)
        { spotDirection[0] = x; spotDirection[1] = y; spotDirection[2] = z; }
    inline void setSpotDirection(EGfloat v[3]) { memcpy(spotDirection, v, sizeof(spotDirection)); }
    inline void setSpotDirection(EGVec3f v3f) { memcpy(spotDirection, v3f.vec, sizeof(spotDirection)); }
    
    inline void setAmbient(EGfloat r, EGfloat g, EGfloat b, EGfloat a)
        { ambient[0] = r; ambient[1] = g; ambient[2] = b; ambient[3] = a; }
    inline void setAmbient(EGfloat v[4]) { memcpy(ambient, v, sizeof(ambient)); }
    inline void setAmbient(EGVec4f v4f) { memcpy(ambient, v4f.vec, sizeof(ambient)); }
    
    inline void setDiffuse(EGfloat r, EGfloat g, EGfloat b, EGfloat a)
        { diffuse[0] = r; diffuse[1] = g; diffuse[2] = b; diffuse[3] = a; }
    inline void setDiffuse(EGfloat v[4]) { memcpy(diffuse, v, sizeof(diffuse)); }
    inline void setDiffuse(EGVec4f v4f) { memcpy(diffuse, v4f.vec, sizeof(diffuse)); }
    
    inline void setSpecular(EGfloat r, EGfloat g, EGfloat b, EGfloat a)
        { specular[0] = r; specular[1] = g; specular[2] = b; specular[3] = a; }
    inline void setSpecular(EGfloat v[4]) { memcpy(specular, v, sizeof(specular)); }
    inline void setSpecular(EGVec4f v4f) { memcpy(specular, v4f.vec, sizeof(specular)); }
    
    void setupGLState();
};

#endif /* EGLIGHT_H_ */
