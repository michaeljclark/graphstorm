/*
 *  EWButton.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWButton_H
#define EWButton_H

class EWButton;
typedef std::shared_ptr<EWButton> EWButtonPtr;

/* EWButtonEvent */

class EWButtonEvent : public EGEvent
{
public:
    static EGEventQualifier PRESSED;
    static EGEventQualifier RELEASED;
    
    typedef EWButton emitter;
    
    EWButton *button;
    
    EWButtonEvent(EGEventQualifier q, EWButton *button);
    
    std::string toString();
};

/* EWButton */

class EWButton : public EWWidget
{
protected:
    EGstring label;
    EGTextPtr text;
    EGImagePtr image;

    EGRect buttonRect;
    EGbool inButton;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;

public:
    EWButton(EGstring label);
    EWButton(EGenum widgetFlags = 0);
    virtual ~EWButton();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    virtual EGstring getLabel();
    virtual EGImagePtr getImage();
    virtual EGTextPtr getText();
    virtual EGenum getImageAlign();

    virtual void setLabel(EGstring label);
    virtual void setText(EGTextPtr text);
    virtual void setImage(EGImagePtr image);
    virtual void setImageAlign(EGenum imageAlignFlags);
    
    virtual void clicked(); /* emitter */

    virtual EGSize calcMinimumSize();
    virtual void layout();

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
    virtual EGbool buttonMouseMotion(EGMouseEvent *evt);
    virtual EGbool buttonMouseReleased(EGMouseEvent *evt);
};

#endif
