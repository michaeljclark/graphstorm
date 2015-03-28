/*
 *  EGGraphNodeRenderer.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphNodeRenderer_H
#define EGGraphNodeRenderer_H


/* EGGraphNodeRenderer */

template <typename VecType>
class EGGraphNodeRenderer
{
public:
    virtual ~EGGraphNodeRenderer() {}
    
    typedef std::shared_ptr<EGGraphNodeRenderer<VecType>> RendererPtr;

    virtual void update(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) = 0;
    virtual void draw(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) = 0;
};

#include "EGGraphNodeRendererES2.h"

#endif
