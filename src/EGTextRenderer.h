/*
 *  EGTextRenderer.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGTextRenderer_H
#define EGTextRenderer_H


/* EGTextRenderer */

class EGTextRenderer
{
public:    
    virtual ~EGTextRenderer() {}

    virtual void clear() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
};

#endif
