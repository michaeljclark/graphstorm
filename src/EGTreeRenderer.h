// See LICENSE for license details.

#pragma once

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
