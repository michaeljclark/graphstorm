/*
 *  EGRenderBatchText.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGRenderBatchText_H
#define EGRenderBatchText_H

/* EGRenderBatchText */

struct EGRenderBatchText : EGRenderBatchBase
{
    std::string batchName;
    EGTextPtr text;
    
    EGRenderBatchText(std::string batchName) : batchName(batchName) {}
    
    void draw()
    {
        if (text) {
            text->draw();
        }
    }
};

#endif
