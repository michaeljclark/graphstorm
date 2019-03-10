// See LICENSE for license details.

#pragma once

class EWContainer;
typedef std::shared_ptr<EWContainer> EWContainerPtr;

struct EWContainerData;
typedef std::shared_ptr<EWContainerData> EWContainerDataPtr;
typedef std::pair<EWWidgetPtr,EWContainerDataPtr> EWContainerDataEntry;
typedef std::map<EWWidgetPtr,EWContainerDataPtr> EWContainerDataMap;

/* EWContainerData */

struct EWContainerData
{
    EWContainerData();
    virtual ~EWContainerData();
};

/* EWContainer */

class EWContainer : public EWWidget
{
protected:
    EWWidgetList children;
    EWContainerDataMap datamap;
    
public:
    EWContainer(EGenum widgetFlags);
    virtual ~EWContainer();

    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;

    virtual EWWidgetList* getChildren();

    EGbool mouseEvent(EGMouseEvent *evt);
};
