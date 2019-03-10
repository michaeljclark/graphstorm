// See LICENSE for license details.

#pragma once

#include "EGColor.h"
#include "EGText.h"
#include "EGImage.h"

class EWWidgetRenderer;
typedef std::shared_ptr<EWWidgetRenderer> EWWidgetRendererPtr;

/* EWStrokeRectFlags */

enum EWStrokeRectFlags
{
    EWStrokeRectInner,
    EWStrokeRectCenter,
    EWStrokeRectOuter,
};

/* EWWidgetRenderer */

class EWWidgetRenderer
{
protected:
    EWWidget *widget;
    
public:
    EWWidgetRenderer(EWWidget *widget);
    virtual ~EWWidgetRenderer();
    
    virtual void draw() = 0;
    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void scissor(EGRect rect) = 0;
    virtual void fill(EGRect rect, EGColor color) = 0;
    virtual void fill(EGRect outerRect, EGRect innerRect, EGColor color) = 0;
    virtual void fill(EGPointList poly, EGColor color) = 0;
    virtual void stroke(EGRect rect, EGColor color, EGfloat width, EWStrokeRectFlags flags = EWStrokeRectCenter) = 0;
    virtual void stroke(EGLine line, EGColor color, EGfloat width) = 0;
    virtual void stroke(EGPointList poly, EGColor color, EGfloat width, EGbool closed = true) = 0;
    virtual void paint(EGRect rect, EGImagePtr image, EGColor color = EGColor(1, 1, 1, 1), EGfloat *uvs = NULL) = 0;
    virtual void paint(EGPoint point, EGTextPtr text, EGColor color = EGColor(1, 1, 1, 1)) = 0;
};
