// See LICENSE for license details.

#pragma once

class EWLayout;
typedef std::shared_ptr<EWLayout> EWLayoutPtr;

/* EWLayoutData */

struct EWLayoutData : EWContainerData
{
    EGPoint point;
    EGSize size;
    
    EWLayoutData(EGPoint point, EGSize size) : point(point), size(size) {}
};

/* EWLayout  */

class EWLayout : public EWContainer
{
protected:
    
public:
    EWLayout(EGenum widgetFlags = 0);
    virtual ~EWLayout();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    virtual void addWidget(EWWidgetPtr widget, EWLayoutData *layoutData);
    virtual void addWidget(EWWidgetPtr widget, EGPoint point, EGSize size);
    virtual void removeWidget(EWWidgetPtr widget);

    virtual EGSize calcMinimumSize();
    virtual void setSize(EGSize size);    
    virtual void layout();
};
