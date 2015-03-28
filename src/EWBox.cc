/*
 *  EWBox.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWContainer.h"
#include "EWBox.h"

/* EWBox */

static char class_name[] = "EWBox";

EWBox::EWBox(EGenum widgetFlags) : EWContainer(widgetFlags), sizearray(NULL), numitems(0), maxcellsize(), totalsize(), minsize()
{
    if (widgetDefaults->getBoolean(class_name, "vertical", true)) {
        setVertical();
    } else {
        setHorizontal();
    }
}

EWBox::~EWBox()
{
    clearSizeArray();
}

void EWBox::clearSizeArray()
{
    if (sizearray) {
        delete [] sizearray;
    }
}

const EGClass* EWBox::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWBox>()->extends<EWContainer>();
    return clazz;
}

const EGClass* EWBox::getClass() const { return classFactory(); }

const char* EWBox::widgetTypeName() { return class_name; }

void EWBox::addWidget(EWWidgetPtr widget, EWBoxData *boxData)
{
    if (widget) {
        widget->setParent(this);
        children.push_back(widget);
        datamap.insert(EWContainerDataEntry(widget, EWContainerDataPtr(boxData)));
    }
}

void EWBox::addWidget(EWWidgetPtr widget, int index)
{
    addWidget(widget, new EWBoxData(index));
}

void EWBox::removeWidget(EWWidgetPtr widget)
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

EGbool EWBox::isHorizontal() { return widgetFlags & EWWidgetFlagsLayoutHorizontal ? true : false; }
EGbool EWBox::isVertical() { return widgetFlags & EWWidgetFlagsLayoutVertical ? true : false; }
EGbool EWBox::isHomogeneous()  { return widgetFlags & EWWidgetFlagsLayoutHomogeneous ? true : false; }
EGbool EWBox::isExpand()  { return widgetFlags & EWWidgetFlagsLayoutExpand ? true : false; }

void EWBox::setHorizontal()
{
    if (!isHorizontal()) {
        widgetFlags &= ~EWWidgetFlagsLayoutVertical;
        widgetFlags |= EWWidgetFlagsLayoutHorizontal;
        setNeedsLayout();
    }
}

void EWBox::setVertical()
{
    if (!isVertical()) {
        widgetFlags &= ~EWWidgetFlagsLayoutHorizontal;
        widgetFlags |= EWWidgetFlagsLayoutVertical;
        setNeedsLayout();
    }
}

void EWBox::setHomogeneous(EGbool homogeneous)
{
    if (isHomogeneous() && !homogeneous) {
        widgetFlags &= ~EWWidgetFlagsLayoutHomogeneous;
        setNeedsLayout();
    } else if (!isHomogeneous() && homogeneous) {
        widgetFlags |= EWWidgetFlagsLayoutHomogeneous;
        setNeedsLayout();
    }
}

void EWBox::setExpand(EGbool expand)
{
    if (isExpand() && !expand) {
        widgetFlags &= ~EWWidgetFlagsLayoutExpand;
        setNeedsLayout();
    } else if (!isExpand() && expand) {
        widgetFlags |= EWWidgetFlagsLayoutExpand;
        setNeedsLayout();
    }
}

EGSize EWBox::calcMinimumSize()
{
    // get child minimum sizes
    EGint numitems = 0;
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        EWWidgetPtr child = *wi;
        EWContainerDataMap::iterator di = datamap.find(child);
        if (di != datamap.end()) {
            EWContainerDataEntry entry = *di;
            EWBoxData *boxData = static_cast<EWBoxData*>(entry.second.get());
            boxData->minSize = child->calcMinimumSize();
            boxData->prefSize = child->calcPreferredSize();
            numitems = (std::max)(numitems, boxData->index + 1);
        }
    }
    
    // populate temporary size array
    if (!sizearray || numitems != this->numitems) {
        clearSizeArray();
        sizearray = new EGRect[numitems];
        this->numitems = numitems;
    }
    for (EGint i = 0; i < numitems; i++) {
        sizearray[i] = EGRect();
    }
    
    // fill size array
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        EWWidgetPtr child = *wi;
        EWContainerDataMap::iterator di = datamap.find(child);
        if (di != datamap.end()) {
            EWContainerDataEntry entry = *di;
            EWBoxData *boxData = static_cast<EWBoxData*>(entry.second.get());
            sizearray[boxData->index].x = 0;
            sizearray[boxData->index].y = 0;
            sizearray[boxData->index].width = boxData->prefSize.width;
            sizearray[boxData->index].height = boxData->prefSize.height;
        }
    }
    
    // calculate max and total for width and height
    totalsize = maxcellsize = EGSize();
    for (EGint i = 0; i < numitems; i++) {
        maxcellsize.width = (std::max)(maxcellsize.width, sizearray[i].width);
        maxcellsize.height = (std::max)(maxcellsize.height, sizearray[i].height);
        totalsize.width += sizearray[i].width;
        totalsize.height += sizearray[i].height;
    }
    
    // calculate minimum overall size
    if (isVertical()) {
        minsize.width = maxcellsize.width;
        minsize.height = isHomogeneous() ? maxcellsize.height * numitems : totalsize.height;
    } else {
        minsize.width = isHomogeneous() ? maxcellsize.width * numitems : totalsize.width;
        minsize.height = maxcellsize.height;
    }
    
    return minsize;
}

void EWBox::setSize(EGSize size)
{
    rect = EGRect(rect.x, rect.y, size.width, size.height);
    
    calcMinimumSize();
    
    // set cell sizes based on available space
    if (isVertical()) {
        EGfloat ratio = isExpand() && minsize.height > 0 ? (EGfloat)size.height / (EGfloat)minsize.height : 1.0f;
        EGint totalheight = 0;
        for (EGint i = 0; i < numitems; i++) {
            sizearray[i].width = size.width;
            sizearray[i].height = (EGint)((isHomogeneous() ? maxcellsize.height : sizearray[i].height) * ratio);
            totalheight += sizearray[i].height;
        }
        if (isExpand()) {
            while (totalheight < size.height) {
                for (EGint i = 0; i < numitems && totalheight < size.height; i++) {
                    sizearray[i].height++;
                    totalheight++;
                }
            }
        }
        EGint y = 0;
        for (EGint i = 0; i < numitems; i++) {
            sizearray[i].x = 0;
            sizearray[i].y = y;
            y += sizearray[i].height;
        }
    } else {
        EGfloat ratio = isExpand() && minsize.width > 0 ? (EGfloat)size.width / (EGfloat)minsize.width : 1.0f;
        EGint totalwidth = 0;
        for (EGint i = 0; i < numitems; i++) {
            sizearray[i].width = (EGint)((isHomogeneous() ? maxcellsize.width : sizearray[i].width) * ratio);
            sizearray[i].height = size.height;
            totalwidth += sizearray[i].width;
        }
        if (isExpand()) {
            while (totalwidth < size.width) {
                for (EGint i = 0; i < numitems && totalwidth < size.width; i++) {
                    sizearray[i].width++;
                    totalwidth++;
                }
            }
        }
        EGint x = 0;
        for (EGint i = 0; i < numitems; i++) {
            sizearray[i].x = x;
            sizearray[i].y = 0;
            x += sizearray[i].width;
        }
    }

    // set cell size and position on children
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        EWWidgetPtr child = *wi;
        EWContainerDataMap::iterator di = datamap.find(child);
        if (di != datamap.end()) {
            EWContainerDataEntry entry = *di;
            EWBoxData *boxData = static_cast<EWBoxData*>(entry.second.get());
            boxData->rect = sizearray[boxData->index];
            child->setSize(boxData->rect.size());
            child->setPosition(EGPoint(rect.x + boxData->rect.x, rect.y + boxData->rect.y));
        }
    }
}

void EWBox::layout()
{
    if (!needsLayout) return;
    setSize(rect.size());
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        (*wi)->layout();
    }
    needsLayout = false;
}
