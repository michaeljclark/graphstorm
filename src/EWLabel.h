/*
 *  EWLabel.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWLabel_H
#define EWLabel_H

class EWLabel;
typedef std::shared_ptr<EWLabel> EWLabelPtr;

/* EWLabel */

class EWLabel : public EWWidget
{
protected:
    EGstring label;
    EGTextPtr text;
    
public:
    EWLabel(EGenum widgetFlags = 0);
    EWLabel(EGstring label);
    virtual ~EWLabel();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    virtual EGstring getLabel();
    virtual EGTextPtr getText();
    
    virtual void setLabel(EGstring label);
    virtual void setText(EGTextPtr text);

    virtual EGSize calcMinimumSize();
    virtual void layout();
};

#endif
