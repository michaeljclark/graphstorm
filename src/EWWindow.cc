/*
 *  EWWindow.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGMath.h"
#include "EGTime.h"

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWContainer.h"
#include "EWContext.h"
#include "EWScrollBar.h"
#include "EWWindow.h"


/* EWWindowEvent */

EGEventQualifier EWWindowEvent::CLOSED   = "window-closed";

EWWindowEvent::EWWindowEvent(EGEventQualifier q, EWWindow *window)
    : EGEvent(q), window(window) {}

std::string EWWindowEvent::toString() {
    std::stringstream s;
    s << "[EWWindowEvent " << q << " window=" << window << "]";
    return s.str();
};


/* EWWindow */

static char class_name[] = "EWWindow";

EWWindow::EWWindow(EGenum widgetFlags) :
    EWContainer(widgetFlags)
{
    setDefaultWidgetName();
    setDefaults(class_name);
    
    EGMath::identityMat4f(matrix);
    EGMath::identityMat4f(invmatrix);
}

EWWindow::~EWWindow() {}

const EGClass* EWWindow::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWWindow>()->extends<EWContainer>()
        ->property("titlePadding",              &EWWindow::getTitlePadding,                 &EWWindow::setTitlePadding)
        ->property("decorationFillColor",       &EWWindow::getDecorationFillColor,          &EWWindow::setDecorationFillColor)
        ->property("decorationBorderWidth",     &EWWindow::getDecorationBorderWidth,        &EWWindow::setDecorationBorderWidth)
        ->property("decorationStrokeColor",     &EWWindow::getDecorationStrokeColor,        &EWWindow::setDecorationStrokeColor)
        ->property("decorationMargin",          &EWWindow::getDecorationMargin,             &EWWindow::setDecorationMargin)
        ->property("activeDecorationFillColor", &EWWindow::getActiveDecorationFillColor,    &EWWindow::setActiveDecorationFillColor)
        ->property("closeBoxColor",             &EWWindow::getCloseBoxColor,                &EWWindow::setCloseBoxColor)
        ->property("hasTitle",                  &EWWindow::hasTitle,                        &EWWindow::setHasTitle)
        ->property("hasCloseBox",               &EWWindow::hasCloseBox,                     &EWWindow::setHasCloseBox)
        ->property("hasDecoration",             &EWWindow::hasDecoration,                   &EWWindow::setHasDecoration)
        ->property("movable",                   &EWWindow::isMovable,                       &EWWindow::setMovable)
        ->property("resizable",                 &EWWindow::isResizable,                     &EWWindow::setResizable)
        ->property("isModal",                   &EWWindow::isModal,                         &EWWindow::setModal)
        ->property("hasVScrollBar",             &EWWindow::hasVScrollBar,                   &EWWindow::setHasVScrollBar)
        ->property("hasHScrollBar",             &EWWindow::hasHScrollBar,                   &EWWindow::setHasHScrollBar);
    return clazz;
}

const EGClass* EWWindow::getClass() const { return classFactory(); }

void EWWindow::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
    setTitlePadding(widgetDefaults->getMargin(class_name, "titlePadding", EGPadding()));
    setDecorationFillColor(widgetDefaults->getColor(class_name, "decorationFillColor", EGColor(0.8f, 0.8f, 0.8f,1)));
    setDecorationBorderWidth(widgetDefaults->getInteger(class_name, "decorationBorderWidth", 1));
    setDecorationStrokeColor(widgetDefaults->getColor(class_name, "decorationStrokeColor", EGColor(1,1,1,1)));
    setDecorationMargin(widgetDefaults->getMargin(class_name, "decorationMargin", EGMargin(5,5,5,5)));
    setActiveDecorationFillColor(widgetDefaults->getColor(class_name, "activeDecorationFillColor", EGColor(0.85f, 0.85f, 0.85f,1)));
    setCloseBoxColor(widgetDefaults->getColor(class_name, "closeBoxColor", EGColor(0.25f,0.25f,0.25f,1)));
    setHasTitle(widgetDefaults->getBoolean(class_name, "hasTitle", true));
    setHasCloseBox(widgetDefaults->getBoolean(class_name, "hasCloseBox", true));
    setHasDecoration(widgetDefaults->getBoolean(class_name, "hasDecoration", true));
    setMovable(widgetDefaults->getBoolean(class_name, "isMovable", false));
    setResizable(widgetDefaults->getBoolean(class_name, "isResizable", false));
    setModal(widgetDefaults->getBoolean(class_name, "isModal", false));
    setHasVScrollBar(widgetDefaults->getBoolean(class_name, "hasVScrollBar", false));
    setHasHScrollBar(widgetDefaults->getBoolean(class_name, "hasHScrollBar", false));
    closeBoxImage = widgetDefaults->getImage(class_name, "closeBoxImage", EGImagePtr());
}

const char* EWWindow::widgetTypeName() { return class_name; }

void EWWindow::addWidget(EWWidgetPtr widget)
{
    if (children.size() == 0) {
        children.push_back(widget);
        widget->setParent(this);
    } else if (children.size() == 1) {
        children[0]->setParent(NULL);
        if (widget) {
            children[0] = widget;
            widget->setParent(this);
        } else {
            children.clear();
        }
    }
}

void EWWindow::setTitle(EGstring title)
{
    if (this->title != title) {
        if (!titleText) {
            titleText = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignCenter);
        }
        titleText->setText(title);
        setNeedsLayout();
    }
}

void EWWindow::setTitleText(EGTextPtr titleText)
{
    if (this->titleText != titleText) {
        this->titleText = titleText;
        setNeedsLayout();
    }
}

void EWWindow::setTitlePadding(EGPadding titlePadding)
{
    if (this->titlePadding != titlePadding) {
        this->titlePadding = titlePadding;
        setNeedsLayout();
    }
}

void EWWindow::setDecorationMargin(EGMargin decorationMargin)
{
    if (this->decorationMargin != decorationMargin) {
        this->decorationMargin = decorationMargin;
        setNeedsLayout();
    }
}

void EWWindow::setDecorationFillColor(EGColor decorationFillColor)
{
    if (this->decorationFillColor != decorationFillColor) {
        this->decorationFillColor = decorationFillColor;
        setNeedsLayout();
    }
}

void EWWindow::setDecorationStrokeColor(EGColor decorationStrokeColor)
{
    if (this->decorationStrokeColor != decorationStrokeColor) {
        this->decorationStrokeColor = decorationStrokeColor;
        setNeedsLayout();
    }
}

void EWWindow::setDecorationBorderWidth(EGint decorationBorderWidth)
{
    if (this->decorationBorderWidth != decorationBorderWidth) {
        this->decorationBorderWidth = decorationBorderWidth;
        setNeedsLayout();
    }
}

void EWWindow::setActiveDecorationFillColor(EGColor activeDecorationFillColor)
{
    if (this->activeDecorationFillColor != activeDecorationFillColor) {
        this->activeDecorationFillColor = activeDecorationFillColor;
        setNeedsLayout();
    }
}

void EWWindow::setCloseBoxColor(EGColor closeBoxColor)
{
    if (this->closeBoxColor != closeBoxColor) {
        this->closeBoxColor = closeBoxColor;
        setNeedsLayout();
    }
}

void EWWindow::setHasTitle(EGbool title)
{
    if (hasTitle() && !title) {
        widgetFlags &= ~EWWidgetFlagsWindowHasTitle;
        setNeedsLayout();
    } else if (!hasTitle() && title) {
        widgetFlags |= EWWidgetFlagsWindowHasTitle;
        setNeedsLayout();
    }
}

void EWWindow::setHasDecoration(EGbool decoration)
{
    if (hasDecoration() && !decoration) {
        widgetFlags &= ~EWWidgetFlagsWindowHasDecoration;
        setNeedsLayout();
    } else if (!hasDecoration() && decoration) {
        widgetFlags |= EWWidgetFlagsWindowHasDecoration;
        setNeedsLayout();
    }
}

void EWWindow::setHasCloseBox(EGbool closebox)
{
    if (hasCloseBox() && !closebox) {
        widgetFlags &= ~EWWidgetFlagsWindowHasCloseBox;
        setNeedsLayout();
    } else if (!hasCloseBox() && closebox) {
        widgetFlags |= EWWidgetFlagsWindowHasCloseBox;
        setNeedsLayout();
    }
}

void EWWindow::setMovable(EGbool movable)
{
    if (isMovable() && !movable) {
        widgetFlags &= ~EWWidgetFlagsWindowIsMovable;
        setNeedsLayout();
    } else if (!isMovable() && movable) {
        widgetFlags |= EWWidgetFlagsWindowIsMovable;
        setNeedsLayout();
    }
}

void EWWindow::setResizable(EGbool resizable)
{
    if (isResizable() && !resizable) {
        widgetFlags &= ~EWWidgetFlagsWindowIsResizable;
        setNeedsLayout();
    } else if (!isResizable() && resizable) {
        widgetFlags |= EWWidgetFlagsWindowIsResizable;
        setNeedsLayout();
    }
}

void EWWindow::setModal(EGbool modal)
{
    if (isModal() && !modal) {
        widgetFlags &= ~EWWidgetFlagsWindowIsModal;
        setNeedsLayout();
    } else if (!isModal() && modal) {
        widgetFlags |= EWWidgetFlagsWindowIsModal;
        setNeedsLayout();
    }
}

void EWWindow::setHasVScrollBar(EGbool vscrollbar)
{
    if (hasVScrollBar() && !vscrollbar) {
        widgetFlags &= ~EWWidgetFlagsWindowHasVScrollBar;
        this->vscrollbar = EWScrollBarPtr();
        setNeedsLayout();
    } else if (!hasVScrollBar() && vscrollbar) {
        widgetFlags |= EWWidgetFlagsWindowHasVScrollBar;
        this->vscrollbar = EWScrollBarPtr(new EWScrollBar());
        this->vscrollbar->setParent(this);
        this->vscrollbar->setVertical(true);
        this->vscrollbar->show();
        setNeedsLayout();
    }
}

void EWWindow::setHasHScrollBar(EGbool hscrollbar)
{
    if (hasHScrollBar() && !hscrollbar) {
        widgetFlags &= ~EWWidgetFlagsWindowHasHScrollBar;
        this->hscrollbar = EWScrollBarPtr();
        setNeedsLayout();
    } else if (!hasHScrollBar() && hscrollbar) {
        widgetFlags |= EWWidgetFlagsWindowHasHScrollBar;
        this->hscrollbar = EWScrollBarPtr(new EWScrollBar());
        this->hscrollbar->setParent(this);
        this->hscrollbar->setVertical(false);
        this->hscrollbar->show();
        setNeedsLayout();
    }
}

EGstring EWWindow::getTitle() { return title; }
EGTextPtr EWWindow::getTitleText() { return titleText; }
EGPadding EWWindow::getTitlePadding() { return titlePadding; }
EGMargin EWWindow::getDecorationMargin() { return decorationMargin; }
EGColor EWWindow::getDecorationFillColor() { return decorationFillColor; }
EGColor EWWindow::getDecorationStrokeColor() { return decorationFillColor; }
EGint EWWindow::getDecorationBorderWidth() { return decorationBorderWidth; }
EGColor EWWindow::getActiveDecorationFillColor() { return activeDecorationFillColor; }
EGColor EWWindow::getCloseBoxColor() { return closeBoxColor; }
EGbool EWWindow::hasTitle() { return widgetFlags & EWWidgetFlagsWindowHasTitle ? true : false; }
EGbool EWWindow::hasDecoration() { return widgetFlags & EWWidgetFlagsWindowHasDecoration ? true : false; }
EGbool EWWindow::hasCloseBox() { return widgetFlags & EWWidgetFlagsWindowHasCloseBox ? true : false; }
EGbool EWWindow::isMovable() { return widgetFlags & EWWidgetFlagsWindowIsMovable ? true : false; }
EGbool EWWindow::isResizable() { return widgetFlags & EWWidgetFlagsWindowIsResizable ? true : false; }
EGbool EWWindow::isModal() { return widgetFlags & EWWidgetFlagsWindowIsModal ? true : false; }
EGbool EWWindow::hasVScrollBar() { return widgetFlags & EWWidgetFlagsWindowHasVScrollBar ? true : false; }
EGbool EWWindow::hasHScrollBar() { return widgetFlags & EWWidgetFlagsWindowHasHScrollBar ? true : false; }

void EWWindow::setNeedsLayout(EGbool propagateUpwards)
{
    EWWidget::setNeedsLayout(propagateUpwards);
    if (hasVScrollBar()) {
        vscrollbar->setNeedsLayout(false);
    }
    if (hasHScrollBar()) {
        hscrollbar->setNeedsLayout(false);
    }
}

EGSize EWWindow::calcMinimumSize()
{
    EGSize minSize;
    EGMargin dMargin(0, 0, 0, 0);
    EGint dBorder = 0;
    EGint dTitleWidth = 0;
    if (hasDecoration()) {
        dMargin = decorationMargin;
        dBorder = decorationBorderWidth;
        if (hasTitle() && titleText) {
            dMargin.top += titlePadding.top + titlePadding.bottom + titleText->getHeight();
            dTitleWidth = titleText->getWidth();
        }
        if (hasCloseBox()) {
            dTitleWidth += 16;
        }
    }
    if (hasVScrollBar()) {
        EGSize minSize = vscrollbar->calcMinimumSize();
        margin.right = minSize.width;
    }
    if (children.size() == 1) {
        minSize = children[0]->calcMinimumSize();
    }
    minSize.width = (std::max)(dTitleWidth, minSize.width);
    
    EGSize vscrollsize = hasVScrollBar() ? vscrollbar->calcMinimumSize() : EGSize();
    EGSize hscrollsize = hasHScrollBar() ? hscrollbar->calcMinimumSize() : EGSize();
    margin.right = (std::max)(hscrollsize.width, vscrollsize.width);
    margin.bottom = (std::max)(hscrollsize.height, vscrollsize.height);
    
    return minSize.expand(dBorder).expand(dMargin).expand(borderWidth).expand(margin).expand(padding);
}

void EWWindow::setSize(EGSize size)
{
    rect = EGRect(rect.x, rect.y, size.width, size.height);
    closeRect = EGRect();
    dMargin = EGMargin();
    dBorder = 0;
    if (hasDecoration()) {
        dMargin = decorationMargin;
        dBorder = decorationBorderWidth;
        if (hasTitle() && titleText) {
            dMargin.top += titlePadding.top + titlePadding.bottom + titleText->getHeight();
        }
        if (hasCloseBox()) {
            closeRect = EGRect(rect.x + rect.width - dBorder - dMargin.right - 16,
                               rect.y + dBorder + titlePadding.top + (dMargin.top - 16) / 2, 16, 16);
        }
    }
    EGRect contentRect = rect.contract(dBorder).contract(dMargin).contract(borderWidth);
    if (children.size() == 1) {
        EGRect childRect = contentRect.contract(margin).contract(padding);
        children[0]->setSize(EGSize(childRect.width, childRect.height));
        children[0]->setPosition(EGPoint(childRect.x, childRect.y));
    }
}

EGfloat* EWWindow::getTransform() { return matrix; }

EGfloat* EWWindow::getInverseTransform() { return invmatrix; }

void EWWindow::setPosition(EGPoint position)
{
    if (this->position != position) {
        this->position = position;
        EGMath::identityMat4f(matrix);
        matrix[12] = position.x;
        matrix[13] = position.y;
        EGMath::invertMat4f(invmatrix, matrix);
    }
}

EGPoint EWWindow::getPosition()
{
    return position;
}

void EWWindow::layout()
{
    if (!needsLayout) return;
    
    setSize(calcPreferredSize());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    renderer->begin();

    if (hasDecoration()) {
        if (decorationBorderWidth > 0) {
            renderer->fill(rect,
                           rect.contract(dBorder), decorationStrokeColor);
        }
        renderer->fill(rect.contract(dBorder),
                       rect.contract(dBorder).contract(dMargin), (hasFocus() && visibleFocus()) ? activeDecorationFillColor : decorationFillColor);
    }
    
    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(dBorder).contract(dMargin),
                       rect.contract(dBorder).contract(dMargin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(dBorder).contract(dMargin).contract(borderWidth), fillColor);
    }
    
    if (hasTitle() && titleText) {
        EGPoint point(rect.x + dBorder + titlePadding.left + dMargin.left,
                      rect.y + dBorder + titlePadding.top + dMargin.top / 2);
        renderer->paint(point, titleText, textColor);
    }
    
    if (hasDecoration() && hasCloseBox()) {
        if (closeBoxImage) {
            renderer->paint(closeRect, closeBoxImage, closeBoxColor);
        } else {
            EGRect cbrect = closeRect.contract(4);
            EGLine line1(EGPoint(cbrect.x, cbrect.y), EGPoint(cbrect.x + cbrect.width, cbrect.y + cbrect.height));
            EGLine line2(EGPoint(cbrect.x, cbrect.y + cbrect.height), EGPoint(cbrect.x + cbrect.width, cbrect.y));
            renderer->stroke(line1, textColor, 2);
            renderer->stroke(line2, textColor, 2);
        }
    }
    
    renderer->end();
    
    EGRect contentRect = rect.contract(dBorder).contract(dMargin).contract(borderWidth);
    
    if (hasVScrollBar()) {
        vscrollbar->setSize(EGSize(vscrollbar->calcMinimumSize().width, contentRect.height));
        vscrollbar->setPosition(EGPoint(contentRect.x + contentRect.width - vscrollbar->calcMinimumSize().width,
                                        contentRect.y));
    }
    if (hasHScrollBar()) {
        hscrollbar->setSize(EGSize(contentRect.width, hscrollbar->calcMinimumSize().height));
        hscrollbar->setPosition(EGPoint(contentRect.x,
                                        contentRect.y + contentRect.height - hscrollbar->calcMinimumSize().height));
    }

    if (children.size() == 1 && children[0]->getNeedsLayout()) {
        children[0]->layout();
    }
    
    needsLayout = false;
}

void EWWindow::subdraw()
{
    if (hasVScrollBar()) {
        vscrollbar->draw();
    }
    if (hasHScrollBar()) {
        hscrollbar->draw();
    }
}

void EWWindow::raiseWindow(EGbool focus)
{
    if (focus && !hasFocus()) {
        requestFocus();
    }
    EWContext* context = getContext();
    if (context) {
        widgetId = nextWidgetId++;
        context->sortTopLevelWidgets();
    }
}

EGbool EWWindow::mouseEvent(EGMouseEvent *evt)
{
    if (hasVScrollBar() && vscrollbar->mouseEvent(evt)) {
        raiseWindow();
        return true;
    }
    if (hasHScrollBar() && hscrollbar->mouseEvent(evt)) {
        raiseWindow();
        return true;
    }
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGRect titleRect(rect.x, rect.y, rect.width, dMargin.top + dMargin.bottom + dBorder * 2);
    EGRect resizeRect(rect.x + rect.width - dMargin.right - dBorder - borderWidth,
                      rect.y + rect.height - dMargin.bottom - dBorder - borderWidth,
                      dMargin.right + dBorder + borderWidth,
                      dMargin.bottom + dBorder + borderWidth);
    if (hasDecoration() && evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON) {
        if (hasCloseBox() && closeRect.contains(p)) {
            mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWWindow>(EGMouseEvent::MOTION, this, &EWWindow::closeBoxMouseMotion);
            mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWWindow>(EGMouseEvent::RELEASED, this, &EWWindow::closeBoxMouseReleased);
            requestFocus();
            inCloseBox = true;
            return true;
        } else if (isMovable() && titleRect.contains(p)) {
            mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWWindow>(EGMouseEvent::MOTION, this, &EWWindow::titleDragMouseMotion);
            mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWWindow>(EGMouseEvent::RELEASED, this, &EWWindow::titleDragMouseReleased);
            requestFocus();
            dragPoint = EGPoint(p.x - rect.x, p.y - rect.y);
            return true;
        } else if (isResizable() && resizeRect.contains(p)) {
            minDragSize = calcMinimumSize();
            mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWWindow>(EGMouseEvent::MOTION, this, &EWWindow::resizeDragMouseMotion);
            mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWWindow>(EGMouseEvent::RELEASED, this, &EWWindow::resizeDragMouseReleased);
            requestFocus();
            resizePoint = EGPoint(p.x - rect.width, p.y - rect.height);
            return true;
        }
    }
    return EWContainer::mouseEvent(evt);
}

EGbool EWWindow::titleDragMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    setPosition(EGPoint(position.x + (p.x - dragPoint.x), position.y + (p.y - dragPoint.y)));
    return true;
}

EGbool EWWindow::titleDragMouseReleased(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    setPosition(EGPoint(position.x + (p.x - dragPoint.x), position.y + (p.y - dragPoint.y)));
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}

EGbool EWWindow::resizeDragMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    setPreferredSize(EGSize((std::max)(minDragSize.width, p.x - resizePoint.x),
                            (std::max)(minDragSize.height, p.y - resizePoint.y)));
    return true;
}

EGbool EWWindow::resizeDragMouseReleased(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    setPreferredSize(EGSize((std::max)(minDragSize.width, p.x - resizePoint.x),
                            (std::max)(minDragSize.height, p.y - resizePoint.y)));
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}

EGbool EWWindow::closeBoxMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    inCloseBox = closeRect.contains(p);
    return true;
}

EGbool EWWindow::closeBoxMouseReleased(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    inCloseBox = closeRect.contains(p);
    if (inCloseBox) {
        EWWindowEvent evt(EWWindowEvent::CLOSED, this);
        EGEventManager::postEvent(&evt);
    }
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}
