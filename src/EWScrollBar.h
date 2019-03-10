// See LICENSE for license details.

#pragma once

class EWScrollBar;
typedef std::shared_ptr<EWScrollBar> EWScrollBarPtr;

/* EWScrollBarEvent */

class EWScrollBarEvent : public EGEvent
{
public:
    static EGEventQualifier VALUE_CHANGED;
    
    typedef EWScrollBar emitter;
    
    EWScrollBar *scrollbar;
    EGfloat value;
    
    EWScrollBarEvent(EGEventQualifier q, EWScrollBar *scrollbar, EGfloat value);
    
    std::string toString();
};

/* EWScrollBar */

class EWScrollBar : public EWWidget
{
protected:
    EGColor barColor;
    EGSize barControlSize;
    EGint barThickness;
    EGfloat value;
    
    EGRect barRect;
    EGRect controlRect;
    EGRect eventRect;
    EGbool inBar;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;
    
public:
    EWScrollBar(EGenum widgetFlags = 0);
    virtual ~EWScrollBar();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);    
    virtual const char* widgetTypeName();
    
    virtual void setVertical(EGbool vertical);
    virtual void setBarColor(EGColor barColor);
    virtual void setBarControlSize(EGSize barControlSize);
    virtual void setBarThickness(EGint barThickness);
    virtual void setValue(EGfloat value);
    
    virtual EGbool isVertical();
    virtual EGColor getBarColor();
    virtual EGSize getBarControlSize();
    virtual EGint getBarThickness();
    virtual EGfloat getValue();
    
    virtual EGSize calcMinimumSize();
    virtual void layout();
    
    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
    virtual EGbool controlDragMouseMotion(EGMouseEvent *evt);
    virtual EGbool controlDragMouseReleased(EGMouseEvent *evt);
};
