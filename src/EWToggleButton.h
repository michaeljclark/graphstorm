/*
 *  EWToggleButton.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWToggleButton_H
#define EWToggleButton_H

class EWToggleButton;
typedef std::shared_ptr<EWToggleButton> EWToggleButtonPtr;

/* EWToggleButtonEvent */

class EWToggleButtonEvent : public EGEvent
{
public:
    static EGEventQualifier VALUE_CHANGED;
    
    typedef EWToggleButton emitter;

    EWToggleButton *ToggleButton;
    EGbool value;
    
    EWToggleButtonEvent(EGEventQualifier q, EWToggleButton *ToggleButton, EGbool value);
    
    std::string toString();
};

/* EWToggleButton */

class EWToggleButton : public EWWidget
{
protected:
    static const EGint TOGGLEBUTTON_WIDTH;
    static const EGint TOGGLEBUTTON_HEIGHT;
    
    EGImagePtr onImage;
    EGImagePtr offImage;
    EGbool value;
    
    EGRect toggleButtonRect;
    EGbool inToggleButton;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;
    
public:
    EWToggleButton(EGenum widgetFlags = 0);
    virtual ~EWToggleButton();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);    
    virtual const char* widgetTypeName();
        
    virtual void setValue(EGbool value);
    virtual EGbool getValue();
    
    virtual void valueChanged(EGbool value); /* emitter */
    
    virtual EGSize calcMinimumSize();
    virtual void layout();
    
    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
    virtual EGbool toggleButtonMouseMotion(EGMouseEvent *evt);
    virtual EGbool toggleButtonMouseReleased(EGMouseEvent *evt);
};

#endif
