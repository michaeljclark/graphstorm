/*
 *  EWBox.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWBox_H
#define EWBox_H

class EWBox;
typedef std::shared_ptr<EWBox> EWBoxPtr;

/* EWBoxData */

struct EWBoxData : EWContainerData
{
    EGint index;

    EGSize minSize;
    EGSize prefSize;
    EGRect rect;

    EWBoxData(int index) : index(index) {}
};

/* EWBox */

class EWBox : public EWContainer
{
protected:
    EGRect* sizearray;
    EGint numitems;
    EGSize maxcellsize;
    EGSize totalsize;
    EGSize minsize;
    EGSize prefsize;

    void clearSizeArray();
    
public:
    EWBox(EGenum widgetFlags = 0);
    virtual ~EWBox();

    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    virtual void addWidget(EWWidgetPtr widget, EWBoxData *boxData);
    virtual void addWidget(EWWidgetPtr widget, int index);
    virtual void removeWidget(EWWidgetPtr widget);

    virtual EGbool isHorizontal();
    virtual EGbool isVertical();
    virtual EGbool isHomogeneous();
    virtual EGbool isExpand();
    
    virtual void setHorizontal();
    virtual void setVertical();
    virtual void setHomogeneous(EGbool homogeneous);
    virtual void setExpand(EGbool expand);

    virtual EGSize calcMinimumSize();
    virtual void setSize(EGSize size);
    
    virtual void layout();
};

#endif
