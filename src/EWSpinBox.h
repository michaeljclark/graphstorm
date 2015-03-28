/*
 *  EWSpinBox.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWSpinBox_H
#define EWSpinBox_H

#include "EWWidget.h"
#include "EWEntry.h"
#include "EWIcon.h"

class EWSpinBox;
typedef std::shared_ptr<EWSpinBox> EWSpinBoxPtr;

/* EWSpinBoxEvent */

class EWSpinBoxEvent : public EGEvent
{
public:
    static EGEventQualifier VALUE_CHANGED;
    
    typedef EWSpinBox emitter;

    EWSpinBox *spinbox;
    EGdouble value;
    
    EWSpinBoxEvent(EGEventQualifier q, EWSpinBox *spinbox, EGdouble value);
    
    std::string toString();
};

/* EWSpinBox */

class EWSpinBox : public EWEntry
{
protected:
    static const int CONTROL_WIDTH;
    
    EGColor controlColor;
    EGdouble value;
    EGdouble minimum;
    EGdouble maximum;
    EGdouble increment;
    EGint precision;
    EWIconPtr upIcon;
    EWIconPtr downIcon;
    
public:
    EWSpinBox(EGenum widgetFlags = 0);
    virtual ~EWSpinBox();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);
    virtual const char* widgetTypeName();
    
    virtual void setControlColor(EGColor controlColor);
    virtual void setString(EGstring string);
    virtual void setValue(EGdouble value);
    virtual void setMinimum(EGdouble minimum);
    virtual void setMaximum(EGdouble maximum);
    virtual void setIncrement(EGdouble increment);
    virtual void setPrecision(EGint precision);
    virtual void setMinimumMaximumIncrementPrecision(EGdouble minimum, EGdouble maximum, EGdouble increment, EGint precision);

    virtual EGColor getControlColor();
    virtual EGdouble getValue();
    virtual EGdouble getMinimum();
    virtual EGdouble getMaximum();
    virtual EGdouble getIncrement();
    virtual EGint getPrecision();
    
    virtual void valueChanged(EGfloat value); /* emitter */
    
    virtual void setNeedsLayout(EGbool propagateUpwards = true);
    virtual EGSize calcMinimumSize();
    virtual void setSize(EGSize size);
    virtual void setPosition(EGPoint pos);
    virtual void layout();
    virtual void draw();
    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool iconEvent(EWIconEvent *evt);
    virtual EGbool entryEvent(EWEntryEvent *evt);
};

#endif
