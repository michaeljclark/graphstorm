/*
 *  EGGraphEdgeRenderer.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphEdgeRenderer_H
#define EGGraphEdgeRenderer_H

/* EGGraphEdgeRenderer */

template <typename VecType>
class EGGraphEdgeRenderer
{
public:
    virtual ~EGGraphEdgeRenderer() {}
    
    typedef std::shared_ptr<EGGraphEdgeRenderer<VecType>> RendererPtr;

    virtual void update(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) = 0;
    virtual void draw(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) = 0;
};

#include "EGGraphEdgeRendererES2.h"

#endif
