// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWToggleButton.h"


/* EWToggleButtonEvent */

EGEventQualifier EWToggleButtonEvent::VALUE_CHANGED   = "ToggleButton-value-changed";

EWToggleButtonEvent::EWToggleButtonEvent(EGEventQualifier q, EWToggleButton *ToggleButton, EGbool value)
: EGEvent(q), ToggleButton(ToggleButton), value(value) {}

std::string EWToggleButtonEvent::toString() {
    std::stringstream s;
    s << "[EWToggleButtonEvent " << q << " ToggleButton=" << ToggleButton << " value=" << value << "]";
    return s.str();
};


/* EWToggleButton */

static char class_name[] = "EWToggleButton";

const EGint EWToggleButton::TOGGLEBUTTON_WIDTH = 48;
const EGint EWToggleButton::TOGGLEBUTTON_HEIGHT = 24;

EWToggleButton::EWToggleButton(EGenum widgetFlags) : EWWidget(widgetFlags), value(false), inToggleButton(false)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWToggleButton::~EWToggleButton() {}

const EGClass* EWToggleButton::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWToggleButton>()->extends<EWWidget>()
        ->property("value",             &EWToggleButton::getValue,              &EWToggleButton::setValue)
        ->emitter("valueChanged",       &EWToggleButton::valueChanged);
    return clazz;
}

const EGClass* EWToggleButton::getClass() const { return classFactory(); }

void EWToggleButton::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
    onImage = widgetDefaults->getImage(class_name, "onImage", EGImagePtr());
    offImage = widgetDefaults->getImage(class_name, "offImage", EGImagePtr());
}

const char* EWToggleButton::widgetTypeName() { return class_name; }

void EWToggleButton::setValue(EGbool value)
{
    if (this->value != value) {
        this->value = value;
        EWToggleButtonEvent evt(EWToggleButtonEvent::VALUE_CHANGED, this, value);
        EGEventManager::postEvent(&evt);
        valueChanged(value);
        setNeedsLayout();
    }
}

EGbool EWToggleButton::getValue() { return value; }

void EWToggleButton::valueChanged(EGbool value) { emit(&EWToggleButton::valueChanged, value); }

EGSize EWToggleButton::calcMinimumSize()
{
    EGSize minSize((std::max)(preferredSize.width, TOGGLEBUTTON_WIDTH), (std::max)(preferredSize.height, TOGGLEBUTTON_HEIGHT));
    return minSize.expand(margin).expand(borderWidth).expand(padding);
}

void EWToggleButton::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    
    toggleButtonRect = EGRect(rect.x + padding.left + borderWidth + margin.left,
                          rect.y + padding.right + borderWidth + margin.right,
                          TOGGLEBUTTON_WIDTH, TOGGLEBUTTON_HEIGHT);
    
    renderer->begin();
    
    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(margin),
                       rect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(margin).contract(borderWidth), fillColor);
    }
    
    if (isEnabled() && canFocus() && hasFocus() && visibleFocus()) {
        renderer->fill(toggleButtonRect.expand(margin), toggleButtonRect, focusBorderColor);
    }
    
    if (onImage && offImage) {
        renderer->paint(toggleButtonRect, value ? onImage : offImage, inToggleButton ? activeFillColor : fillColor);
    } else {
        EGRect cbrect = toggleButtonRect.contract(2);
        renderer->fill(cbrect, cbrect.contract(1), inToggleButton ? activeStrokeColor : strokeColor);
        renderer->fill(cbrect.contract(1), value ? selectColor : fillColor);
        cbrect = cbrect.contract(3);
        if (value) {
            cbrect.x += cbrect.width / 2;
            cbrect.width = cbrect.width / 2;
        } else {
            cbrect.width = cbrect.width / 2;
        }
        renderer->fill(cbrect, cbrect.contract(borderWidth), inToggleButton ? activeStrokeColor : strokeColor);
        renderer->fill(cbrect.contract(borderWidth), inToggleButton ? activeFillColor : fillColor);
    }
    renderer->end();
    
    needsLayout = false;
}

EGbool EWToggleButton::mouseEvent(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON && toggleButtonRect.contains(p)) {
        mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWToggleButton>(EGMouseEvent::MOTION, this, &EWToggleButton::toggleButtonMouseMotion);
        mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWToggleButton>(EGMouseEvent::RELEASED, this, &EWToggleButton::toggleButtonMouseReleased);
        inToggleButton = true;
        requestFocus();
        return true;
    }
    return false;
}

EGbool EWToggleButton::keyEvent(EGKeyEvent *evt)
{
    if (evt->q == EGKeyEvent::PRESSED) {
        if (evt->charcode == ' ' || evt->charcode == '\r') {
            setValue(!value);
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

EGbool EWToggleButton::toggleButtonMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGbool b = toggleButtonRect.contains(p);
    if (inToggleButton != b) {
        setNeedsLayout();
    }
    inToggleButton = b;
    return true;
}

EGbool EWToggleButton::toggleButtonMouseReleased(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    inToggleButton = toggleButtonRect.contains(p);
    if (inToggleButton) {
        setValue(!value);
    }
    inToggleButton = false;
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}

