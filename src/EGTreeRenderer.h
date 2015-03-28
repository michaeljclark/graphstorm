/*
 *  EGTreeRenderer.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGTreeRenderer_H
#define EGTreeRenderer_H


/* EGTreeRenderer */

template <typename VecType>
class EGTreeRenderer
{
public:
    virtual ~EGTreeRenderer() {}
    
    typedef std::shared_ptr<EGTreeRenderer<VecType>> RendererPtr;
    
    virtual void update(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) = 0;
    virtual void draw(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) = 0;
};

#include "EGTreeRendererES2.h"

#endif
