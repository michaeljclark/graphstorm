// See LICENSE for license details.

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

