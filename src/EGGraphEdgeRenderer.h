// See LICENSE for license details.

#pragma once

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
