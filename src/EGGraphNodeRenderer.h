// See LICENSE for license details.

#pragma once

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
