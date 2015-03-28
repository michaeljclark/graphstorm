/*
 *  EGGraphStormWidgets.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphStormWidgets_H
#define EGGraphStormWidgets_H

#include "EG.h"
#include "EGGL.h"
#include "EGMath.h"
#include "EGColor.h"
#include "EGText.h"
#include "EGImage.h"
#include "EGResource.h"
#include "EGRenderApi.h"
#include "EGRenderApiES2.h"
#include "EGViewport.h"

#include "EWWidget.h"
#include "EWContainer.h"
#include "EWContext.h"
#include "EWBox.h"
#include "EWButton.h"
#include "EWCheckBox.h"
#include "EWComboBox.h"
#include "EWEntry.h"
#include "EWFrame.h"
#include "EWGrid.h"
#include "EWIcon.h"
#include "EWLabel.h"
#include "EWLayout.h"
#include "EWList.h"
#include "EWProgressBar.h"
#include "EWSlider.h"
#include "EWSpinner.h"
#include "EWSpinBox.h"
#include "EWToggleButton.h"
#include "EWWindow.h"

class EGGraphStormWidgets;
typedef std::shared_ptr<EGGraphStormWidgets> EGGraphStormWidgetsPtr;


/* EGGraphStormWidgets */

class EGGraphStormWidgets
{
public:
    EGRenderApi *gl;
    EGViewportPtr viewport;
    EWContextPtr context;
    EWLabelPtr statusLabel;
    EWProgressBarPtr progressBar;
    
    EGGraphStormWidgets();
    virtual ~EGGraphStormWidgets();
    
    void init();
    void createWidgets();
    void reshape(int width, int height);
    void draw();
    
    EGbool spinBoxEvent(EWSpinBoxEvent *evt);
    EGbool sliderEvent(EWSliderEvent *evt);
    EGbool buttonEvent(EWButtonEvent *evt);
    EGbool iconEvent(EWIconEvent *evt);
    EGbool listEvent(EWListEvent *evt);
    EGbool checkBoxEvent(EWCheckBoxEvent *evt);
    EGbool comboBoxEvent(EWComboBoxEvent *evt);
    EGbool toggleButtonEvent(EWToggleButtonEvent *evt);
    EGbool windowEvent(EWWindowEvent *evt);
};

#endif
