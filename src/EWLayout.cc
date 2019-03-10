// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWContainer.h"
#include "EWLayout.h"


/* EWLayout */

static char class_name[] = "EWLayout";

EWLayout::EWLayout(EGenum widgetFlags) : EWContainer(widgetFlags)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWLayout::~EWLayout() {}

const EGClass* EWLayout::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWLayout>()->extends<EWContainer>();
    return clazz;
}

const EGClass* EWLayout::getClass() const { return classFactory(); }

const char* EWLayout::widgetTypeName() { return class_name; }

void EWLayout::addWidget(EWWidgetPtr widget, EWLayoutData *layoutData)
{
    if (widget) {
        widget->setParent(this);
        children.push_back(widget);
        datamap.insert(EWContainerDataEntry(widget, EWContainerDataPtr(layoutData)));
    }
}

void EWLayout::addWidget(EWWidgetPtr widget, EGPoint point, EGSize size)
{
    addWidget(widget, new EWLayoutData(point, size));
}

void EWLayout::removeWidget(EWWidgetPtr widget)
{
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end();) {
        if (*wi == widget) {
            widget->setParent(NULL);
            wi = children.erase(wi);
        } else {
            wi++;
        }
    }
    datamap.erase(widget);
}

EGSize EWLayout::calcMinimumSize()
{
    return EGSize(preferredSize.width, preferredSize.height);
    // TODO
}

void EWLayout::setSize(EGSize size)
{
    rect = EGRect(rect.x, rect.y, size.width, size.height);
    // TODO
}

void EWLayout::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    renderer->begin();
    
    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(margin),
                       rect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(margin).contract(borderWidth), fillColor);
    }
    
    renderer->end();

    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        (*wi)->layout();
    }

    needsLayout = false;
}
