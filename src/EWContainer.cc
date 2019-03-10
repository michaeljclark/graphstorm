// See LICENSE for license details.

#include "EWWidget.h"
#include "EWContainer.h"


/* EWContainerData */

EWContainerData::EWContainerData() {}

EWContainerData::~EWContainerData() {}


/* EWContainer */

EWContainer::EWContainer(EGenum widgetFlags) : EWWidget(widgetFlags), children(), datamap() {}

EWContainer::~EWContainer() {}

const EGClass* EWContainer::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWContainer>()->extends<EWWidget>();
    return clazz;
}

const EGClass* EWContainer::getClass() const { return classFactory(); }

EWWidgetList* EWContainer::getChildren()
{
    return &children;
}

EGbool EWContainer::mouseEvent(EGMouseEvent *evt)
{
    for (EWWidgetList::reverse_iterator wi = children.rbegin(); wi != children.rend(); wi++) {
        EWWidgetPtr &widget = *wi;
        if (!widget->isEnabled()) continue;
        if (widget->getRect().contains(widget->pointToView(EGPoint(evt->x, evt->y)))) {
            return (*wi)->mouseEvent(evt);
        }
    }
    return false;
}
