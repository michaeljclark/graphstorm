/*
 *  EWColorPicker.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWColorPicker_H
#define EWColorPicker_H

#include "EWWidget.h"
#include "EWContainer.h"
#include "EWWindow.h"
#include "EWGrid.h"
#include "EWButton.h"
#include "EWFrame.h"
#include "EWLabel.h"
#include "EWSlider.h"
#include "EWSpinBox.h"

class EWColorPicker;
typedef std::shared_ptr<EWColorPicker> EWColorPickerPtr;

/* EWColorPickerEvent */

class EWColorPickerEvent : public EGEvent
{
public:
    static EGEventQualifier VALUE_CHANGED;
    static EGEventQualifier CANCELLED;
    
    typedef EWColorPicker emitter;
    
    EWColorPicker *colorpicker;
    EGColor color;
    
    EWColorPickerEvent(EGEventQualifier q, EWColorPicker *colorpicker, EGColor color);
    
    std::string toString();
};

/* EWColorPicker */

class EWColorPicker : public EWWindow
{
protected:
    EGColor color;
    EWGridPtr layoutGrid;

    EWFramePtr colorFrame;
    EWLabelPtr colorSwatch;

    EWFramePtr rgbFrame;
    EWGridPtr rgbGrid;
    EWSliderPtr redSlider;
    EWSliderPtr greenSlider;
    EWSliderPtr blueSlider;
    EWSpinBoxPtr redSpinBox;
    EWSpinBoxPtr greenSpinBox;
    EWSpinBoxPtr blueSpinBox;
    
    EWFramePtr hslFrame;
    EWGridPtr hslGrid;
    EWSliderPtr hueSlider;
    EWSliderPtr saturationSlider;
    EWSliderPtr brightnessSlider;
    EWSpinBoxPtr hueSpinBox;
    EWSpinBoxPtr saturationSpinBox;
    EWSpinBoxPtr brightnessSpinBox;
    
    EWGridPtr buttonGrid;
    EWButtonPtr okayButton;
    EWButtonPtr cancelButton;
    
    EGbool inEvent;
    
    void setColorInternal(EGColor color, EWWidgetPtr updateWidget = EWWidgetPtr());

public:
    EWColorPicker(EGenum widgetFlags = 0);
    virtual ~EWColorPicker();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();
    
    virtual EGColor getColor();
    virtual void setColor(EGColor color);
    
    virtual void okay(); /* emmitter */
    virtual void cancel(); /* emmitter */
    
    EGbool sliderEvent(EWSliderEvent *event);
    EGbool spinBoxEvent(EWSpinBoxEvent *event);
    EGbool buttonEvent(EWButtonEvent *event);
};

#endif
