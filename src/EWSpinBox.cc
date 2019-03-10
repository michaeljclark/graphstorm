// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWEntry.h"
#include "EWIcon.h"
#include "EWSpinBox.h"


/* EWSpinBoxEvent */

EGEventQualifier EWSpinBoxEvent::VALUE_CHANGED   = "spinbox-value-changed";

EWSpinBoxEvent::EWSpinBoxEvent(EGEventQualifier q, EWSpinBox *spinbox, EGdouble value)
    : EGEvent(q), spinbox(spinbox), value(value) {}

std::string EWSpinBoxEvent::toString() {
    std::stringstream s;
    s << "[EWSpinBoxEvent " << q << " spinbox=" << spinbox << " value=" << value << "]";
    return s.str();
};


/* EWSpinBox */

static char class_name[] = "EWSpinBox";

const int EWSpinBox::CONTROL_WIDTH = 16;

EWSpinBox::EWSpinBox(EGenum widgetFlags) : EWEntry(widgetFlags),
    value((std::numeric_limits<EGdouble>::quiet_NaN)()),
    minimum(-(std::numeric_limits<EGdouble>::max)()),
    maximum((std::numeric_limits<EGdouble>::max)()),
    increment(1),
    precision(9)
{
    upIcon = EWIconPtr(new EWIcon());
    downIcon = EWIconPtr(new EWIcon());
    
    setValue(0);
    setDefaultWidgetName();
    setDefaults(class_name);
    
    upIcon->setImage(widgetDefaults->getImage(class_name, "upImage", EGImagePtr()));
    upIcon->setActiveImage(widgetDefaults->getImage(class_name, "upPressedImage", EGImagePtr()));
    upIcon->setImageAlign(EWWidgetFlagsImageHAlignCenter | EWWidgetFlagsImageVAlignCenter);
    upIcon->setImageScale(2.0f);
    upIcon->setCanFocus(false);
    upIcon->setVisibleFocus(false);
    upIcon->setParent(this);
    upIcon->show();
    
    downIcon->setImage(widgetDefaults->getImage(class_name, "downImage", EGImagePtr()));
    downIcon->setActiveImage(widgetDefaults->getImage(class_name, "downPressedImage", EGImagePtr()));
    downIcon->setImageAlign(EWWidgetFlagsImageHAlignCenter | EWWidgetFlagsImageVAlignCenter);
    downIcon->setImageScale(2.0f);
    downIcon->setCanFocus(false);
    downIcon->setVisibleFocus(false);
    downIcon->setParent(this);
    downIcon->show();

    EGEventManager::connect<EWIconEvent,EWSpinBox>(EWIconEvent::RELEASED, this, &EWSpinBox::iconEvent);
    EGEventManager::connect<EWEntryEvent,EWSpinBox>(EWEntryEvent::TEXT_CHANGED, this, &EWSpinBox::entryEvent);
}

EWSpinBox::~EWSpinBox() {}

const EGClass* EWSpinBox::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWSpinBox>()->extends<EWWidget>()
        ->property("minimum",           &EWSpinBox::getMinimum,         &EWSpinBox::setMinimum)
        ->property("maximum",           &EWSpinBox::getMaximum,         &EWSpinBox::setMaximum)
        ->property("increment",         &EWSpinBox::getIncrement,       &EWSpinBox::setIncrement)
        ->property("precision",         &EWSpinBox::getPrecision,       &EWSpinBox::setPrecision)
        ->property("value",             &EWSpinBox::getValue,           &EWSpinBox::setValue)
        ->emitter("valueChanged",       &EWSpinBox::valueChanged);
    return clazz;
}

const EGClass* EWSpinBox::getClass() const { return classFactory(); }

void EWSpinBox::setDefaults(const char *class_name)
{
    EWEntry::setDefaults(class_name);
    setControlColor(widgetDefaults->getColor(class_name, "controlColor", EGColor(0.85f,0.85f,0.85f,1)));
}

const char* EWSpinBox::widgetTypeName() { return class_name; }

void EWSpinBox::setControlColor(EGColor controlColor)
{
    if (this->controlColor != controlColor) {
        this->controlColor = controlColor;
        setNeedsLayout();
    }
}

void EWSpinBox::setString(EGstring string)
{
    EGdouble value = atof(string.c_str());

    if (value < minimum) return;
    if (value > maximum) return;
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    
    if (this->value != value) {
        this->value = value;
        
        EWSpinBoxEvent evt(EWSpinBoxEvent::VALUE_CHANGED, this, value);
        EGEventManager::postEvent(&evt);

        valueChanged(value);

        setNeedsLayout();
    }

    EWEntry::setString(ss.str());
}

void EWSpinBox::setValue(EGdouble value)
{
    if (this->value != value) {
        
        if (value < minimum) value = minimum;
        if (value > maximum) value = maximum;
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        setString(ss.str());
    }
}

void EWSpinBox::setMinimum(EGdouble minimum)
{
    if (this->minimum != minimum) {
        this->minimum = minimum;
        setValue(value);
        setNeedsLayout();
    }
}

void EWSpinBox::setMaximum(EGdouble maximum)
{
    if (this->maximum != maximum) {
        this->maximum = maximum;
        setValue(value);
        setNeedsLayout();
    }
}

void EWSpinBox::setIncrement(EGdouble increment)
{
    if (this->increment != increment) {
        this->increment = increment;
        setNeedsLayout();
    }
}

void EWSpinBox::setPrecision(EGint precision)
{
    if (this->precision != precision) {
        this->precision = precision;
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        setString(ss.str());
        
        setNeedsLayout();
    }
}

void EWSpinBox::setMinimumMaximumIncrementPrecision(EGdouble minimum, EGdouble maximum, EGdouble increment, EGint precision)
{
    setMinimum(minimum);
    setMaximum(maximum);
    setIncrement(increment);
    setPrecision(precision);
}


EGColor EWSpinBox::getControlColor() { return controlColor; }
EGdouble EWSpinBox::getValue() { return value; }
EGdouble EWSpinBox::getMinimum() { return minimum; }
EGdouble EWSpinBox::getMaximum() { return maximum; }
EGdouble EWSpinBox::getIncrement() { return increment; }
EGint EWSpinBox::getPrecision() { return precision; }

void EWSpinBox::valueChanged(EGfloat value) { emit(&EWSpinBox::valueChanged, value); }

void EWSpinBox::setNeedsLayout(EGbool propagateUpwards)
{
    EWEntry::setNeedsLayout(propagateUpwards);
    upIcon->setNeedsLayout(false);
    downIcon->setNeedsLayout(false);
}

EGSize EWSpinBox::calcMinimumSize()
{
    EGSize minSize = EWEntry::calcMinimumSize();
    minSize.width += CONTROL_WIDTH;
    return minSize;
}

void EWSpinBox::setPosition(EGPoint pos)
{
    EWEntry::setPosition(pos);
    upIcon->setPosition(EGPoint(pos.x + rect.width - CONTROL_WIDTH, pos.y));
    downIcon->setPosition(EGPoint(pos.x + rect.width - CONTROL_WIDTH, pos.y + rect.height / 2));
}

void EWSpinBox::setSize(EGSize size)
{
    EWEntry::setSize(size);
    upIcon->setSize(EGSize(CONTROL_WIDTH, size.height/2));
    downIcon->setSize(EGSize(CONTROL_WIDTH, size.height/2));
}

void EWSpinBox::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
        cursorRenderer = createRenderer(this);
    }
    
    renderer->begin();
    
    if (isEnabled() && canFocus() && hasFocus() && visibleFocus()) {
        renderer->fill(rect, rect.contract(margin), focusBorderColor);
    }
    
    EGRect entryRect = rect;

    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(entryRect.contract(margin),
                       entryRect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(entryRect.contract(margin).contract(borderWidth), fillColor);
    }
    
    EGRect controlRect = rect;
    controlRect.x += entryRect.width - CONTROL_WIDTH;
    controlRect.width = CONTROL_WIDTH;
    renderer->fill(controlRect.contract(margin),
                   controlRect.contract(margin).contract(borderWidth), strokeColor);
    renderer->fill(controlRect.contract(margin).contract(borderWidth), controlColor);
    
    upIcon->layout();
    downIcon->layout();
    
    EGPoint tp = getTextPosition();
    
    if (selectstartpos != selectendpos) {
        EGint selectstartx = xOffsetForChar(selectstartpos);
        EGint selectendx = xOffsetForChar(selectendpos);
        EGRect selectRect(tp.x + selectstartx, entryRect.y + margin.top + borderWidth + padding.top,
                          selectendx - selectstartx, entryRect.height - margin.top - margin.bottom - borderWidth * 2 - padding.top - padding.bottom);
        renderer->fill(selectRect, selectColor);
    }
    
    if (text) {
        renderer->paint(tp, text, textColor);
    }
    
    renderer->end();
    
    cursorRenderer->begin();
    EGint cursorposx = xOffsetForChar(cursorpos);
    EGLine cursorLine(EGPoint(tp.x + cursorposx, entryRect.y + margin.top + borderWidth + padding.top),
                      EGPoint(tp.x + cursorposx, entryRect.y + rect.height - margin.bottom - borderWidth - padding.bottom));
    cursorRenderer->stroke(cursorLine, textColor, 1);
    cursorRenderer->end();
    
    needsLayout = false;
}

void EWSpinBox::draw()
{
    EWEntry::draw();
    upIcon->draw();
    downIcon->draw();
}

EGbool EWSpinBox::mouseEvent(EGMouseEvent *evt)
{
    if (upIcon->mouseEvent(evt)) {
        requestFocus();
        return true;
    } else if (downIcon->mouseEvent(evt)) {
        requestFocus();
        return true;
    } else {
        return EWEntry::mouseEvent(evt);
    }
}

EGbool EWSpinBox::iconEvent(EWIconEvent *evt)
{
    if (evt->icon == upIcon.get()) {
        setValue(value + increment);
        return true;
    } else if (evt->icon == downIcon.get()) {
        setValue(value - increment);
        return true;
    }
    return false;
}

EGbool EWSpinBox::entryEvent(EWEntryEvent *evt)
{
    if (evt->entry == static_cast<EWEntry*>(this)) {
        setValue(atof(evt->text.c_str()));
        return true;
    }
    return false;
}

