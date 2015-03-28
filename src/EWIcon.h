/*
 *  EWIcon.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWIcon_H
#define EWIcon_H

class EWIcon;
typedef std::shared_ptr<EWIcon> EWIconPtr;

/* EWIconEvent */

class EWIconEvent : public EGEvent
{
public:
    static EGEventQualifier PRESSED;
    static EGEventQualifier RELEASED;
    
    typedef EWIcon emitter;

    EWIcon *icon;
    
    EWIconEvent(EGEventQualifier q, EWIcon *icon);
    
    std::string toString();
};

/* EWIcon */

class EWIcon : public EWWidget
{
protected:
    EGImagePtr image;
    EGImagePtr activeImage;
    EGfloat imageScale;
    
    EGRect iconRect;
    EGbool inIcon;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;

public:
    EWIcon(EGenum widgetFlags = 0);
    virtual ~EWIcon();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    virtual EGImagePtr getImage();
    virtual EGImagePtr getActiveImage();
    virtual EGenum getImageAlign();
    virtual EGfloat getImageScale();

    virtual void clicked(); /* emitter */

    virtual void setImage(EGImagePtr image);
    virtual void setActiveImage(EGImagePtr activeImage);
    virtual void setImageAlign(EGenum imageAlignFlags);
    virtual void setImageScale(EGfloat imageScale);

    virtual EGSize calcMinimumSize();
    virtual void layout();
    
    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
    virtual EGbool iconMouseMotion(EGMouseEvent *evt);
    virtual EGbool iconMouseReleased(EGMouseEvent *evt);
};

#endif
