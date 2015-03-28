/*
 *  EWSlider.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWSlider.h"


/* EWSliderEvent */

EGEventQualifier EWSliderEvent::VALUE_CHANGED   = "slider-value-changed";

EWSliderEvent::EWSliderEvent(EGEventQualifier q, EWSlider *slider, EGfloat value)
    : EGEvent(q), slider(slider), value(value) {}

std::string EWSliderEvent::toString() {
    std::stringstream s;
    s << "[EWSliderEvent " << q << " slider=" << slider << " value=" << value << "]";
    return s.str();
};


/* EWSlider */

static char class_name[] = "EWSlider";

const int EWSlider::MIN_SLIDER_LENGTH = 100;

EWSlider::EWSlider(EGenum widgetFlags) : EWWidget(widgetFlags), value(0), inSlider(false)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWSlider::~EWSlider() {}

const EGClass* EWSlider::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWSlider>()->extends<EWWidget>()
        ->property("vertical",          &EWSlider::isVertical,              &EWSlider::setVertical)
        ->property("sliderControlSize", &EWSlider::getSliderControlSize,    &EWSlider::setSliderControlSize)
        ->property("sliderThickness",   &EWSlider::getSliderThickness,      &EWSlider::setSliderThickness)
        ->property("value",             &EWSlider::getValue,                &EWSlider::setValue)
        ->emitter("valueChanged",       &EWSlider::valueChanged);
    return clazz;
}

const EGClass* EWSlider::getClass() const { return classFactory(); }

void EWSlider::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
    setVertical(widgetDefaults->getBoolean(class_name, "vertical", false));
    setSliderControlSize(widgetDefaults->getSize(class_name, "sliderControlSize", EGSize(8,20)));
    setSliderThickness(widgetDefaults->getInteger(class_name, "sliderThickness", 8));
}

const char* EWSlider::widgetTypeName() { return class_name; }

void EWSlider::setVertical(EGbool vertical)
{
    if (!isVertical() && vertical) {
        widgetFlags |= EWWidgetFlagsLayoutVertical;
        setNeedsLayout();
    } else if (isVertical() && !vertical) {
        widgetFlags &= ~EWWidgetFlagsLayoutVertical;
    }
}

void EWSlider::setSliderControlSize(EGSize sliderControlSize)
{
    if (this->sliderControlSize != sliderControlSize) {
        this->sliderControlSize = sliderControlSize;
        setNeedsLayout();
    }
}

void EWSlider::setSliderThickness(EGint sliderThickness)
{
    if (this->sliderThickness != sliderThickness) {
        this->sliderThickness = sliderThickness;
        setNeedsLayout();
    }
}

void EWSlider::setValue(EGfloat value)
{
    if (this->value != value) {
        this->value = value;
        EWSliderEvent evt(EWSliderEvent::VALUE_CHANGED, this, value);
        EGEventManager::postEvent(&evt);
        valueChanged(value);
        setNeedsLayout();
    }
}

EGbool EWSlider::isVertical() { return widgetFlags & EWWidgetFlagsLayoutVertical ? true : false; }
EGSize EWSlider::getSliderControlSize() { return sliderControlSize; }
EGint EWSlider::getSliderThickness() { return sliderThickness; }
EGfloat EWSlider::getValue() { return value; }

void EWSlider::valueChanged(EGfloat value) { emit(&EWSlider::valueChanged, value); }

EGSize EWSlider::calcMinimumSize()
{
    EGSize controlSize, sliderSize;
    if (isVertical()) {
        controlSize = EGSize(sliderControlSize.height, sliderControlSize.width);
        sliderSize = EGSize(sliderThickness, MIN_SLIDER_LENGTH);
    } else {
        controlSize = EGSize(sliderControlSize.width, sliderControlSize.height);
        sliderSize = EGSize(MIN_SLIDER_LENGTH, sliderThickness);
    }
    EGSize minSize((std::max)(controlSize.width, sliderSize.width), (std::max)(controlSize.height, sliderSize.height));
    EGSize size((std::max)(minSize.width, preferredSize.width), (std::max)(minSize.height, preferredSize.height));
    return size.expand(margin).expand(borderWidth).expand(padding);
}

void EWSlider::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    renderer->begin();

    EGRect innerRect = rect.contract(margin).contract(borderWidth).contract(padding);
    EGSize controlSize, sliderSize;
    if (isVertical()) {
        controlSize = EGSize(sliderControlSize.height, sliderControlSize.width);
        sliderSize = EGSize(sliderThickness, innerRect.height);
        controlRect = EGRect(rect.x + margin.left + borderWidth + padding.left,
                             rect.y + margin.top + borderWidth + padding.top + (innerRect.height * value) - controlSize.height / 2,
                             controlSize.width, controlSize.height);
        sliderRect = EGRect(rect.x + margin.left + borderWidth + padding.left + (innerRect.width - sliderSize.width) / 2,
                            rect.y + margin.top + borderWidth + padding.top,
                            sliderSize.width, sliderSize.height);
        eventRect = EGRect(rect.x + margin.left + borderWidth + padding.left,
                           rect.y + margin.top + borderWidth + padding.top,
                           controlSize.width, sliderSize.height);
    } else {
        controlSize = EGSize(sliderControlSize.width, sliderControlSize.height);
        sliderSize = EGSize(innerRect.width, sliderThickness);
        controlRect = EGRect(rect.x + margin.left + borderWidth + padding.left + (innerRect.width * value) - controlSize.width / 2,
                             rect.y + margin.top + borderWidth + padding.top,
                             controlSize.width, controlSize.height);
        sliderRect = EGRect(rect.x + margin.left + borderWidth + padding.left,
                            rect.y + margin.top + borderWidth + padding.top + (innerRect.height - sliderSize.height) / 2,
                            sliderSize.width, sliderSize.height);
        eventRect = EGRect(rect.x + margin.left + borderWidth + padding.left,
                           rect.y + margin.top + borderWidth + padding.top,
                           sliderSize.width, controlSize.height);
    }

    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(margin),
                       rect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(margin).contract(borderWidth), fillColor);
    }
    
    if (isEnabled() && canFocus() && hasFocus() && visibleFocus()) {
        renderer->fill(sliderRect.expand(margin), sliderRect, focusBorderColor);
    }

    renderer->fill(sliderRect, sliderRect.contract(borderWidth), strokeColor);
    renderer->fill(sliderRect.contract(borderWidth), fillColor);
    renderer->fill(controlRect, controlRect.contract(borderWidth), inSlider ? activeStrokeColor : strokeColor);
    renderer->fill(controlRect.contract(borderWidth), inSlider ? activeFillColor : fillColor);
    
    renderer->end();
    
    needsLayout = false;
}

EGbool EWSlider::mouseEvent(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON && eventRect.contains(p)) {
        mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWSlider>(EGMouseEvent::MOTION, this, &EWSlider::controlDragMouseMotion);
        mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWSlider>(EGMouseEvent::RELEASED, this, &EWSlider::controlDragMouseReleased);
        EGfloat newvalue = isVertical() ? (EGfloat)(std::min)((std::max)(p.y - eventRect.y, 0), eventRect.height) / eventRect.height
                                        : (EGfloat)(std::min)((std::max)(p.x - eventRect.x, 0), eventRect.width) / eventRect.width;
        setValue(newvalue);
        inSlider = true;
        requestFocus();
        return true;
    }
    return false;
}

EGbool EWSlider::keyEvent(EGKeyEvent *evt)
{
    EGfloat newvalue = value;
    if (evt->q == EGKeyEvent::PRESSED) {
        if (evt->charcode == 9 && evt->modifiers == EGKeyEvent::MODIFIER_NONE /* tab */ ) {
            nextFocus();
            return true;
        } else if (evt->charcode == 25 || (evt->charcode == 9 && evt->modifiers == EGKeyEvent::MODIFIER_SHIFT)) {
            prevFocus();
            return true;
        } else if (isVertical()) {
            if (evt->keycode == EGKeyEvent::KEYCODE_UP) {
                newvalue = (std::max)(0.0f, value - 0.025f);
            } else if (evt->keycode == EGKeyEvent::KEYCODE_DOWN) {
                newvalue = (std::min)(1.0f, value + 0.025f);
            }
        } else {
            if (evt->keycode == EGKeyEvent::KEYCODE_LEFT) {
                newvalue = (std::max)(0.0f, value - 0.025f);
            } else if (evt->keycode == EGKeyEvent::KEYCODE_RIGHT) {
                newvalue = (std::min)(1.0f, value + 0.025f);
            }
        }
    }
    setValue(newvalue);
    return true;
}

EGbool EWSlider::controlDragMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGfloat newvalue = isVertical() ? (EGfloat)(std::min)((std::max)(p.y - eventRect.y, 0), eventRect.height) / eventRect.height
                                    : (EGfloat)(std::min)((std::max)(p.x - eventRect.x, 0), eventRect.width) / eventRect.width;
    setValue(newvalue);
    return true;
}

EGbool EWSlider::controlDragMouseReleased(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGfloat newvalue = isVertical() ? (EGfloat)(std::min)((std::max)(p.y - eventRect.y, 0), eventRect.height) / eventRect.height
                                    : (EGfloat)(std::min)((std::max)(p.x - eventRect.x, 0), eventRect.width) / eventRect.width;
    setValue(newvalue);
    setNeedsLayout();
    inSlider = false;
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}
