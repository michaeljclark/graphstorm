// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWButton.h"


/* EWButtonEvent */

EGEventQualifier EWButtonEvent::PRESSED   = "button-pressed";
EGEventQualifier EWButtonEvent::RELEASED   = "button-released";

EWButtonEvent::EWButtonEvent(EGEventQualifier q, EWButton *button)
    : EGEvent(q), button(button) {}

std::string EWButtonEvent::toString() {
    std::stringstream s;
    s << "[EWButtonEvent " << q << " button=" << button << "]";
    return s.str();
};


/* EWButton */

static char class_name[] = "EWButton";

EWButton::EWButton(EGstring label) : EWWidget(0), inButton(false)
{
    setDefaultWidgetName();
    setDefaults(class_name);
    setLabel(label);
}

EWButton::EWButton(EGenum widgetFlags) : EWWidget(widgetFlags), inButton(false)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWButton::~EWButton() {}

const EGClass* EWButton::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWButton>()->extends<EWWidget>()
        ->emitter("clicked",        &EWButton::clicked);
    return clazz;
}

const EGClass* EWButton::getClass() const { return classFactory(); }

const char* EWButton::widgetTypeName() { return class_name; }

EGstring EWButton::getLabel() { return label; }
EGImagePtr EWButton::getImage() { return image; }
EGenum EWButton::getImageAlign() { return widgetFlags & (EWWidgetFlagsImageVAlignCenter | EWWidgetFlagsImageHAlignCenter); }
EGTextPtr EWButton::getText() { return text; }

void EWButton::setLabel(EGstring label)
{
    if (this->label != label) {
        if (!text) {
            text = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignCenter | EGTextVAlignCenter);
        }
        text->setText(label);
        setNeedsLayout();
    }
}

void EWButton::setImage(EGImagePtr image)
{
    if (this->image != image) {
        this->image = image;
        setNeedsLayout();
    }
}

void EWButton::setImageAlign(EGenum imageAlignFlags)
{
    if (getImageAlign() != imageAlignFlags) {
        widgetFlags &= ~(EWWidgetFlagsImageVAlignCenter | EWWidgetFlagsImageHAlignCenter);
        widgetFlags |= (EWWidgetFlagsImageVAlignCenter | EWWidgetFlagsImageHAlignCenter) & imageAlignFlags;
        setNeedsLayout();
    }
}

void EWButton::setText(EGTextPtr text)
{
    if (this->text != text) {
        this->text = text;
        setNeedsLayout();
    }
}

void EWButton::clicked() { emit(&EWButton::clicked); }

EGSize EWButton::calcMinimumSize()
{
    return EGSize(margin.left + margin.right + padding.left + padding.right + borderWidth * 2 + (text ? text->getWidth() : 0),
                  margin.top + margin.bottom + padding.top + padding.bottom + borderWidth * 2 + (text ? text->getHeight() : 0));
}

void EWButton::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }

    buttonRect = rect.contract(margin);

    renderer->begin();

    if (isEnabled() && canFocus() && hasFocus() && visibleFocus()) {
        renderer->fill(rect, rect.contract(margin), focusBorderColor);
    }
    
    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(buttonRect,
                       buttonRect.contract(borderWidth), inButton ? activeStrokeColor : strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(buttonRect.contract(borderWidth), inButton ? activeFillColor : fillColor);
    }
    
    if (text) {
        EGenum textFlags = text->getFlags();
        EGfloat x = rect.x, y = rect.y;
        if (textFlags & EGTextHAlignLeft) {
            x += margin.left + padding.left + borderWidth;
        } else if (textFlags & EGTextHAlignCenter) {
            x += rect.width / 2;
        } else if (textFlags & EGTextHAlignRight) {
            x += rect.width - margin.right - padding.right - borderWidth;
        }
        if (textFlags & EGTextVAlignTop) {
            y += margin.top + padding.top + borderWidth;
        } else if (textFlags & EGTextVAlignBase) {
            y += rect.height - margin.bottom - padding.bottom - borderWidth;
        } else if (textFlags & EGTextVAlignCenter) {
            y += rect.height / 2;
        }
        renderer->paint(EGPoint(x, y), text, inButton ? activeTextColor : textColor);
    }
    
    renderer->end();
    
    needsLayout = false;
}

EGbool EWButton::mouseEvent(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON && buttonRect.contains(p)) {
        mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWButton>(EGMouseEvent::MOTION, this, &EWButton::buttonMouseMotion);
        mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWButton>(EGMouseEvent::RELEASED, this, &EWButton::buttonMouseReleased);
        inButton = true;
        requestFocus();
        setNeedsLayout();
        EWButtonEvent evt(EWButtonEvent::PRESSED, this);
        EGEventManager::postEvent(&evt);
        return true;
    }
    return false;
}

EGbool EWButton::keyEvent(EGKeyEvent *evt)
{
    if (evt->q == EGKeyEvent::PRESSED) {
        if (evt->charcode == ' ' || evt->charcode == '\r') {
            EWButtonEvent evt(EWButtonEvent::RELEASED, this);
            EGEventManager::postEvent(&evt);
            clicked();
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

EGbool EWButton::buttonMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGbool b = buttonRect.contains(p);
    if (inButton != b) {
        setNeedsLayout();
    }
    inButton = b;
    return true;
}

EGbool EWButton::buttonMouseReleased(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    inButton = buttonRect.contains(p);
    if (inButton) {
        setNeedsLayout();
        EWButtonEvent evt(EWButtonEvent::RELEASED, this);
        EGEventManager::postEvent(&evt);
        clicked();
    }
    inButton = false;
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}

