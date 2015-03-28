/*
 *  EWColorPicker.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EWWidget.h"
#include "EWContainer.h"
#include "EWContext.h"
#include "EWWindow.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWFrame.h"
#include "EWGrid.h"
#include "EWLabel.h"
#include "EWSlider.h"
#include "EWSpinBox.h"
#include "EWColorPicker.h"

/* EWColorPickerEvent */

EGEventQualifier EWColorPickerEvent::VALUE_CHANGED   = "colorpicker-value-changed";
EGEventQualifier EWColorPickerEvent::CANCELLED   = "colorpicker-cancelled";

EWColorPickerEvent::EWColorPickerEvent(EGEventQualifier q, EWColorPicker *colorpicker, EGColor color)
    : EGEvent(q), colorpicker(colorpicker), color(color) {}

std::string EWColorPickerEvent::toString() {
    std::stringstream s;
    s << "[EWColorPickerEvent " << q << " colorpicker=" << colorpicker << " color=" << color.htmlColor().c_str() << "]";
    return s.str();
};


/* EWColorPicker */

static char class_name[] = "EWColorPicker";

EWColorPicker::EWColorPicker(EGenum widgetFlags) : EWWindow(widgetFlags), inEvent(false)
{
    setDefaultWidgetName();
    setDefaults(class_name);
    
    layoutGrid = EWGridPtr(new EWGrid());
    layoutGrid->setRowsHomogeneous(false);
    layoutGrid->setColumnsHomogeneous(false);
    layoutGrid->setRowsExpand(true);
    layoutGrid->setColumnsExpand(true);
    this->addWidget(layoutGrid);
    
    colorFrame = EWFramePtr(new EWFrame("Color"));
    layoutGrid->addWidget(colorFrame, 0, 0);
    colorSwatch = EWLabelPtr(new EWLabel());
    colorFrame->addWidget(colorSwatch);
    colorSwatch->setFillColor(color);
    colorSwatch->setFillBackground(true);
    colorSwatch->setPreferredSize(EGSize(0,32));

    rgbFrame = EWFramePtr(new EWFrame("RGB"));
    layoutGrid->addWidget(rgbFrame, 0, 1);
    
    rgbGrid = EWGridPtr(new EWGrid());
    rgbGrid->setRowsHomogeneous(false);
    rgbGrid->setColumnsHomogeneous(false);
    rgbGrid->setRowsExpand(false);
    rgbGrid->setColumnsExpand(false);
    rgbFrame->addWidget(rgbGrid);
    
    EWLabelPtr redLabel(new EWLabel("Red"));
    redLabel->setPreferredSize(EGSize(60,0));
    rgbGrid->addWidget(redLabel, 0, 0);
    redSlider = EWSliderPtr(new EWSlider());
    redSlider->setPreferredSize(EGSize(134,0));
    rgbGrid->addWidget(redSlider, 1, 0);
    redSpinBox = EWSpinBoxPtr(new EWSpinBox());
    redSpinBox->setPrecision(0);
    redSpinBox->setMinimum(0);
    redSpinBox->setMaximum(255);
    redSpinBox->setIncrement(1);
    redSpinBox->setPreferredSize(EGSize(60,0));
    rgbGrid->addWidget(redSpinBox, 2, 0);

    EWLabelPtr greenLabel(new EWLabel("Green"));
    greenLabel->setPreferredSize(EGSize(60,0));
    rgbGrid->addWidget(greenLabel, 0, 1);
    greenSlider = EWSliderPtr(new EWSlider());
    greenSlider->setPreferredSize(EGSize(134,0));
    rgbGrid->addWidget(greenSlider, 1, 1);
    greenSpinBox = EWSpinBoxPtr(new EWSpinBox());
    greenSpinBox->setPrecision(0);
    greenSpinBox->setMinimum(0);
    greenSpinBox->setMaximum(255);
    greenSpinBox->setIncrement(1);
    greenSpinBox->setPreferredSize(EGSize(60,0));
    rgbGrid->addWidget(greenSpinBox, 2, 1);

    EWLabelPtr blueLabel(new EWLabel("Blue"));
    blueLabel->setPreferredSize(EGSize(60,0));
    rgbGrid->addWidget(blueLabel, 0, 2);
    blueSlider = EWSliderPtr(new EWSlider());
    blueSlider->setPreferredSize(EGSize(134,0));
    rgbGrid->addWidget(blueSlider, 1, 2);
    blueSpinBox = EWSpinBoxPtr(new EWSpinBox());
    blueSpinBox->setPrecision(0);
    blueSpinBox->setIncrement(1);
    blueSpinBox->setMinimum(0);
    blueSpinBox->setMaximum(255);
    blueSpinBox->setPreferredSize(EGSize(60,0));
    rgbGrid->addWidget(blueSpinBox, 2, 2);

    hslFrame = EWFramePtr(new EWFrame("HSB"));
    layoutGrid->addWidget(hslFrame, 0, 2);
    
    hslGrid = EWGridPtr(new EWGrid());
    hslGrid->setRowsHomogeneous(false);
    hslGrid->setColumnsHomogeneous(false);
    hslGrid->setRowsExpand(false);
    hslGrid->setColumnsExpand(false);
    hslFrame->addWidget(hslGrid);
    
    EWLabelPtr hueLabel(new EWLabel("H °"));
    hueLabel->setPreferredSize(EGSize(60,0));
    hslGrid->addWidget(hueLabel, 0, 0);
    hueSlider = EWSliderPtr(new EWSlider());
    hueSlider->setPreferredSize(EGSize(134,0));
    hslGrid->addWidget(hueSlider, 1, 0);
    hueSpinBox = EWSpinBoxPtr(new EWSpinBox());
    hueSpinBox->setPrecision(0);
    hueSpinBox->setMinimum(0);
    hueSpinBox->setMaximum(360);
    hueSpinBox->setIncrement(1);
    hueSpinBox->setPreferredSize(EGSize(60,0));
    hslGrid->addWidget(hueSpinBox, 2, 0);
    
    EWLabelPtr saturationLabel(new EWLabel("S %"));
    saturationLabel->setPreferredSize(EGSize(60,0));
    hslGrid->addWidget(saturationLabel, 0, 1);
    saturationSlider = EWSliderPtr(new EWSlider());
    saturationSlider->setPreferredSize(EGSize(134,0));
    hslGrid->addWidget(saturationSlider, 1, 1);
    saturationSpinBox = EWSpinBoxPtr(new EWSpinBox());
    saturationSpinBox->setPrecision(1);
    saturationSpinBox->setMinimum(0);
    saturationSpinBox->setMaximum(100);
    saturationSpinBox->setIncrement(1);
    saturationSpinBox->setPreferredSize(EGSize(60,0));
    hslGrid->addWidget(saturationSpinBox, 2, 1);
    
    EWLabelPtr brightnessLabel(new EWLabel("B %"));
    brightnessLabel->setPreferredSize(EGSize(60,0));
    hslGrid->addWidget(brightnessLabel, 0, 2);
    brightnessSlider = EWSliderPtr(new EWSlider());
    brightnessSlider->setPreferredSize(EGSize(134,0));
    hslGrid->addWidget(brightnessSlider, 1, 2);
    brightnessSpinBox = EWSpinBoxPtr(new EWSpinBox());
    brightnessSpinBox->setPrecision(1);
    brightnessSpinBox->setMinimum(0);
    brightnessSpinBox->setMaximum(100);
    brightnessSpinBox->setIncrement(1);
    brightnessSpinBox->setPreferredSize(EGSize(60,0));
    hslGrid->addWidget(brightnessSpinBox, 2, 2);

    buttonGrid = EWGridPtr(new EWGrid());
    buttonGrid->setRowsHomogeneous(false);
    buttonGrid->setColumnsHomogeneous(true);
    buttonGrid->setRowsExpand(false);
    buttonGrid->setColumnsExpand(true);
    layoutGrid->addWidget(buttonGrid, 0, 3);
    
    cancelButton = EWButtonPtr(new EWButton("Cancel"));
    buttonGrid->addWidget(cancelButton, 0, 0);
    okayButton = EWButtonPtr(new EWButton("Okay"));
    buttonGrid->addWidget(okayButton, 2, 0);

    EGEventManager::connect<EWSpinBoxEvent,EWColorPicker>(EWSpinBoxEvent::VALUE_CHANGED, this, &EWColorPicker::spinBoxEvent);
    EGEventManager::connect<EWSliderEvent,EWColorPicker>(EWSliderEvent::VALUE_CHANGED, this, &EWColorPicker::sliderEvent);
    EGEventManager::connect<EWButtonEvent,EWColorPicker>(EWButtonEvent::RELEASED, this, &EWColorPicker::buttonEvent);
}

EWColorPicker::~EWColorPicker() {}

const EGClass* EWColorPicker::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWColorPicker>()->extends<EWWindow>()
        ->property("color",     &EWColorPicker::getColor, &EWColorPicker::setColor)
        ->emitter("okay",       &EWColorPicker::okay)
        ->emitter("cancel",     &EWColorPicker::cancel);
    return clazz;
}

const EGClass* EWColorPicker::getClass() const { return classFactory(); }

const char* EWColorPicker::widgetTypeName() { return class_name; }

EGColor EWColorPicker::getColor() { return color; }

#define HSV_MIN3(x,y,z)  ((y) <= (z) ? ((x) <= (y) ? (x) : (y)) : ((x) <= (z) ? (x) : (z)))
#define HSV_MAX3(x,y,z)  ((y) >= (z) ? ((x) >= (y) ? (x) : (y)) : ((x) >= (z) ? (x) : (z)))

static void HSVtoRGB(EGfloat &r, EGfloat &g, EGfloat &b, EGfloat h, EGfloat s, EGfloat v)
{
    if (h < 0) h += 360;
    else if (h >= 360) h -= 360;
    
    h = (std::max)((std::min)(360.0f,h),0.0f);
    s = (std::max)((std::min)(1.0f,s),0.0f);
    v = (std::max)((std::min)(1.0f,v),0.0f);
    
    if (s == 0) {
        r = g = b = v;
        return;
    }
    
    h /= 60;
	EGint i = (EGint)floor(h) % 6;
	EGfloat f = h - i;
	EGfloat p = v * ( 1 - s );
	EGfloat q = v * ( 1 - s * f );
	EGfloat t = v * ( 1 - s * ( 1 - f ) );

    p = (std::max)((std::min)(1.0f,p),0.0f);
    q = (std::max)((std::min)(1.0f,q),0.0f);
    t = (std::max)((std::min)(1.0f,t),0.0f);

    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }
}

static void RGBtoHSV(EGfloat &h, EGfloat &s, EGfloat &v, EGfloat r, EGfloat g, EGfloat b)
{
    r = (std::max)((std::min)(1.0f,r),0.0f);
    g = (std::max)((std::min)(1.0f,g),0.0f);
    b = (std::max)((std::min)(1.0f,b),0.0f);
    
    EGfloat rgb_min = HSV_MIN3(r, g, b);
    EGfloat rgb_max = HSV_MAX3(r, g, b);
    
    v = rgb_max;
	EGfloat delta = rgb_max - rgb_min;
    
	if (rgb_max != 0) {
		s = delta / rgb_max;
	} else {
		s = 0;
		h = 0;
		return;
	}
    
    EGfloat i = 0;
    if (delta == 0) {
        i = 0;
    } else if (r == rgb_max) {
		i = ( g - b ) / delta;
	} else if( g == rgb_max) {
		i = 2 + ( b - r ) / delta;
	} else if( b == rgb_max) {
		i = 4 + ( r - g ) / delta;
    }
    
    h = 360.0f - i * 60.0f;
    if (h < 0) h += 360;
    else if (h >= 360) h -= 360;
}

void EWColorPicker::setColorInternal(EGColor color, EWWidgetPtr updateWidget)
{
    if (this->color != color) {
        this->color = color;
        colorSwatch->setFillColor(color);
        
        inEvent = true;
        
        if (updateWidget != redSlider) redSlider->setValue(color.red);
        if (updateWidget != redSpinBox) redSpinBox->setValue(roundf(color.red * 255.0f));
        if (updateWidget != greenSlider) greenSlider->setValue(color.green);
        if (updateWidget != greenSpinBox) greenSpinBox->setValue(roundf(color.green * 255.0f));
        if (updateWidget != blueSlider) blueSlider->setValue(color.blue);
        if (updateWidget != blueSpinBox) blueSpinBox->setValue(roundf(color.blue * 255.0f));
        
        EGfloat h, s, v;
        RGBtoHSV(h, s, v, color.red, color.blue, color.green);
        
        if (updateWidget != hueSlider) hueSlider->setValue(h / 360.0f);
        if (updateWidget != hueSpinBox) hueSpinBox->setValue(h);
        if (updateWidget != saturationSlider) saturationSlider->setValue(s);
        if (updateWidget != saturationSpinBox) saturationSpinBox->setValue(s * 100.0f);
        if (updateWidget != brightnessSlider) brightnessSlider->setValue(v);
        if (updateWidget != brightnessSpinBox) brightnessSpinBox->setValue(v * 100.0f);
        
        inEvent = false;
        
        setNeedsLayout();
    }
}

void EWColorPicker::setColor(EGColor color)
{
    setColorInternal(color);
}

void EWColorPicker::okay()
{
    emit(&EWColorPicker::okay);
}

void EWColorPicker::cancel()
{
    emit(&EWColorPicker::cancel);
}

EGbool EWColorPicker::sliderEvent(EWSliderEvent *event)
{
    if (inEvent) return true;
    
    EGColor color = this->color;
    
    if (event->slider == redSlider.get()) {
        color.red = redSlider->getValue();
        setColorInternal(color, redSlider);
    } else if (event->slider == greenSlider.get()) {
        color.green = greenSlider->getValue();
        setColorInternal(color, greenSlider);
    } else if (event->slider == blueSlider.get()) {
        color.blue = blueSlider->getValue();
        setColorInternal(color, blueSlider);
    } else if (event->slider == hueSlider.get()) {
        HSVtoRGB(color.red, color.green, color.blue, hueSlider->getValue() * 360.0f, saturationSlider->getValue(), brightnessSlider->getValue());
        setColorInternal(color, hueSlider);
    } else if (event->slider == saturationSlider.get()) {
        HSVtoRGB(color.red, color.green, color.blue, hueSlider->getValue() * 360.0f, saturationSlider->getValue(), brightnessSlider->getValue());
        setColorInternal(color, saturationSlider);
    } else if (event->slider == brightnessSlider.get()) {
        HSVtoRGB(color.red, color.green, color.blue, hueSlider->getValue() * 360.0f, saturationSlider->getValue(), brightnessSlider->getValue());
        setColorInternal(color, brightnessSlider);
    }
    return false;
}

EGbool EWColorPicker::spinBoxEvent(EWSpinBoxEvent *event)
{
    if (inEvent) return true;
    
    EGColor color = this->color;

    if (event->spinbox == redSpinBox.get()) {
        color.red = redSpinBox->getValue() / 255.0f;
        setColorInternal(color, redSpinBox);
    } else if (event->spinbox == greenSpinBox.get()) {
        color.green = greenSpinBox->getValue() / 255.0f;
        setColorInternal(color, greenSpinBox);
    } else if (event->spinbox == blueSpinBox.get()) {
        color.blue = blueSpinBox->getValue() / 255.0f;
        setColorInternal(color, blueSpinBox);
    } else if (event->spinbox == hueSpinBox.get()) {
        hueSlider->setValue(hueSpinBox->getValue() / 360.0f);
        HSVtoRGB(color.red, color.green, color.blue, hueSlider->getValue() * 360.0f, saturationSlider->getValue(), brightnessSlider->getValue());
        setColorInternal(color, hueSpinBox);
    } else if (event->spinbox == saturationSpinBox.get()) {
        saturationSlider->setValue(saturationSpinBox->getValue() / 100.0f);
        HSVtoRGB(color.red, color.green, color.blue, hueSlider->getValue() * 360.0f, saturationSlider->getValue(), brightnessSlider->getValue());
        setColorInternal(color, saturationSpinBox);
    } else if (event->spinbox == brightnessSpinBox.get()) {
        brightnessSlider->setValue(brightnessSpinBox->getValue() / 100.0f);
        HSVtoRGB(color.red, color.green, color.blue, hueSlider->getValue() * 360.0f, saturationSlider->getValue(), brightnessSlider->getValue());
        setColorInternal(color, brightnessSpinBox);
    }
    return false;
}

EGbool EWColorPicker::buttonEvent(EWButtonEvent *event)
{
    if (event->button == okayButton.get()) {
        EWColorPickerEvent evt(EWColorPickerEvent::VALUE_CHANGED, this, color);
        EGEventManager::postEvent(&evt);
        okay();
        return true;
    } else if (event->button == cancelButton.get()) {
        EWColorPickerEvent evt(EWColorPickerEvent::CANCELLED, this, color);
        EGEventManager::postEvent(&evt);
        cancel();
        return true;
    }
    return false;
}
