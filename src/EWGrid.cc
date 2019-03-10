// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWContainer.h"
#include "EWGrid.h"


/* EWGrid */

static char class_name[] = "EWGrid";

EWGrid::EWGrid(EGenum widgetFlags)
    : EWContainer(widgetFlags), numrows(0), numcols(0), sizearray(NULL), colwidths(NULL), rowheights(NULL), maxcolwidth(0), maxrowheight(0), minsize()
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWGrid::~EWGrid()
{
    clearSizeArrays();
}

const EGClass* EWGrid::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWGrid>()->extends<EWContainer>();
    return clazz;
}

const EGClass* EWGrid::getClass() const { return classFactory(); }

void EWGrid::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
    setRowsHomogeneous(widgetDefaults->getBoolean(class_name, "rowsHomogeneous", false));
    setColumnsHomogeneous(widgetDefaults->getBoolean(class_name, "columnsHomogeneous", false));
    setRowsExpand(widgetDefaults->getBoolean(class_name, "rowsExpand", false));
    setColumnsHomogeneous(widgetDefaults->getBoolean(class_name, "columnsExpand", false));
}

void EWGrid::clearSizeArrays()
{
    if (sizearray) {
        for (EGint i = 0; i < numrows; i++) {
            delete [] sizearray[i];
        }
        delete [] sizearray;
        delete [] colwidths;
        delete [] rowheights;
    }
}

const char* EWGrid::widgetTypeName() { return class_name; }

void EWGrid::addWidget(EWWidgetPtr widget, EWGridData *gridData)
{
    if (widget) {
        widget->setParent(this);
        children.push_back(widget);
        datamap.insert(EWContainerDataEntry(widget, EWContainerDataPtr(gridData)));
    }
}

void EWGrid::addWidget(EWWidgetPtr widget, EGint left, EGint top, EGint width, EGint height)
{
    addWidget(widget, new EWGridData(left, top, width, height));
}

void EWGrid::removeWidget(EWWidgetPtr widget)
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

EGbool EWGrid::isColumnsHomogeneous() { return widgetFlags & EWWidgetFlagsLayoutHomogeneousHorizontal ? true : false; }
EGbool EWGrid::isRowsHomogeneous() { return widgetFlags & EWWidgetFlagsLayoutHomogeneousVertical ? true : false; }
EGbool EWGrid::isColumnsExpand() { return widgetFlags & EWWidgetFlagsLayoutExpandHorizontal ? true : false; }
EGbool EWGrid::isRowsExpand() { return widgetFlags & EWWidgetFlagsLayoutExpandVertical ? true : false; }

void EWGrid::setColumnsHomogeneous(EGbool homogeneous)
{
    if (isColumnsHomogeneous() && !homogeneous) {
        widgetFlags &= ~EWWidgetFlagsLayoutHomogeneousHorizontal;
        setNeedsLayout();
    } else if (!isColumnsHomogeneous() && homogeneous) {
        widgetFlags |= EWWidgetFlagsLayoutHomogeneousHorizontal;
        setNeedsLayout();
    }
}

void EWGrid::setRowsHomogeneous(EGbool homogeneous)
{
    if (isRowsHomogeneous() && !homogeneous) {
        widgetFlags &= ~EWWidgetFlagsLayoutHomogeneousVertical;
        setNeedsLayout();
    } else if (!isRowsHomogeneous() && homogeneous) {
        widgetFlags |= EWWidgetFlagsLayoutHomogeneousVertical;
        setNeedsLayout();
    }
}

void EWGrid::setColumnsExpand(EGbool expand)
{
    if (isColumnsExpand() && !expand) {
        widgetFlags &= ~EWWidgetFlagsLayoutExpandHorizontal;
        setNeedsLayout();
    } else if (!isColumnsExpand() && expand) {
        widgetFlags |= EWWidgetFlagsLayoutExpandHorizontal;
        setNeedsLayout();
    }
}

void EWGrid::setRowsExpand(EGbool expand)
{
    if (isRowsExpand() && !expand) {
        widgetFlags &= ~EWWidgetFlagsLayoutExpandVertical;
        setNeedsLayout();
    } else if (!isRowsExpand() && expand) {
        widgetFlags |= EWWidgetFlagsLayoutExpandVertical;
        setNeedsLayout();
    }
}

EGSize EWGrid::calcMinimumSize()
{
    // get child minimum sizes
    EGint numrows = 0, numcols = 0;
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        EWWidgetPtr child = *wi;
        EWContainerDataMap::iterator di = datamap.find(child);
        if (di != datamap.end()) {
            EWContainerDataEntry entry = *di;
            EWGridData *gridData = static_cast<EWGridData*>(entry.second.get());
            gridData->minSize = child->calcMinimumSize();
            gridData->prefSize = child->calcPreferredSize();
            numcols = (std::max)(numcols, gridData->left + gridData->width);
            numrows = (std::max)(numrows, gridData->top + gridData->height);
        }
    }
    
    // populate temporary size array
    if (!sizearray || numrows != this->numrows || numcols != this->numcols) {
        clearSizeArrays();
        sizearray = new EGRect*[numrows];
        for (EGint i = 0; i < numrows; i++) {
            sizearray[i] = new EGRect[numcols];
        }
        colwidths = new EGint[numcols];
        rowheights = new EGint[numrows];
        this->numrows = numrows;
        this->numcols = numcols;
    }
    
    // clear size arrays
    for (EGint i = 0; i < numcols; i++) {
        for (EGint j = 0; j < numrows; j++) {
            sizearray[j][i] = EGRect();
        }
    }
    for (EGint i = 0; i < numrows; i++) {
        rowheights[i] = 0;
    }
    for (EGint i = 0; i < numcols; i++) {
        colwidths[i] = 0;
    }

    // fill size array
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        EWWidgetPtr child = *wi;
        EWContainerDataMap::iterator di = datamap.find(child);
        if (di != datamap.end()) {
            EWContainerDataEntry entry = *di;
            EWGridData *gridData = static_cast<EWGridData*>(entry.second.get());
            for (EGint i = 0; i < gridData->width; i++) {
                for (EGint j = 0; j < gridData->height; j++) {
                    sizearray[gridData->top + j][gridData->left + i].width = gridData->prefSize.width / gridData->width;
                    sizearray[gridData->top + j][gridData->left + i].height = gridData->prefSize.height / gridData->height;
                }
            }
        }
    }
    
    // calculate max width for each column
    maxcolwidth = 0;
    for (EGint i = 0; i < numcols; i++) {
        for (EGint j = 0; j < numrows; j++) {
            colwidths[i] = (std::max)(colwidths[i], sizearray[j][i].width);
        }
        maxcolwidth = (std::max)(colwidths[i], maxcolwidth);
    }
    
    // calculate max height for each for row
    maxrowheight = 0;
    for (EGint j = 0; j < numrows; j++) {
        for (EGint i = 0; i < numcols; i++) {
            rowheights[j] = (std::max)(rowheights[j], sizearray[j][i].height);
        }
        maxrowheight = (std::max)(rowheights[j], maxrowheight);
    }
    
    // calculate minimum overall size
    minsize = EGSize();
    if (isColumnsHomogeneous()) {
        minsize.width = maxcolwidth * numcols;
    } else {
        for (EGint i = 0; i < numcols; i++) {
            minsize.width += colwidths[i];
        }
    }
    if (isRowsHomogeneous()) {
        minsize.height = maxrowheight * numrows;
    } else {
        for (EGint j = 0; j < numrows; j++) {
            minsize.height += rowheights[j];
        }
    }

    return minsize;
}

void EWGrid::setSize(EGSize size)
{
    rect = EGRect(rect.x, rect.y, size.width, size.height);
    
    calcMinimumSize();

    EGfloat hratio = isColumnsExpand() && minsize.width > 0 ? (EGfloat)size.width / (EGfloat)minsize.width : 1.0f;
    EGfloat vratio = isRowsExpand() && minsize.height > 0 ? (EGfloat)size.height / (EGfloat)minsize.height : 1.0f;

    // expand cell sizes horizontally
    for (EGint j = 0; j < numrows; j++) {
        for (EGint i = 0; i < numcols; i++) {
            sizearray[j][i].width = (EGint)((isColumnsHomogeneous() ? maxcolwidth : colwidths[i]) * hratio);
        }
    }
    if (isColumnsExpand()) {
        for (EGint j = 0; j < numrows; j++) {
            EGint totalwidth = 0;
            for (EGint i = 0; i < numcols; i++) {
                totalwidth += sizearray[j][i].width;
            }
            while (totalwidth < rect.width) {
                for (EGint i = 0; i < numcols && totalwidth < rect.width; i++) {
                    sizearray[j][i].width++;
                    totalwidth++;
                }
            }
        }
    }
    for (EGint j = 0; j < numrows; j++) {
        for (EGint i = 0; i < numcols; i++) {
            sizearray[j][i].height = (EGint)((isRowsHomogeneous() ? maxrowheight : rowheights[j]) * vratio);
        }
    }
    if (isRowsExpand()) {
        // expand cell sizes vertically
        for (EGint i = 0; i < numcols; i++) {
            EGint totalheight = 0;
            for (EGint j = 0; j < numrows; j++) {
                totalheight += sizearray[j][i].height;
            }
            while (totalheight < rect.height) {
                for (EGint j = 0; j < numrows && totalheight < rect.height; j++) {
                    sizearray[j][i].height++;
                    totalheight++;
                }
            }
        }
    }
    
    // set x and y coordinates
    EGint y = 0;
    for (EGint j = 0; j < numrows; j++) {
        EGint x = 0;
        for (EGint i = 0; i < numcols; i++) {
            sizearray[j][i].x = x;
            sizearray[j][i].y = y;
            x += sizearray[j][i].width;
        }
        y += sizearray[j][0].height;
    }

    // set cell size and positions on children
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        EWWidgetPtr child = *wi;
        EWContainerDataMap::iterator di = datamap.find(child);
        if (di != datamap.end()) {
            EWContainerDataEntry entry = *di;
            EWGridData *gridData = static_cast<EWGridData*>(entry.second.get());
            EGSize childSize;
            for (EGint i = 0; i < gridData->width; i++) {
                for (EGint j = 0; j < gridData->height; j++) {
                    childSize.width += sizearray[gridData->top + j][gridData->left + i].width;
                    childSize.height += sizearray[gridData->top + j][gridData->left + i].height;
                }
            }
            EGPoint childPosition(rect.x + sizearray[gridData->top][gridData->left].x, rect.y + sizearray[gridData->top][gridData->left].y);
            child->setSize(childSize);
            child->setPosition(childPosition);
        }
    }
}

void EWGrid::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        (*wi)->layout();
    }
    
    needsLayout = false;
}
