// See LICENSE for license details.

#pragma once

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
