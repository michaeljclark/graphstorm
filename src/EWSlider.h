// See LICENSE for license details.

#pragma once

class EWSlider;
typedef std::shared_ptr<EWSlider> EWSliderPtr;

/* EWSliderEvent */

class EWSliderEvent : public EGEvent
{
public:
    static EGEventQualifier VALUE_CHANGED;
    
    typedef EWSlider emitter;

    EWSlider *slider;
    EGfloat value;
    
    EWSliderEvent(EGEventQualifier q, EWSlider *slider, EGfloat value);
    
    std::string toString();
};

/* EWSlider */

class EWSlider : public EWWidget
{
protected:
    static const int MIN_SLIDER_LENGTH;
    
    EGSize sliderControlSize;
    EGint sliderThickness;
    EGfloat value;
    
    EGRect sliderRect;
    EGRect controlRect;
    EGRect eventRect;
    EGbool inSlider;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;

public:
    EWSlider(EGenum widgetFlags = 0);
    virtual ~EWSlider();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);    
    virtual const char* widgetTypeName();

    virtual void setVertical(EGbool vertical);
    virtual void setSliderControlSize(EGSize sliderControlSize);
    virtual void setSliderThickness(EGint sliderThickness);
    virtual void setValue(EGfloat value);
    
    virtual EGbool isVertical();
    virtual EGSize getSliderControlSize();
    virtual EGint getSliderThickness();
    virtual EGfloat getValue();
    
    void valueChanged(EGfloat value); /* emitter */

    virtual EGSize calcMinimumSize();
    virtual void layout();

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
    virtual EGbool controlDragMouseMotion(EGMouseEvent *evt);
    virtual EGbool controlDragMouseReleased(EGMouseEvent *evt);
};
