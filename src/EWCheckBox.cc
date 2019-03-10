// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWCheckBox.h"


/* EWCheckBoxEvent */

EGEventQualifier EWCheckBoxEvent::VALUE_CHANGED   = "checkbox-value-changed";

EWCheckBoxEvent::EWCheckBoxEvent(EGEventQualifier q, EWCheckBox *checkbox, EGbool value)
    : EGEvent(q), checkbox(checkbox), value(value) {}

std::string EWCheckBoxEvent::toString() {
    std::stringstream s;
    s << "[EWCheckBoxEvent " << q << " checkbox=" << checkbox << " value=" << value << "]";
    return s.str();
};


/* EWCheckBox */

static char class_name[] = "EWCheckBox";

const EGint EWCheckBox::CHECKBOX_WIDTH = 16;
const EGint EWCheckBox::CHECKBOX_HEIGHT = 16;

EWCheckBox::EWCheckBox(EGenum widgetFlags) : EWWidget(widgetFlags), value(false), inCheckBox(false)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWCheckBox::~EWCheckBox() {}

const EGClass* EWCheckBox::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWCheckBox>()->extends<EWWidget>()
        ->property("value",             &EWCheckBox::getValue,              &EWCheckBox::setValue)
        ->emitter("valueChanged",       &EWCheckBox::valueChanged);
    return clazz;
}

const EGClass* EWCheckBox::getClass() const { return classFactory(); }

void EWCheckBox::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
    onImage = widgetDefaults->getImage(class_name, "onImage", EGImagePtr());
    offImage = widgetDefaults->getImage(class_name, "offImage", EGImagePtr());
}

const char* EWCheckBox::widgetTypeName() { return class_name; }

void EWCheckBox::setValue(EGbool value)
{
    if (this->value != value) {
        this->value = value;
        EWCheckBoxEvent evt(EWCheckBoxEvent::VALUE_CHANGED, this, value);
        EGEventManager::postEvent(&evt);
        valueChanged(value);
        setNeedsLayout();
    }
}

EGbool EWCheckBox::getValue() { return value; }

void EWCheckBox::valueChanged(EGbool value) { emit(&EWCheckBox::valueChanged, value); }

EGSize EWCheckBox::calcMinimumSize()
{
    EGSize minSize((std::max)(preferredSize.width, CHECKBOX_WIDTH), (std::max)(preferredSize.height, CHECKBOX_HEIGHT));
    return minSize.expand(margin).expand(borderWidth).expand(padding);
}

void EWCheckBox::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    
    checkBoxRect = EGRect(rect.x + padding.left + borderWidth + margin.left,
                          rect.y + padding.right + borderWidth + margin.right,
                          CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
    
    renderer->begin();
        
    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(margin),
                       rect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(margin).contract(borderWidth), fillColor);
    }
    
    if (isEnabled() && canFocus() && hasFocus() && visibleFocus()) {
        renderer->fill(checkBoxRect.expand(margin), checkBoxRect, focusBorderColor);
    }

    if (onImage && offImage) {
        renderer->paint(checkBoxRect, value ? onImage : offImage, inCheckBox ? activeFillColor : fillColor);
    } else {
        EGRect cbrect = checkBoxRect.contract(2);
        renderer->fill(cbrect, cbrect.contract(1), inCheckBox ? activeStrokeColor : strokeColor);
        renderer->fill(cbrect.contract(1), inCheckBox ? activeFillColor : fillColor);
        if (value) {
            cbrect = cbrect.contract(3);
            EGLine line1(EGPoint(cbrect.x, cbrect.y), EGPoint(cbrect.x + cbrect.width, cbrect.y + cbrect.height));
            EGLine line2(EGPoint(cbrect.x + cbrect.width, cbrect.y), EGPoint(cbrect.x, cbrect.y + cbrect.height));
            renderer->stroke(line1, inCheckBox ? activeTextColor : textColor, 1);
            renderer->stroke(line2, inCheckBox ? activeTextColor : textColor, 1);
        }
    }
    renderer->end();
    
    needsLayout = false;
}

EGbool EWCheckBox::mouseEvent(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON && checkBoxRect.contains(p)) {
        mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWCheckBox>(EGMouseEvent::MOTION, this, &EWCheckBox::checkBoxMouseMotion);
        mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWCheckBox>(EGMouseEvent::RELEASED, this, &EWCheckBox::checkBoxMouseReleased);
        inCheckBox = true;
        requestFocus();
        return true;
    }
    return false;
}

EGbool EWCheckBox::keyEvent(EGKeyEvent *evt)
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

EGbool EWCheckBox::checkBoxMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGbool b = checkBoxRect.contains(p);
    if (inCheckBox != b) {
        setNeedsLayout();
    }
    inCheckBox = b;
    return true;
}

EGbool EWCheckBox::checkBoxMouseReleased(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    inCheckBox = checkBoxRect.contains(p);
    if (inCheckBox) {
        setValue(!value);
    }
    inCheckBox = false;
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}

