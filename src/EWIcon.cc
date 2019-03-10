// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWIcon.h"


/* EWIconEvent */

EGEventQualifier EWIconEvent::PRESSED   = "icon-pressed";
EGEventQualifier EWIconEvent::RELEASED   = "icon-released";

EWIconEvent::EWIconEvent(EGEventQualifier q, EWIcon *icon)
    : EGEvent(q), icon(icon) {}

std::string EWIconEvent::toString() {
    std::stringstream s;
    s << "[EWIconEvent " << q << " icon=" << icon << "]";
    return s.str();
};


/* EWIcon */

static char class_name[] = "EWIcon";

EWIcon::EWIcon(EGenum widgetFlags) : EWWidget(widgetFlags), imageScale(1.0f), inIcon(false)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWIcon::~EWIcon() {}

const EGClass* EWIcon::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWIcon>()->extends<EWWidget>()
        ->emitter("clicked",        &EWIcon::clicked);
    return clazz;
}

const EGClass* EWIcon::getClass() const { return classFactory(); }

const char* EWIcon::widgetTypeName() { return class_name; }

EGImagePtr EWIcon::getImage() { return image; }
EGImagePtr EWIcon::getActiveImage() { return activeImage; }
EGenum EWIcon::getImageAlign() { return widgetFlags & (EWWidgetFlagsImageVAlignCenter | EWWidgetFlagsImageHAlignCenter); }
EGfloat EWIcon::getImageScale() { return imageScale; }

void EWIcon::setImage(EGImagePtr image)
{
    if (this->image != image) {
        this->image = image;
        setNeedsLayout();
    }
}

void EWIcon::setActiveImage(EGImagePtr activeImage)
{
    if (this->activeImage != activeImage) {
        this->activeImage = activeImage;
        setNeedsLayout();
    }
}

void EWIcon::setImageAlign(EGenum imageAlignFlags)
{
    if (getImageAlign() != imageAlignFlags) {
        widgetFlags &= ~(EWWidgetFlagsImageVAlignCenter | EWWidgetFlagsImageHAlignCenter);
        widgetFlags |= (EWWidgetFlagsImageVAlignCenter | EWWidgetFlagsImageHAlignCenter) & imageAlignFlags;
        setNeedsLayout();
    }
}

void EWIcon::setImageScale(EGfloat imageScale)
{
    if (this->imageScale != imageScale) {
        this->imageScale = imageScale;
        setNeedsLayout();
    }
}

void EWIcon::clicked() { emit(&EWIcon::clicked); }

EGSize EWIcon::calcMinimumSize()
{
    EGint iw = image ? (EGint)(image->getOrigWidth() / imageScale) : 0;
    EGint ih = image ? (EGint)(image->getOrigHeight() / imageScale) : 0;
    return EGSize(margin.left + margin.right + borderWidth * 2 + padding.left + padding.right + iw,
                  margin.top + margin.bottom + borderWidth * 2 + padding.top + padding.bottom + ih);
}

void EWIcon::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    renderer->begin();

    if (isEnabled() && canFocus() && hasFocus() && visibleFocus()) {
        renderer->fill(rect, rect.contract(margin), focusBorderColor);
    }

    EGImagePtr image = (inIcon && activeImage) ? this->activeImage : this->image;
    iconRect = rect;
    EGint iw = image ? (EGint)(image->getOrigWidth() / imageScale) : 0;
    EGint ih = image ? (EGint)(image->getOrigHeight() / imageScale) : 0;
    if ((widgetFlags & EWWidgetFlagsImageHAlignCenter) == EWWidgetFlagsImageHAlignCenter) {
        iconRect.x = rect.x + rect.width / 2 - iw / 2;
        iconRect.width = iw;
    } else if (widgetFlags & EWWidgetFlagsImageHAlignLeft) {
        iconRect.x = rect.x + margin.left + borderWidth + padding.left;
        iconRect.width = iw;
    } else if (widgetFlags & EWWidgetFlagsImageHAlignRight) {
        iconRect.x = rect.x + rect.width - (margin.left + margin.right + borderWidth * 2 + padding.left + padding.right + iw);
        iconRect.width = iw;
    } else { /* justify */
        iconRect.x = rect.x + margin.left + borderWidth + padding.left;
        iconRect.width = rect.width - margin.left - margin.right - borderWidth * 2 - padding.left - padding.right;
    }
    if ((widgetFlags & EWWidgetFlagsImageVAlignCenter) == EWWidgetFlagsImageVAlignCenter) {
        iconRect.y = rect.y + rect.height / 2 - ih / 2;
        iconRect.height = ih;
    } else if (widgetFlags & EWWidgetFlagsImageVAlignTop) {
        iconRect.y = rect.y + margin.top + borderWidth + padding.top;
        iconRect.height = ih;
    } else if (widgetFlags & EWWidgetFlagsImageVAlignBottom) {
        iconRect.y = rect.y + rect.height - (margin.top + margin.bottom + borderWidth * 2 + padding.top + padding.bottom + ih);
        iconRect.height = ih;
    } else { /* justify */
        iconRect.y = rect.y + margin.top + borderWidth + padding.top;
        iconRect.height = rect.height - margin.top - margin.bottom - borderWidth * 2 - padding.top - padding.bottom;
    }
    
    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(margin),
                       rect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(margin).contract(borderWidth), fillColor);
    }
    
    if (image) {
        renderer->paint(iconRect, image);
    }
    
    renderer->end();
    
    needsLayout = false;
}

EGbool EWIcon::mouseEvent(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON && rect.contains(p)) {
        mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWIcon>(EGMouseEvent::MOTION, this, &EWIcon::iconMouseMotion);
        mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWIcon>(EGMouseEvent::RELEASED, this, &EWIcon::iconMouseReleased);
        inIcon = true;
        requestFocus();
        setNeedsLayout();
        EWIconEvent evt(EWIconEvent::PRESSED, this);
        EGEventManager::postEvent(&evt);
        return true;
    }
    return false;
}

EGbool EWIcon::keyEvent(EGKeyEvent *evt)
{
    if (evt->q == EGKeyEvent::PRESSED) {
        if (evt->charcode == ' ' || evt->charcode == '\r') {
            EWIconEvent evt(EWIconEvent::RELEASED, this);
            EGEventManager::postEvent(&evt);
            clicked();
            return true;
        } else if (evt->charcode == 9 && evt->modifiers == EGKeyEvent::MODIFIER_NONE /* tab */ ) {
            nextFocus();
            return true;
        } else if (evt->charcode == 25 || (evt->charcode == 9 && evt->modifiers == EGKeyEvent::MODIFIER_SHIFT)) {
            prevFocus();
            return true;
        }
    }
    return false;
}

EGbool EWIcon::iconMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGbool b = rect.contains(p);
    if (inIcon != b) {
        setNeedsLayout();
    }
    inIcon = b;
    return true;
}

EGbool EWIcon::iconMouseReleased(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    inIcon = rect.contains(p);
    if (inIcon) {
        setNeedsLayout();
        EWIconEvent evt(EWIconEvent::RELEASED, this);
        EGEventManager::postEvent(&evt);
        clicked();
    }
    inIcon = false;
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}

