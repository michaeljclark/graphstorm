/*
 *  EWContext.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWContext_H
#define EWContext_H

class EGViewport;
typedef std::shared_ptr<EGViewport> EGViewportPtr;
class EWContext;
typedef std::shared_ptr<EWContext> EWContextPtr;

/* EWContext */

class EWContext : public EWContainer
{
protected:
    friend class EWWidget;
    friend class EWWindow;
    
    EGViewportPtr viewport;
    EGTime drawTime;
    EWWidgetPtr focusWidget;
    EGEventInfoRef mousePressedEvent;
    EGEventInfoRef mouseReleasedEvent;
    EGEventInfoRef keyPressedEvent;
    EGEventInfoRef keyReleasedEvent;
    
    EGfloat matrix[16];
    EGfloat invmatrix[16];
    
    virtual void sortTopLevelWidgets();

public:
    EWContext(EGViewportPtr viewport, EGenum widgetFlags = 0);
    virtual ~EWContext();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    virtual void addWidget(EWWidgetPtr widget);
    virtual void removeWidget(EWWidgetPtr widget);
    
    virtual EGViewportPtr getViewport();
    virtual const EGTime& getDrawTime();
    
    virtual EGfloat* getInverseTransform();
    virtual EGfloat* getTransform();
    
    virtual void reshape();
    virtual void draw();
    
    EGbool keyEvent(EGKeyEvent *evt);
};

#endif
