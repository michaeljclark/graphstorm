// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWScrollBar.h"


/* EWScrollBarEvent */

EGEventQualifier EWScrollBarEvent::VALUE_CHANGED   = "scrollbar-value-changed";

EWScrollBarEvent::EWScrollBarEvent(EGEventQualifier q, EWScrollBar *scrollbar, EGfloat value)
    : EGEvent(q), scrollbar(scrollbar), value(value) {}

std::string EWScrollBarEvent::toString() {
    std::stringstream s;
    s << "[EWScrollBarEvent " << q << " scrollbar=" << scrollbar << " value=" << value << "]";
    return s.str();
};


/* EWScrollBar */

static char class_name[] = "EWScrollBar";

EWScrollBar::EWScrollBar(EGenum widgetFlags) : EWWidget(widgetFlags), value(0), inBar(false)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWScrollBar::~EWScrollBar() {}

const EGClass* EWScrollBar::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWScrollBar>()->extends<EWWidget>()
        ->property("vertical",          &EWScrollBar::isVertical,           &EWScrollBar::setVertical)
        ->property("barColor",          &EWScrollBar::getBarColor,          &EWScrollBar::setBarColor)
        ->property("barControlSize",    &EWScrollBar::getBarControlSize,    &EWScrollBar::setBarControlSize)
        ->property("barThickness",      &EWScrollBar::getBarThickness,      &EWScrollBar::setBarThickness);
    return clazz;
}

const EGClass* EWScrollBar::getClass() const { return classFactory(); }

void EWScrollBar::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
    setVertical(widgetDefaults->getBoolean(class_name, "vertical", false));
    setBarColor(widgetDefaults->getColor(class_name, "barColor", EGColor(0.85f,0.85f,0.85f,1)));
    setBarControlSize(widgetDefaults->getSize(class_name, "barControlSize", EGSize(40,20)));
    setBarThickness(widgetDefaults->getInteger(class_name, "barThickness", 20));
}

const char* EWScrollBar::widgetTypeName() { return class_name; }

void EWScrollBar::setVertical(EGbool vertical)
{
    if (!isVertical() && vertical) {
        widgetFlags |= EWWidgetFlagsLayoutVertical;
        setNeedsLayout();
    } else if (isVertical() && !vertical) {
        widgetFlags &= ~EWWidgetFlagsLayoutVertical;
    }
}

void EWScrollBar::setBarColor(EGColor barColor)
{
    if (this->barColor != barColor) {
        this->barColor = barColor;
        setNeedsLayout();
    }
}

void EWScrollBar::setBarControlSize(EGSize barControlSize)
{
    if (this->barControlSize != barControlSize) {
        this->barControlSize = barControlSize;
        setNeedsLayout();
    }
}

void EWScrollBar::setBarThickness(EGint barThickness)
{
    if (this->barThickness != barThickness) {
        this->barThickness = barThickness;
        setNeedsLayout();
    }
}

void EWScrollBar::setValue(EGfloat value)
{
    if (this->value != value) {
        this->value = value;
        setNeedsLayout();
    }
}

EGbool EWScrollBar::isVertical() { return widgetFlags & EWWidgetFlagsLayoutVertical ? true : false; }
EGColor EWScrollBar::getBarColor() { return barColor; }
EGSize EWScrollBar::getBarControlSize() { return barControlSize; }
EGint EWScrollBar::getBarThickness() { return barThickness; }
EGfloat EWScrollBar::getValue() { return value; }

EGSize EWScrollBar::calcMinimumSize()
{
    EGSize controlSize, BarSize;
    if (isVertical()) {
        controlSize = EGSize(barControlSize.height, barControlSize.width);
        BarSize = EGSize(barThickness, barThickness);
    } else {
        controlSize = EGSize(barControlSize.width, barControlSize.height);
        BarSize = EGSize(barThickness, barThickness);
    }
    EGSize minSize((std::max)(controlSize.width, BarSize.width), (std::max)(controlSize.height, BarSize.height));
    EGSize size((std::max)(minSize.width, preferredSize.width), (std::max)(minSize.height, preferredSize.height));
    return size.expand(margin).expand(borderWidth).expand(padding);
}

void EWScrollBar::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    renderer->begin();
    
    EGRect innerRect = rect.contract(margin).contract(borderWidth).contract(padding);
    EGSize controlSize, BarSize;
    if (isVertical()) {
        controlSize = EGSize(barControlSize.height, barControlSize.width);
        BarSize = EGSize(barThickness, innerRect.height);
        controlRect = EGRect(rect.x + margin.left + borderWidth + padding.left,
                             rect.y + margin.top + borderWidth + padding.top + ((innerRect.height- controlSize.height) * value),
                             controlSize.width, controlSize.height);
        barRect = EGRect(rect.x + margin.left + borderWidth + padding.left + (innerRect.width - BarSize.width) / 2,
                         rect.y + margin.top + borderWidth + padding.top,
                         BarSize.width, BarSize.height);
        eventRect = EGRect(rect.x + margin.left + borderWidth + padding.left,
                           rect.y + margin.top + borderWidth + padding.top,
                           controlSize.width, BarSize.height);
    } else {
        controlSize = EGSize(barControlSize.width, barControlSize.height);
        BarSize = EGSize(innerRect.width, barThickness);
        controlRect = EGRect(rect.x + margin.left + borderWidth + padding.left + ((innerRect.width - controlSize.width) * value),
                             rect.y + margin.top + borderWidth + padding.top,
                             controlSize.width, controlSize.height);
        barRect = EGRect(rect.x + margin.left + borderWidth + padding.left,
                         rect.y + margin.top + borderWidth + padding.top + (innerRect.height - BarSize.height) / 2,
                         BarSize.width, BarSize.height);
        eventRect = EGRect(rect.x + margin.left + borderWidth + padding.left,
                           rect.y + margin.top + borderWidth + padding.top,
                           BarSize.width, controlSize.height);
    }
    
    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(margin),
                       rect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(margin).contract(borderWidth), fillColor);
    }
    
    renderer->fill(barRect, barRect.contract(borderWidth), strokeColor);
    renderer->fill(barRect.contract(borderWidth), barColor);
    renderer->fill(controlRect, controlRect.contract(borderWidth), inBar ? activeStrokeColor : strokeColor);
    renderer->fill(controlRect.contract(borderWidth), inBar ? activeFillColor : fillColor);
    
    renderer->end();
    
    needsLayout = false;
}

EGbool EWScrollBar::mouseEvent(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON && eventRect.contains(p)) {
        mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWScrollBar>(EGMouseEvent::MOTION, this, &EWScrollBar::controlDragMouseMotion);
        mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWScrollBar>(EGMouseEvent::RELEASED, this, &EWScrollBar::controlDragMouseReleased);
        EGfloat newvalue = isVertical() ? (EGfloat)(std::min)((std::max)(p.y - eventRect.y, 0), eventRect.height) / eventRect.height
                                        : (EGfloat)(std::min)((std::max)(p.x - eventRect.x, 0), eventRect.width) / eventRect.width;
        if (value != newvalue) {
            value = newvalue;
            EWScrollBarEvent evt(EWScrollBarEvent::VALUE_CHANGED, this, value);
            EGEventManager::postEvent(&evt);
        }
        inBar = true;
        requestFocus();
        return true;
    }
    return false;
}

EGbool EWScrollBar::keyEvent(EGKeyEvent *evt)
{
    EGfloat newvalue = value;
    if (evt->q == EGKeyEvent::PRESSED) {
        if (isVertical()) {
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
    if (value != newvalue) {
        value = newvalue;
        setNeedsLayout();
        EWScrollBarEvent evt(EWScrollBarEvent::VALUE_CHANGED, this, value);
        EGEventManager::postEvent(&evt);
    }
    return true;
}

EGbool EWScrollBar::controlDragMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGfloat newvalue = isVertical() ? (EGfloat)(std::min)((std::max)(p.y - eventRect.y, 0), eventRect.height) / eventRect.height
                                    : (EGfloat)(std::min)((std::max)(p.x - eventRect.x, 0), eventRect.width) / eventRect.width;
    if (value != newvalue) {
        value = newvalue;
        setNeedsLayout();
        EWScrollBarEvent evt(EWScrollBarEvent::VALUE_CHANGED, this, value);
        EGEventManager::postEvent(&evt);
    }
    return true;
}

EGbool EWScrollBar::controlDragMouseReleased(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGfloat newvalue = isVertical() ? (EGfloat)(std::min)((std::max)(p.y - eventRect.y, 0), eventRect.height) / eventRect.height
                                    : (EGfloat)(std::min)((std::max)(p.x - eventRect.x, 0), eventRect.width) / eventRect.width;
    if (value != newvalue) {
        value = newvalue;
        EWScrollBarEvent evt(EWScrollBarEvent::VALUE_CHANGED, this, value);
        EGEventManager::postEvent(&evt);
    }
    setNeedsLayout();
    inBar = false;
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}
