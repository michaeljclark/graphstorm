/*
 *  EWWidget.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGGL.h"
#include "EGMath.h"
#include "EGText.h"
#include "EGImage.h"
#include "EGColor.h"
#include "EGIndexArray.h"
#include "EGVertexArray.h"
#include "EGRenderApi.h"
#include "EGRenderBatch.h"
#include "EGRenderApiES2.h"
#include "EGRenderBatchES2.h"

#include "EWWidget.h"
#include "EWContainer.h"
#include "EWWindow.h"
#include "EWContext.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWWidgetRendererES2.h"

/* EWWidget */

static char class_name[] = "EWWidget";

EGint EWWidget::nextWidgetId = 0;

EWWidgetDefaultsPtr EWWidget::widgetDefaults;

EWWidget::EWWidget(EGenum widgetFlags) :
    widgetId(nextWidgetId++),
    widgetFlags(widgetFlags),
    name(),
    parent(NULL),
    minimumSize(0, 0),
    maximumSize(65536, 65536),
    preferredSize(16, 16),
    zOrder(0),
    fillColor(0, 0, 0, 1),
    strokeColor(1, 1, 1, 1),
    textColor(0, 0, 0, 1),
    activeFillColor(0, 0, 0, 1),
    activeStrokeColor(1, 1, 1, 1),
    activeTextColor(0, 0, 0, 1),
    margin(2, 2, 2, 2),
    padding(2, 2, 2, 2),
    borderWidth(0),
    needsLayout(true),
    size(0, 0),
    rect(0, 0, 0, 0)
{
    if (!widgetDefaults) {
        widgetDefaults = EWWidgetDefaultsPtr(new EWWidgetDefaults());
    }
    setEnabled(true);
}

EWWidget::~EWWidget() {}

const EGClass* EWWidget::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWWidget>()
        ->property("visible",           &EWWidget::isVisible,               &EWWidget::setVisible)
        ->property("enabled",           &EWWidget::isEnabled,               &EWWidget::setEnabled)
        ->property("canFocus",          &EWWidget::canFocus,                &EWWidget::setCanFocus)
        ->property("visibleFocus",      &EWWidget::visibleFocus,            &EWWidget::setVisibleFocus)
        ->property("name",              &EWWidget::getName,                 &EWWidget::setName)
        ->property("fillColor",         &EWWidget::getFillColor,            &EWWidget::setFillColor)
        ->property("strokeColor",       &EWWidget::getStrokeColor,          &EWWidget::setStrokeColor)
        ->property("textColor",         &EWWidget::getTextColor,            &EWWidget::setTextColor)
        ->property("selectColor",       &EWWidget::getSelectColor,          &EWWidget::setSelectColor)
        ->property("activeFillColor",   &EWWidget::getActiveFillColor,      &EWWidget::setActiveFillColor)
        ->property("activeStrokeColor", &EWWidget::getActiveStrokeColor,    &EWWidget::setActiveStrokeColor)
        ->property("activeTextColor",   &EWWidget::getActiveTextColor,      &EWWidget::setActiveTextColor)
        ->property("focusBorderColor",  &EWWidget::getFocusBorderColor,     &EWWidget::setFocusBorderColor)
        ->property("fillBackground",    &EWWidget::isFillBackground,        &EWWidget::setFillBackground)
        ->property("strokeBorder",      &EWWidget::isStrokeBorder,          &EWWidget::setStrokeBorder)
        ->property("preferredSize",     &EWWidget::getPreferredSize,        &EWWidget::setPreferredSize)
        ->property("margin",            &EWWidget::getMargin,               &EWWidget::setMargin)
        ->property("padding",           &EWWidget::getPadding,              &EWWidget::setPadding);
    return clazz;
}

const EGClass* EWWidget::getClass() const { return classFactory(); }

void EWWidget::setDefaults(const char *class_name)
{
    setCanFocus(widgetDefaults->getBoolean(class_name, "canFocus", false));
    setVisibleFocus(widgetDefaults->getBoolean(class_name, "visibleFocus", false));
    setFillColor(widgetDefaults->getColor(class_name, "fillColor", EGColor(0.85f,0.85f,0.85f,1)));
    setStrokeColor(widgetDefaults->getColor(class_name, "strokeColor", EGColor(0.65f,0.65f,0.65f,1)));
    setTextColor(widgetDefaults->getColor(class_name, "textColor", EGColor(0.1f,0.1f,0.1f,1)));
    setSelectColor(widgetDefaults->getColor(class_name, "selectColor", EGColor(0.7f,0.7f,0.9f,1)));
    setActiveFillColor(widgetDefaults->getColor(class_name, "activeFillColor", EGColor(0.9f,0.9f,0.9f,1)));
    setActiveStrokeColor(widgetDefaults->getColor(class_name, "activeStrokeColor", EGColor(0.7f,0.7f,0.7f,1)));
    setActiveTextColor(widgetDefaults->getColor(class_name, "activeTextColor", EGColor(0.05f,0.05f,0.05f,1)));
    setFocusBorderColor(widgetDefaults->getColor(class_name, "focusBorderColor", EGColor(0.7f,0.7f,1,1)));
    setBorderWidth(widgetDefaults->getInteger(class_name, "borderWidth", 1));
    setFillBackground(widgetDefaults->getBoolean(class_name, "fillBackground", false));
    setStrokeBorder(widgetDefaults->getBoolean(class_name, "strokeBorder", false));
    setMargin(widgetDefaults->getMargin(class_name, "margin", EGMargin(2,0,2,2)));
    setPadding(widgetDefaults->getMargin(class_name, "padding", EGPadding(4,0,4,4)));
}

const char* EWWidget::widgetTypeName() { return class_name; }

void EWWidget::setDefaultWidgetName()
{
    std::stringstream ss;
    ss << widgetTypeName() << "-" << widgetId;
    name = ss.str();
}

EGenum EWWidget::getFlags() { return widgetFlags; }
EGstring EWWidget::getName() { return name; }
EWWidget* EWWidget::getParent() { return parent; }
EGbool EWWidget::isVisible() { return widgetFlags & EWWidgetFlagsVisible ? true : false; }
EGbool EWWidget::isEnabled() { return widgetFlags & EWWidgetFlagsEnabled ? true : false; }
EGbool EWWidget::canFocus() { return widgetFlags & EWWidgetFlagsCanFocus ? true : false; }
EGbool EWWidget::visibleFocus() { return widgetFlags & EWWidgetFlagsVisibleFocus ? true : false; }
EGbool EWWidget::hasFocus() { return widgetFlags & EWWidgetFlagsHasFocus ? true : false; }
EGbool EWWidget::isFillBackground() { return widgetFlags & EWWidgetFlagsFillBackground ? true : false; }
EGbool EWWidget::isStrokeBorder() { return widgetFlags & EWWidgetFlagsStrokeBorder ? true : false; }
EGColor EWWidget::getFillColor() { return fillColor; }
EGColor EWWidget::getStrokeColor() { return strokeColor; }
EGColor EWWidget::getTextColor() { return textColor; }
EGColor EWWidget::getSelectColor() { return selectColor; }
EGColor EWWidget::getActiveFillColor() { return activeFillColor; }
EGColor EWWidget::getActiveStrokeColor() { return activeStrokeColor; }
EGColor EWWidget::getActiveTextColor() { return activeTextColor; }
EGColor EWWidget::getFocusBorderColor() { return focusBorderColor; }
EGMargin EWWidget::getMargin() { return margin; }
EGPadding EWWidget::getPadding() { return padding; }
EGint EWWidget::getBorderWidth() { return borderWidth; }
EGSize EWWidget::getMinimumSize() { return minimumSize; }
EGSize EWWidget::getMaximumSize() { return maximumSize; }
EGSize EWWidget::getPreferredSize() { return preferredSize; }
EGint EWWidget::getZOrder() { return zOrder; }

void EWWidget::setFlags(EGenum flags)
{
    widgetFlags = flags;
    setNeedsLayout();
}

void EWWidget::setName(EGstring name)
{
    this->name = name;
}

void EWWidget::setParent(EWWidget *parent)
{
    this->parent = parent;
}

void EWWidget::setVisible(EGbool visible)
{
    if (isVisible() && !visible) {
        widgetFlags &= ~EWWidgetFlagsVisible;
        setNeedsLayout();
    } else if (!isVisible() && visible) {
        widgetFlags |= EWWidgetFlagsVisible;
        setNeedsLayout();
    }
}

void EWWidget::setEnabled(EGbool enabled)
{
    if (isEnabled() && !enabled) {
        widgetFlags &= ~EWWidgetFlagsEnabled;
        setNeedsLayout();
    } else if (!isEnabled() && enabled) {
        widgetFlags |= EWWidgetFlagsEnabled;
        setNeedsLayout();
    }
}

void EWWidget::setCanFocus(EGbool canfocus)
{
    if (!canfocus) {
        widgetFlags &= ~EWWidgetFlagsCanFocus;
    } else {
        widgetFlags |= EWWidgetFlagsCanFocus;
    }
}

void EWWidget::setVisibleFocus(EGbool visiblefocus)
{
    if (!visiblefocus) {
        widgetFlags &= ~EWWidgetFlagsVisibleFocus;
    } else {
        widgetFlags |= EWWidgetFlagsVisibleFocus;
    }
}

void EWWidget::setFillBackground(EGbool fillBackground)
{
    if (isFillBackground() && !fillBackground) {
        widgetFlags &= ~EWWidgetFlagsFillBackground;
        setNeedsLayout();
    } else if (!isFillBackground() && fillBackground) {
        widgetFlags |= EWWidgetFlagsFillBackground;
        setNeedsLayout();
    }
}

void EWWidget::setStrokeBorder(EGbool strokeBorder)
{
    if (isStrokeBorder() && !strokeBorder) {
        widgetFlags &= ~EWWidgetFlagsStrokeBorder;
        setNeedsLayout();
    } else if (!isStrokeBorder() && strokeBorder) {
        widgetFlags |= EWWidgetFlagsStrokeBorder;
        setNeedsLayout();
    }
}

void EWWidget::setFillColor(EGColor fillColor)
{
    if (this->fillColor != fillColor) {
        this->fillColor = fillColor;
        setNeedsLayout();
    }
}

void EWWidget::setStrokeColor(EGColor strokeColor)
{
    if (this->strokeColor != strokeColor) {
        this->strokeColor = strokeColor;
        setNeedsLayout();
    }
}

void EWWidget::setTextColor(EGColor textColor)
{
    if (this->textColor != textColor) {
        this->textColor = textColor;
        setNeedsLayout();
    }
}

void EWWidget::setSelectColor(EGColor selectColor)
{
    if (this->selectColor != selectColor) {
        this->selectColor = selectColor;
        setNeedsLayout();
    }
}

void EWWidget::setActiveFillColor(EGColor activeFillColor)
{
    if (this->activeFillColor != activeFillColor) {
        this->activeFillColor = activeFillColor;
        setNeedsLayout();
    }
}

void EWWidget::setActiveStrokeColor(EGColor activeStrokeColor)
{
    if (this->activeStrokeColor != activeStrokeColor) {
        this->activeStrokeColor = activeStrokeColor;
        setNeedsLayout();
    }
}

void EWWidget::setActiveTextColor(EGColor activeTextColor)
{
    if (this->activeTextColor != activeTextColor) {
        this->activeTextColor = activeTextColor;
        setNeedsLayout();
    }
}

void EWWidget::setFocusBorderColor(EGColor focusBorderColor)
{
    if (this->focusBorderColor != focusBorderColor) {
        this->focusBorderColor = focusBorderColor;
        setNeedsLayout();
    }
}

void EWWidget::setMargin(EGMargin margin)
{
    if (this->margin != margin) {
        this->margin = margin;
        setNeedsLayout();
    }
}

void EWWidget::setPadding(EGPadding padding)
{
    if (this->padding != padding) {
        this->padding = padding;
        setNeedsLayout();
    }
}

void EWWidget::setBorderWidth(EGint borderWidth)
{
    if (this->borderWidth != borderWidth) {
        this->borderWidth = borderWidth;
        setNeedsLayout();
    }
}

void EWWidget::setMinimumSize(EGSize minimumSize)
{
    if (this->minimumSize != minimumSize) {
        this->minimumSize = minimumSize;
        setNeedsLayout();
    }
}

void EWWidget::setMaximumSize(EGSize maximumSize)
{
    if (this->maximumSize != maximumSize) {
        this->maximumSize = maximumSize;
        setNeedsLayout();
    }
}

void EWWidget::setPreferredSize(EGSize preferredSize)
{
    if (this->preferredSize != preferredSize) {
        this->preferredSize = preferredSize;
        setNeedsLayout();
    }
}

void EWWidget::setZOrder(EGint zOrder)
{
    if (this->zOrder != zOrder) {
        this->zOrder = zOrder;
        EWContext *context = getContext();
        if (context) {
            context->sortTopLevelWidgets();
            setNeedsLayout();
        }
    }
}

EWWidgetList* EWWidget::getChildren()
{
    return NULL;
}

EWContext* EWWidget::getContext()
{
    EWWidget* p = this;
    while (p != NULL) {
        if (typeid(*p) == typeid(EWContext)) {
            return static_cast<EWContext*>(p);
        }
        p = p->getParent();
    }
    return NULL;
}

EWWindow* EWWidget::getWindow()
{
    EWWidget* p = this;
    while (p != NULL) {
        if (typeid(*p) == typeid(EWWindow)) {
            return static_cast<EWWindow*>(p);
        }
        p = p->getParent();
    }
    return NULL;
}

EGbool EWWidget::getNeedsLayout()
{
    return needsLayout;
}

void EWWidget::setNeedsLayout(EGbool propagateUpwards)
{
    if (needsLayout) {
        return;
    }
    needsLayout = true;
    if (propagateUpwards) {
        EWWidget* p = getParent();
        EWWindow* w = getWindow();
        while (p != NULL) {
            if (p == w) break;
            p->needsLayout = true;
            p = p->getParent();
        }
    }
    EWWidgetList *children = getChildren();
    if (children) {
        for (EWWidgetList::iterator wi = children->begin(); wi != children->end(); wi++) {
            (*wi)->setNeedsLayout(false);
        }
    }
}

EGSize EWWidget::calcMinimumSize()
{
    return minimumSize;
}

EGSize EWWidget::calcPreferredSize()
{
    EGSize minimumSize = calcMinimumSize();
    return EGSize((std::max)(preferredSize.width, minimumSize.width),
                  (std::max)(preferredSize.height, minimumSize.height));
}

void EWWidget::setSize(EGSize size)
{
    rect = EGRect(rect.x, rect.y, size.width, size.height);
}

void EWWidget::setPosition(EGPoint position)
{
    if (getPosition() != position) {
        rect.x = position.x;
        rect.y = position.y;
        setNeedsLayout();
    }
}

EGRect EWWidget::getRect()
{
    return rect;
}

EGSize EWWidget::getSize()
{
    return rect.size();
}

EGPoint EWWidget::getPosition()
{
    return EGPoint(rect.x, rect.y);
}

void EWWidget::show()
{
    setVisible(true);
}

void EWWidget::showAll()
{
    setVisible(true);
    EWWidgetList *children = getChildren();
    if (children) {
        for (EWWidgetList::iterator wi = children->begin(); wi != children->end(); wi++) {
            (*wi)->showAll();
        }
    }
}

void EWWidget::hide()
{
    setVisible(false);
}

EWWidget* EWWidget::nextWidget(EWWidget *current)
{
    EWWidget *parent = current->getParent();
    if (!parent) {
        return NULL;
    }
    EWWidgetList *children = parent->getChildren();
    
    while (children != NULL) {
        EWWidget *widget = NULL, *lastwidget = NULL;
        for (EWWidgetList::iterator wi = children->begin(); wi != children->end(); wi++) {
            widget = (*wi).get();
            if (lastwidget == current || current == NULL) {
                while ((children = widget->getChildren())) {
                    if (children->size() > 0) {
                        widget = children->at(0).get();
                        if (!widget->getChildren()) {
                            return widget;
                        }
                    } else {
                        widget = NULL;
                        break;
                    }
                }
                if (widget) {
                    return widget;
                }
            }
            lastwidget = widget;
        }        
        current = parent;
        parent = current->getParent();
        if (!parent) {
            parent = getWindow();
            current = NULL;
        }
        children = parent->getChildren();
    }
    
    return NULL;
}

EWWidget* EWWidget::prevWidget(EWWidget *current)
{
    EWWidget *parent = current->getParent();
    if (!parent) {
        return NULL;
    }
    EWWidgetList *children = parent->getChildren();
    
    while (children != NULL) {
        EWWidget *widget = NULL, *lastwidget = NULL;
        for (EWWidgetList::reverse_iterator wi = children->rbegin(); wi != children->rend(); wi++) {
            widget = (*wi).get();
            if (lastwidget == current || current == NULL) {
                while ((children = widget->getChildren())) {
                    if (children->size() > 0) {
                        widget = children->back().get();
                        if (!widget->getChildren()) {
                            return widget;
                        }
                    } else {
                        widget = NULL;
                        break;
                    }
                }
                if (widget) {
                    return widget;
                }
            }
            lastwidget = widget;
        }
        current = parent;
        parent = current->getParent();
        if (!parent) {
            parent = getWindow();
            current = NULL;
        }
        children = parent->getChildren();
    }
    
    return NULL;
}

void EWWidget::nextFocus()
{
    EWWidget *w = this;
    while ((w = nextWidget(w))) {
        if (w->canFocus() && w->isEnabled()) {
            w->requestFocus();
            return;
        }
    }
}

void EWWidget::prevFocus()
{
    EWWidget *w = this;
    while ((w = prevWidget(w))) {
        if (w->canFocus() && w->isEnabled()) {
            w->requestFocus();
            return;
        }
    }
}

void EWWidget::releaseFocus()
{
    EWWidget* p = this;
    while (p != NULL) {
        if (p->canFocus()) {
            p->widgetFlags &= ~EWWidgetFlagsHasFocus;
            p->needsLayout = true;
        }
        p = p->getParent();
    }
}

void EWWidget::requestFocus()
{
    if (isEnabled() && canFocus() && !hasFocus()) {
        EWWindow *window = getWindow();
        EWContext *context = getContext();
        if (context) {
            if (context->focusWidget) {
                context->focusWidget->releaseFocus();
            }
            context->focusWidget = shared_from_this();
            EWWidget *p = this;
            while (p != NULL) {
                if (p->canFocus()) {
                    p->widgetFlags |= EWWidgetFlagsHasFocus;
                    p->needsLayout = true;
                }
                p = p->getParent();
            }
        }
        if (window) {
            window->raiseWindow(false);
        }
        setNeedsLayout();
    }
}

EGPoint EWWidget::pointFromView(EGPoint point)
{
    //EGPoint origpoint = point;
    EWWidget *w = this;
    std::list<EGfloat*> matrixList;
    while (w != NULL) {
        EGfloat *matrix = w->getTransform();
        if (matrix) matrixList.push_back(matrix);
        w = w->getParent();
    }
    for (std::list<EGfloat*>::reverse_iterator mi = matrixList.rbegin(); mi != matrixList.rend(); mi++) {
        EGfloat *matrix = *mi;
        EGfloat v[4] = { (EGfloat)point.x, (EGfloat)point.y, 0, 1 };
        EGfloat r[4];
        EGMath::multMat4fVec4f(r, v, matrix);
        point = EGPoint(r[0], r[1]);
    }
    //EGDebug("%s %s %d,%d -> %d,%d\n", __func__, name.c_str(), origpoint.x, origpoint.y, point.x, point.y);
    return point;
}

EGPoint EWWidget::pointToView(EGPoint point)
{
    //EGPoint origpoint = point;
    EWWidget *w = this;
    std::list<EGfloat*> matrixList;
    while (w != NULL) {
        EGfloat *matrix = w->getInverseTransform();
        if (matrix) matrixList.push_back(matrix);
        w = w->getParent();
    }
    for (std::list<EGfloat*>::reverse_iterator mi = matrixList.rbegin(); mi != matrixList.rend(); mi++) {
        EGfloat *matrix = *mi;
        EGfloat v[4] = { (EGfloat)point.x, (EGfloat)point.y, 0, 1 };
        EGfloat r[4];
        EGMath::multMat4fVec4f(r, v, matrix);
        point = EGPoint(r[0], r[1]);
    }
    //EGDebug("%s %s %d,%d -> %d,%d\n", __func__, name.c_str(), origpoint.x, origpoint.y, point.x, point.y);
    return point;
}

EGfloat* EWWidget::getTransform()
{
    return NULL;
}

EGfloat* EWWidget::getInverseTransform()
{
    return NULL;
}

void EWWidget::layout()
{
    needsLayout = false;
}

void EWWidget::draw()
{
    if (!isVisible()) {
        return;
    }
    if (!renderer || needsLayout) {
        layout();
    }
    
    EGfloat *modelView = NULL;
    EGfloat *transform = getTransform();
    EGRenderApi *gl = &EGRenderApi::currentApiImpl();

    if (transform) {
        modelView = new EGfloat[16];
        gl->getMatrix(EGRenderMatrixModelView, modelView);
        gl->multMatrix(EGRenderMatrixModelView, transform);
        gl->updateUniforms();
    }
    
    if (renderer) {
        renderer->draw();
    }
    
    subdraw();
    
    EWWidgetList *children = getChildren();
    if (children) {
        for (EWWidgetList::iterator wi = children->begin(); wi != children->end(); wi++) {
            if ((*wi)->isVisible()) {
                (*wi)->draw();
            }
        }
    }
    
    if (transform) {
        gl->loadMatrix(EGRenderMatrixModelView, modelView);
        gl->updateUniforms();
    }
}

void EWWidget::subdraw() {}

EGbool EWWidget::mouseEvent(EGMouseEvent *evt)
{
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON) {
        EWWindow *window = getWindow();
        if (window) {
            window->raiseWindow(false);
        }
        return true;
    }
    return false;
}

EGbool EWWidget::keyEvent(EGKeyEvent *evt)
{
    return false;
}

EWWidgetRendererPtr EWWidget::createRenderer(EWWidget *widget)
{
    return EWWidgetRendererPtr(new EWWidgetRendererES2(widget));
}
