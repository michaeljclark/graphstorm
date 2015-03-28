/*
 *  EGGraphAppUI.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphAppUI_H
#define EGGraphAppUI_H

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
#include "EWColorPicker.h"
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

class EGGraphApp;
class EGGraphAppUI;
typedef std::shared_ptr<EGGraphAppUI> EGGraphAppUIPtr;

class EGGraphAppUI
{
protected:
    friend class EGGraphApp;
    
    EGGraphApp *graphapp;
    EGSpringLayoutSettings settings;
    std::map<EGint,std::vector<EGint>> visibleDimensionCombos;
    volatile std::atomic<EGbool> running;
    
    EWContextPtr context;
    EWWindowPtr window;
    EWBoxPtr windowBox;
    
    EWFramePtr layoutFrame;
    EWGridPtr layoutGrid;
    EWSpinBoxPtr stiffnessSpinBox;
    EWSpinBoxPtr repulsionSpinBox;
    EWSpinBoxPtr centerAttractSpinBox;
    EWSpinBoxPtr dampingSpinBox;
    EWSpinBoxPtr timeStepSpinBox;
    EWSpinBoxPtr maxSpeedSpinBox;
    EWSpinBoxPtr stopEnergySpinBox;
    EWComboBoxPtr layoutDimensionsComboBox;
    EWComboBoxPtr algorithmComboBox;
    EWComboBoxPtr openCLDeviceComboBox;
    EWCheckBoxPtr openCLEnabledCheckBox;
    
    EWFramePtr modifiersFrame;
    EWGridPtr modifiersGrid;
    EWComboBoxPtr nodeMassComboBox;
    EWComboBoxPtr nodeSizeComboBox;
    EGColor nodeColor;
    EWBoxPtr nodeColorBox;
    EWComboBoxPtr nodeColorComboBox;
    EWButtonPtr nodeColorButton;
    EGColor edgeColor;
    EWBoxPtr edgeColorBox;
    EWComboBoxPtr edgeColorComboBox;
    EWButtonPtr edgeColorButton;
    
    EWFramePtr defaultsFrame;
    EWGridPtr defaultsGrid;
    EWComboBoxPtr viewDimensionsComboBox;
    EWComboBoxPtr visibleDimensionsComboBox;
    EWComboBoxPtr nodeShapeComboBox;
    EWSpinBoxPtr nodeWidthSpinBox;
    EWCheckBoxPtr nodeLabelsCheckBox;
    EWSpinBoxPtr edgeThicknessSpinBox;
    EWCheckBoxPtr edgeLabelsCheckBox;
    
    EWFramePtr controllerFrame;
    EWGridPtr controllerGrid;
    EWComboBoxPtr graphComboBox;
    EWBoxPtr buttonBox;
    EWButtonPtr startStopButton;
    EWButtonPtr resetButton;

    EWColorPickerPtr colorPicker;
    EWWidgetPtr colorPickerOwner;

public:
    EGGraphAppUI(EGGraphApp* graphapp);
    virtual ~EGGraphAppUI();
    
    void setup();
    void reset();
    void startStop();
    void settingsChanged();
    void showColorPicker(EWWidgetPtr colorPickerOwner);
    void colorPickerOkay();
    void colorPickerCancel();
    EGbool isRunning();
    EGGraphLayoutFactory* getLayoutFactory();
    EGSpringLayoutSettings& getSettings();
    void populateOpenCLDevices();
    void updateViewDimensions();
    void updateLayoutData();
    
    void setNeedsLayout();
    void draw();
};

#endif
