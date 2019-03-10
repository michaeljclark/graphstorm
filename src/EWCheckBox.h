// See LICENSE for license details.

#pragma once

class EWCheckBox;
typedef std::shared_ptr<EWCheckBox> EWCheckBoxPtr;

/* EWCheckBoxEvent */

class EWCheckBoxEvent : public EGEvent
{
public:
    static EGEventQualifier VALUE_CHANGED;
    
    typedef EWCheckBox emitter;

    EWCheckBox *checkbox;
    EGbool value;
    
    EWCheckBoxEvent(EGEventQualifier q, EWCheckBox *checkbox, EGbool value);
    
    std::string toString();
};

/* EWCheckBox */

class EWCheckBox : public EWWidget
{
protected:
    static const EGint CHECKBOX_WIDTH;
    static const EGint CHECKBOX_HEIGHT;
    
    EGImagePtr onImage;
    EGImagePtr offImage;
    EGbool value;
    
    EGRect checkBoxRect;
    EGbool inCheckBox;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;
    
public:
    EWCheckBox(EGenum widgetFlags = 0);
    virtual ~EWCheckBox();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();
    virtual void setDefaults(const char *class_name);
    
    virtual void setValue(EGbool value);
    virtual EGbool getValue();
    
    virtual void valueChanged(EGbool value); /* emitter */

    virtual EGSize calcMinimumSize();
    virtual void layout();

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
    virtual EGbool checkBoxMouseMotion(EGMouseEvent *evt);
    virtual EGbool checkBoxMouseReleased(EGMouseEvent *evt);
};
