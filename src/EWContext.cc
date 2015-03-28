/*
 *  EWContext.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGMath.h"
#include "EGViewport.h"

#include "EWWidget.h"
#include "EWContainer.h"
#include "EWContext.h"


/* EWContext */

static char class_name[] = "EWContext";

EWContext::EWContext(EGViewportPtr viewport, EGenum widgetFlags) : EWContainer(widgetFlags), viewport(viewport), drawTime(0LL)
{
    setDefaultWidgetName();
    mousePressedEvent = EGEventManager::connect<EGMouseEvent,EWContext>(EGMouseEvent::PRESSED, this, &EWContext::mouseEvent);
    mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWContext>(EGMouseEvent::RELEASED, this, &EWContext::mouseEvent);
    keyPressedEvent = EGEventManager::connect<EGKeyEvent,EWContext>(EGKeyEvent::PRESSED, this, &EWContext::keyEvent);
    keyReleasedEvent = EGEventManager::connect<EGKeyEvent,EWContext>(EGKeyEvent::RELEASED, this, &EWContext::keyEvent);
    
    reshape();
}

EWContext::~EWContext()
{
    //EGEventManager::disconnect(mousePressedEvent);
    //EGEventManager::disconnect(mouseReleasedEvent);
    //EGEventManager::disconnect(keyPressedEvent);
    //EGEventManager::disconnect(keyReleasedEvent);
}

const EGClass* EWContext::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWContext>()->extends<EWContainer>();
    return clazz;
}

const EGClass* EWContext::getClass() const { return classFactory(); }

const char* EWContext::widgetTypeName() { return class_name; }

void EWContext::sortTopLevelWidgets()
{
    struct WidgetSorter {
        EGbool operator()(const EWWidgetPtr &a, const EWWidgetPtr &b) {
            return a->zOrder < b->zOrder || (a->zOrder == b->zOrder && a->widgetId < b->widgetId);
        }
    };
    sort(children.begin(), children.end(), WidgetSorter());
}

void EWContext::addWidget(EWWidgetPtr widget)
{
    if (widget) {
        widget->setParent(this);
        children.push_back(widget);
        sortTopLevelWidgets();
    }
}

void EWContext::removeWidget(EWWidgetPtr widget)
{
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end();) {
        if (*wi == widget) {
            widget->setParent(NULL);
            wi = children.erase(wi);
        } else {
            wi++;
        }
    }
}

EGViewportPtr EWContext::getViewport() { return viewport; }

const EGTime& EWContext::getDrawTime() { return drawTime; }

EGfloat* EWContext::getTransform() { return matrix; }

EGfloat* EWContext::getInverseTransform() { return invmatrix; }

void EWContext::reshape()
{
#if 0
    const EGfloat angle = 1.0f;
    EGfloat tx = viewport->screenWidth * 0.5f;
    EGfloat ty = viewport->screenHeight * 0.5f;
    EGfloat sa = sinf(angle * EGMath::Deg2Rad);
    EGfloat ca = cosf(angle * EGMath::Deg2Rad);
    EGfloat pretrans[16] = {
        1,   0,   0,   0,
        0,   1  , 0,   0,
        0,   0,   1,   0,
        -tx, -ty, 0,   1
    };
    EGfloat posttrans[16] = {
        1,   0,   0,   0,
        0,   1  , 0,   0,
        0,   0,   1,   0,
        tx,  ty,  0,   1
    };
    EGfloat rot[16] = {
        ca,  sa,  0,   0,
        -sa, ca,  0,   0,
        0,   0,   1,   0,
        0,   0,   0,   1
    };
    EGfloat tempmatrix[16];
    EGMath::multMat4fMat4f(tempmatrix, pretrans, rot);
    EGMath::multMat4fMat4f(matrix, tempmatrix, posttrans);
    EGMath::invertMat4f(invmatrix, matrix);
#else
    EGMath::identityMat4f(matrix);
    EGMath::identityMat4f(invmatrix);
#endif
}

void EWContext::draw()
{
    drawTime = EGTime();

    /* make sure we lay everything out before we start drawing to avoid gl buffer setup during draw calls */
    for (EWWidgetList::iterator wi = children.begin(); wi != children.end(); wi++) {
        if ((*wi)->needsLayout) {
            (*wi)->layout();
        }
    }

    EWWidget::draw();
}

EGbool EWContext::keyEvent(EGKeyEvent *evt)
{
    if (focusWidget && focusWidget->isEnabled()) {
        return focusWidget->keyEvent(evt);
    }
    return false;
}
