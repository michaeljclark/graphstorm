// See LICENSE for license details.

#pragma once

/* EGTextRenderer */

class EGTextRenderer
{
public:    
    virtual ~EGTextRenderer() {}

    virtual void clear() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
};
