/*
 *  EGBoundsRenderer.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGBoundsRenderer_h
#define EGBoundsRenderer_h

/* EGBoundsRenderer */

template <typename VecType>
class EGBoundsRenderer
{
public:
    virtual ~EGBoundsRenderer() {}
    
    typedef std::shared_ptr<EGBoundsRenderer<VecType>> RendererPtr;
    
    virtual void update(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) = 0;
    virtual void draw(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) = 0;
};

#include "EGBoundsRendererES2.h"

#endif
