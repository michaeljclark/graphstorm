/*
 *  EGGraphAppUI.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EGGraphApp.h"
#include "EGGraphAppUI.h"


/* EGGraphAppUI */

static const char class_name[] = "EGGraphAppUI";

EGGraphAppUI::EGGraphAppUI(EGGraphApp *graphapp) : graphapp(graphapp), running(false)
{
    setup();
    context->showAll();
}

EGGraphAppUI::~EGGraphAppUI() {}

void EGGraphAppUI::setup()
{
    nodeColor = EGColor(0.75f, 0.75f, 1.00f, 1);
    edgeColor = EGColor(0.75f, 0.75f, 0.75f, 1);
    
    context = EWContextPtr(new EWContext(graphapp->viewport));
    
    window = EWWindowPtr(new EWWindow());
    window->setPosition(EGPoint(8, 8));
    window->setPreferredSize(EGSize(256, 512));
    window->setHasDecoration(true);
    window->setDecorationBorderWidth(1);
    window->setBorderWidth(1);
    window->setHasTitle(true);
    window->setHasCloseBox(false);
    window->setMovable(true);
    window->setResizable(true);
    window->setTitle("GraphStorm");
    context->addWidget(window);
    
    windowBox = EWBoxPtr(new EWBox());
    windowBox->setVertical();
    windowBox->setExpand(true);
    window->addWidget(windowBox);
    
    
    //
    // Layout Settings
    //

    layoutFrame = EWFramePtr(new EWFrame("Layout Settings"));
    windowBox->addWidget(layoutFrame, 0);
    
    layoutGrid = EWGridPtr(new EWGrid());
    layoutGrid->setRowsHomogeneous(true);
    layoutGrid->setColumnsHomogeneous(true);
    layoutGrid->setRowsExpand(true);
    layoutGrid->setColumnsExpand(true);
    layoutFrame->addWidget(layoutGrid);
    
    // Stiffness
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("Stiffness")), 0, 0);
    stiffnessSpinBox = EWSpinBoxPtr(new EWSpinBox());
    stiffnessSpinBox->setValue(400.0);
    stiffnessSpinBox->setMinimumMaximumIncrementPrecision(0.0, 1000.0, 10.0, 1);
    stiffnessSpinBox->connect<EGfloat>("valueChanged", [&] (EGfloat) { settingsChanged(); });
    layoutGrid->addWidget(stiffnessSpinBox, 1, 0);
    
    // Repulsion
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("Repulsion")), 0, 1);
    repulsionSpinBox = EWSpinBoxPtr(new EWSpinBox());
    repulsionSpinBox->setValue(400.0);
    repulsionSpinBox->setMinimumMaximumIncrementPrecision(0.0, 1000.0, 10.0, 1);
    repulsionSpinBox->connect<EGfloat>("valueChanged", [&] (EGfloat) { settingsChanged(); });
    layoutGrid->addWidget(repulsionSpinBox, 1, 1);
    
    // Center Attract
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("Center Attract")), 0, 2);
    centerAttractSpinBox = EWSpinBoxPtr(new EWSpinBox());
    centerAttractSpinBox->setValue(0.0);
    centerAttractSpinBox->setMinimumMaximumIncrementPrecision(0.0, 100.0, 1.0, 2);
    centerAttractSpinBox->connect<EGfloat>("valueChanged", [&] (EGfloat) { settingsChanged(); });
    layoutGrid->addWidget(centerAttractSpinBox, 1, 2);
    
    // Damping
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("Damping")), 0, 3);
    dampingSpinBox = EWSpinBoxPtr(new EWSpinBox());
    dampingSpinBox->setValue(0.5);
    dampingSpinBox->setMinimumMaximumIncrementPrecision(0.05, 0.75, 0.05, 2);
    dampingSpinBox->connect<EGfloat>("valueChanged", [&] (EGfloat) { settingsChanged(); });
    layoutGrid->addWidget(dampingSpinBox, 1, 3);
    
    // Time Step
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("Time Step")), 0, 4);
    timeStepSpinBox = EWSpinBoxPtr(new EWSpinBox());
    timeStepSpinBox->setValue(0.03);
    timeStepSpinBox->setMinimumMaximumIncrementPrecision(0.01, 1.0, 0.01, 2);
    timeStepSpinBox->connect<EGfloat>("valueChanged", [&] (EGfloat) { settingsChanged(); });
    layoutGrid->addWidget(timeStepSpinBox, 1, 4);
    
    // Max Speed
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("Max Speed")), 0, 5);
    maxSpeedSpinBox = EWSpinBoxPtr(new EWSpinBox());
    maxSpeedSpinBox->setValue(1000.0);
    maxSpeedSpinBox->setMinimumMaximumIncrementPrecision(1.0, 10000.0, 100.0, 2);
    maxSpeedSpinBox->connect<EGfloat>("valueChanged", [&] (EGfloat) { settingsChanged(); });
    layoutGrid->addWidget(maxSpeedSpinBox, 1, 5);
    
    // Stop Energy
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("Stop Energy")), 0, 6);
    stopEnergySpinBox = EWSpinBoxPtr(new EWSpinBox());
    stopEnergySpinBox->setValue(0.01);
    stopEnergySpinBox->setMinimumMaximumIncrementPrecision(0.0, 1000.0, 0.001, 3);
    stopEnergySpinBox->connect<EGfloat>("valueChanged", [&] (EGfloat) { settingsChanged(); });
    layoutGrid->addWidget(stopEnergySpinBox, 1, 6);
    
    // Layout Dimensions
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("Layout Dimensions")), 0, 7);
    layoutDimensionsComboBox = EWComboBoxPtr(new EWComboBox());
    layoutDimensionsComboBox->addItem("2D");
    layoutDimensionsComboBox->addItem("3D");
    layoutDimensionsComboBox->addItem("4D");
    layoutDimensionsComboBox->setCurrentIndex(1);
    layoutDimensionsComboBox->connect<EGint>("currentIndexChanged", [&] (EGint index) {
        EGint nDim = layoutDimensionsComboBox->currentIndex() + 2;
        EGint viewDim = viewDimensionsComboBox->currentIndex() + 2;
        if (viewDim != (std::min)(nDim, 3)) {
            viewDim = (std::min)(nDim, 3);
            viewDimensionsComboBox->setCurrentIndex(viewDim - 2);
        }
        reset();
    });
    layoutGrid->addWidget(layoutDimensionsComboBox, 1, 7);
    
    // Algorithm
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("Algorithm")), 0, 8);
    algorithmComboBox = EWComboBoxPtr(new EWComboBox());
    algorithmComboBox->addItem("Spring Layout");
    algorithmComboBox->addItem("Spring Layout (Barnes-Hut)");
    algorithmComboBox->connect<EGint>("currentIndexChanged", [&] (EGint) { reset(); });
    layoutGrid->addWidget(algorithmComboBox, 1, 8);
    
    // OpenCL Device
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("OpenCL Device")), 0, 9);
    openCLDeviceComboBox = EWComboBoxPtr(new EWComboBox());
    layoutGrid->addWidget(openCLDeviceComboBox, 1, 9);
    
    // OpenCL Enabled
    layoutGrid->addWidget(EWLabelPtr(new EWLabel("OpenCL Enabled")), 0, 10);
    openCLEnabledCheckBox = EWCheckBoxPtr(new EWCheckBox());
#if USE_OPENCL
    openCLEnabledCheckBox->setValue(true);
#endif
    openCLEnabledCheckBox->connect<EGbool>("valueChanged", [&] (EGbool value) { reset(); });
    layoutGrid->addWidget(openCLEnabledCheckBox, 1, 10);
    
    
    //
    // Graph Modifiers
    //
    
    modifiersFrame = EWFramePtr(new EWFrame("Graph Modifiers"));
    windowBox->addWidget(modifiersFrame, 1);
    
    modifiersGrid = EWGridPtr(new EWGrid());
    modifiersGrid->setRowsHomogeneous(true);
    modifiersGrid->setColumnsHomogeneous(true);
    modifiersGrid->setRowsExpand(true);
    modifiersGrid->setColumnsExpand(true);
    modifiersFrame->addWidget(modifiersGrid);
    
    // Node Mass
    modifiersGrid->addWidget(EWLabelPtr(new EWLabel("Node Mass")), 0, 0);
    nodeMassComboBox = EWComboBoxPtr(new EWComboBox());
    nodeMassComboBox->addItem("User Defined");
    nodeMassComboBox->addItem("Constant");
    nodeMassComboBox->addItem("Degree");
    nodeMassComboBox->setCurrentIndex(1);
    nodeMassComboBox->connect<EGint>("currentIndexChanged", [&] (EGint index) {
        EGGraphNodeMassType nodeMassType = (EGGraphNodeMassType)index;
        EGGraphModifierList modifierList;
        modifierList.push_back(EGGraphModifierPtr(new EGGraphNodeMassModifier(nodeMassType, 1.0f)));
        EGGraphModifier::modify(graphapp->graph, modifierList);
        updateLayoutData();
    });
    modifiersGrid->addWidget(nodeMassComboBox, 1, 0);
    
    // Node Size
    modifiersGrid->addWidget(EWLabelPtr(new EWLabel("Node Size")), 0, 1);
    nodeSizeComboBox = EWComboBoxPtr(new EWComboBox());
    nodeSizeComboBox->addItem("User Defined");
    nodeSizeComboBox->addItem("Constant");
    nodeSizeComboBox->addItem("log(Degree)");
    nodeSizeComboBox->setCurrentIndex(1);
    nodeSizeComboBox->connect<EGint>("currentIndexChanged", [&] (EGint index) {
        EGGraphNodeSizeType nodeSizeType = (EGGraphNodeSizeType)index;
        EGGraphModifierList modifierList;
        modifierList.push_back(EGGraphModifierPtr(new EGGraphNodeSizeModifier(nodeSizeType, 1.0f)));
        EGGraphModifier::modify(graphapp->graph, modifierList);
        updateLayoutData();
    });
    modifiersGrid->addWidget(nodeSizeComboBox, 1, 1);

    // Node Color
    modifiersGrid->addWidget(EWLabelPtr(new EWLabel("Node Color")), 0, 2);
    nodeColorBox = EWBoxPtr(new EWBox());
    nodeColorBox->setHorizontal();
    nodeColorBox->setExpand(true);
    modifiersGrid->addWidget(nodeColorBox, 1, 2);
    nodeColorComboBox = EWComboBoxPtr(new EWComboBox());
    nodeColorComboBox->addItem("User Defined");
    nodeColorComboBox->addItem("Constant");
    nodeColorComboBox->addItem("Degree");
    nodeColorComboBox->setCurrentIndex(1);
    nodeColorComboBox->connect<EGint>("currentIndexChanged", [&] (EGint index) {
        EGGraphNodeColorType nodeColorType = (EGGraphNodeColorType)index;
        switch (nodeColorType) {
            case EGGraphNodeColorTypeConstant:
                nodeColorButton->setEnabled(true);
                break;
            default:
                nodeColorButton->setEnabled(false);
                break;
        }
        EGGraphModifierList modifierList;
        modifierList.push_back(EGGraphModifierPtr(new EGGraphNodeColorModifier(nodeColorType, nodeColor)));
        EGGraphModifier::modify(graphapp->graph, modifierList);
        updateLayoutData();
    });
    nodeColorBox->addWidget(nodeColorComboBox, 0);
    nodeColorButton = EWButtonPtr(new EWButton());
    nodeColorButton->setPreferredSize(EGSize(20, 20));
    nodeColorButton->setFillColor(nodeColor);
    nodeColorButton->setActiveFillColor(nodeColor);
    nodeColorButton->connect("clicked", [&] () { showColorPicker(nodeColorButton); });
    nodeColorBox->addWidget(nodeColorButton, 1);
    
    // Edge Color
    modifiersGrid->addWidget(EWLabelPtr(new EWLabel("Edge Color")), 0, 3);
    edgeColorBox = EWBoxPtr(new EWBox());
    edgeColorBox->setHorizontal();
    edgeColorBox->setExpand(true);
    modifiersGrid->addWidget(edgeColorBox, 1, 3);
    edgeColorComboBox = EWComboBoxPtr(new EWComboBox());
    edgeColorComboBox->addItem("User Defined");
    edgeColorComboBox->addItem("Constant");
    edgeColorComboBox->addItem("Length");
    edgeColorComboBox->setCurrentIndex(1);
    edgeColorComboBox->connect<EGint>("currentIndexChanged", [&] (EGint index) {
        EGGraphEdgeColorType edgeColorType = (EGGraphEdgeColorType)index;
        switch (edgeColorType) {
            case EGGraphEdgeColorTypeConstant:
                edgeColorButton->setEnabled(true);
                break;
            default:
                edgeColorButton->setEnabled(false);
                break;
        }
        EGGraphModifierList modifierList;
        modifierList.push_back(EGGraphModifierPtr(new EGGraphEdgeColorModifier(edgeColorType, edgeColor)));
        EGGraphModifier::modify(graphapp->graph, modifierList);
        updateLayoutData();
    });
    edgeColorBox->addWidget(edgeColorComboBox, 0);
    edgeColorButton = EWButtonPtr(new EWButton());
    edgeColorButton->setPreferredSize(EGSize(20, 20));
    edgeColorButton->setFillColor(edgeColor);
    edgeColorButton->setActiveFillColor(edgeColor);
    edgeColorButton->connect("clicked", [&] () { showColorPicker(edgeColorButton); });
    edgeColorBox->addWidget(edgeColorButton, 1);

    // Color Picker
    colorPicker = EWColorPickerPtr(new EWColorPicker());
    colorPicker->connect("okay", [&] () { colorPickerOkay(); });
    colorPicker->connect("cancel", [&] () { colorPickerCancel(); });

    
    //
    // Display Defaults
    //
    
    defaultsFrame = EWFramePtr(new EWFrame("Display Defaults"));
    windowBox->addWidget(defaultsFrame, 2);
    
    defaultsGrid = EWGridPtr(new EWGrid());
    defaultsGrid->setRowsHomogeneous(true);
    defaultsGrid->setColumnsHomogeneous(true);
    defaultsGrid->setRowsExpand(true);
    defaultsGrid->setColumnsExpand(true);
    defaultsFrame->addWidget(defaultsGrid);
    
    // View Dimensions
    defaultsGrid->addWidget(EWLabelPtr(new EWLabel("View Dimensions")), 0, 0);
    viewDimensionsComboBox = EWComboBoxPtr(new EWComboBox());
    viewDimensionsComboBox->addItem("2D");
    viewDimensionsComboBox->addItem("3D");
    viewDimensionsComboBox->setCurrentIndex(1);
    viewDimensionsComboBox->connect<EGint>("currentIndexChanged", [&] (EGint index) {
        updateViewDimensions();
    });
    defaultsGrid->addWidget(viewDimensionsComboBox, 1, 0);
    
    // Visible Dimensions
    defaultsGrid->addWidget(EWLabelPtr(new EWLabel("Visible Dimensions")), 0, 1);
    visibleDimensionsComboBox = EWComboBoxPtr(new EWComboBox());
    visibleDimensionsComboBox->connect<EGint>("currentIndexChanged", [&] (EGint index) {
        graphapp->layoutImpl->setVisibleDimensions(visibleDimensionCombos[index]);
    });
    defaultsGrid->addWidget(visibleDimensionsComboBox, 1, 1);
    
    // Node Shape
    defaultsGrid->addWidget(EWLabelPtr(new EWLabel("Node Shape")), 0, 2);
    nodeShapeComboBox = EWComboBoxPtr(new EWComboBox());
    nodeShapeComboBox->addItem("Point");
    nodeShapeComboBox->addItem("Cube");
    nodeShapeComboBox->setCurrentIndex(1);
    nodeShapeComboBox->connect<EGint>("currentIndexChanged", [&] (EGint index) {
        graphapp->layoutImpl->setDefaultNodeShape((EGGraphRendererParamNodeShape)index);
    });
    defaultsGrid->addWidget(nodeShapeComboBox, 1, 2);
    
    // Node Width
    defaultsGrid->addWidget(EWLabelPtr(new EWLabel("Node Width")), 0, 3);
    nodeWidthSpinBox = EWSpinBoxPtr(new EWSpinBox());
    nodeWidthSpinBox->setValue(10.0);
    nodeWidthSpinBox->setMinimumMaximumIncrementPrecision(1.0, 64.0, 1.0, 0);
    nodeWidthSpinBox->connect<EGfloat>("valueChanged", [&] (EGfloat value) {
        graphapp->layoutImpl->setDefaultNodePointSize(value);
    });
    defaultsGrid->addWidget(nodeWidthSpinBox, 1, 3);
    
    // Node Labels
    defaultsGrid->addWidget(EWLabelPtr(new EWLabel("Node Labels")), 0, 4);
    nodeLabelsCheckBox = EWCheckBoxPtr(new EWCheckBox());
    nodeLabelsCheckBox->connect<EGbool>("valueChanged", [&] (EGbool value) {
        graphapp->layoutImpl->setDefaultNodeShowLabels(value);
    });
    defaultsGrid->addWidget(nodeLabelsCheckBox, 1, 4);
    
    // Edge Thickness
    defaultsGrid->addWidget(EWLabelPtr(new EWLabel("Edge Thickness")), 0, 5);
    edgeThicknessSpinBox = EWSpinBoxPtr(new EWSpinBox());
    edgeThicknessSpinBox->setValue(1.0);
    edgeThicknessSpinBox->setMinimumMaximumIncrementPrecision(0.25, 8.0, 0.25, 2);
    edgeThicknessSpinBox->connect<EGfloat>("valueChanged", [&] (EGfloat value) {
        graphapp->layoutImpl->setDefaultEdgeLineWidth(value);
    });
    defaultsGrid->addWidget(edgeThicknessSpinBox, 1, 5);
    
    // Edge Labels
    defaultsGrid->addWidget(EWLabelPtr(new EWLabel("Edge Labels")), 0, 6);
    edgeLabelsCheckBox = EWCheckBoxPtr(new EWCheckBox());
    edgeLabelsCheckBox->connect<EGbool>("valueChanged", [&] (EGbool value) {
        graphapp->layoutImpl->setDefaultEdgeShowLabels(value);
    });
    defaultsGrid->addWidget(edgeLabelsCheckBox, 1, 6);
    
    
    //
    // Controller
    //

    controllerFrame = EWFramePtr(new EWFrame("Controller"));
    windowBox->addWidget(controllerFrame, 3);
    
    controllerGrid = EWGridPtr(new EWGrid());
    controllerGrid->setRowsHomogeneous(true);
    controllerGrid->setColumnsHomogeneous(true);
    controllerGrid->setRowsExpand(true);
    controllerGrid->setColumnsExpand(true);
    controllerFrame->addWidget(controllerGrid);
    
    // Graph
    controllerGrid->addWidget(EWLabelPtr(new EWLabel("Graph")), 0, 0);
    graphComboBox = EWComboBoxPtr(new EWComboBox());
    EGint i = 0;
    while (EGGraphTestImpl::testgraphs[i] != NULL) {
        EGstring name = EGGraphTestImpl::testgraphs[i]->name;
        graphComboBox->addItem(name);
        if (name == "Grid 16x16") {
            graphComboBox->setCurrentIndex(i);
        }
        i++;
    }
    graphComboBox->connect<EGint>("currentIndexChanged", [&] (EGint) { reset(); });
    controllerGrid->addWidget(graphComboBox, 1, 0);
    
    buttonBox = EWBoxPtr(new EWBox());
    buttonBox->setHorizontal();
    buttonBox->setExpand(true);
    controllerGrid->addWidget(buttonBox, 1, 1);
    
    // Start/Stop Button
    startStopButton = EWButtonPtr(new EWButton());
    startStopButton->setLabel("Start");
    startStopButton->connect("clicked", [&] () { startStop(); });
    buttonBox->addWidget(startStopButton, 0);
    resetButton = EWButtonPtr(new EWButton());
    resetButton->setLabel("Reset");
    resetButton->connect("clicked", [&] () { reset(); });
    buttonBox->addWidget(resetButton, 1);
}

void EGGraphAppUI::reset()
{
    running = false;
    EGGraphLayoutFactory *layoutFactory = getLayoutFactory();
    if (layoutFactory) {
        EGGraphTest *testGraph = EGGraphTestImpl::testgraphs[graphComboBox->currentIndex()];
        
        EGGraphModifierList modifierList;
        EGGraphNodeMassType nodeMassType = (EGGraphNodeMassType)nodeMassComboBox->currentIndex();
        modifierList.push_back(EGGraphModifierPtr(new EGGraphNodeMassModifier(nodeMassType, 1.0f)));
        EGGraphNodeSizeType nodeSizeType = (EGGraphNodeSizeType)nodeSizeComboBox->currentIndex();
        modifierList.push_back(EGGraphModifierPtr(new EGGraphNodeSizeModifier(nodeSizeType, 1.0f)));
        EGGraphEdgeColorType edgeColorType = (EGGraphEdgeColorType)edgeColorComboBox->currentIndex();
        modifierList.push_back(EGGraphModifierPtr(new EGGraphEdgeColorModifier(edgeColorType, edgeColor)));
        EGGraphNodeColorType nodeColorType = (EGGraphNodeColorType)nodeColorComboBox->currentIndex();
        modifierList.push_back(EGGraphModifierPtr(new EGGraphNodeColorModifier(nodeColorType, nodeColor)));
        
        getSettings();
        graphapp->setupGraph(settings, *testGraph, *layoutFactory, modifierList);
        
        graphapp->layoutImpl->setDefaultNodeShape((EGGraphRendererParamNodeShape)nodeShapeComboBox->currentIndex());
        graphapp->layoutImpl->setDefaultNodePointSize(nodeWidthSpinBox->getValue());
        graphapp->layoutImpl->setDefaultNodeShowLabels(nodeLabelsCheckBox->getValue());
        graphapp->layoutImpl->setDefaultNodeColor(nodeColor);
        graphapp->layoutImpl->setDefaultEdgeLineWidth(edgeThicknessSpinBox->getValue());
        graphapp->layoutImpl->setDefaultEdgeShowLabels(edgeLabelsCheckBox->getValue());
        graphapp->layoutImpl->setDefaultEdgeColor(edgeColor);
        updateViewDimensions();
    }
    startStopButton->setLabel("Start");
}

void EGGraphAppUI::startStop()
{
    running = !running;
    startStopButton->setLabel(isRunning() ? "Stop" : "Start");
}

void EGGraphAppUI::settingsChanged()
{
    graphapp->layoutImpl->updateSettings(getSettings());
}

void EGGraphAppUI::showColorPicker(EWWidgetPtr colorPickerOwner)
{
    if (this->colorPickerOwner || colorPicker->isVisible()) return;
    this->colorPickerOwner = colorPickerOwner;
    
    EGViewportPtr viewport = context->getViewport();
    EGSize colorPickerSize = colorPicker->calcPreferredSize();
    context->addWidget(colorPicker);
    colorPicker->setColor(colorPickerOwner->getFillColor());
    colorPicker->setPosition(EGPoint(viewport->screenWidth / 2 - colorPickerSize.width / 2,
                                     viewport->screenHeight / 2 - colorPickerSize.height / 2));
    colorPicker->showAll();
    colorPicker->requestFocus();
}

void EGGraphAppUI::colorPickerOkay()
{
    if (colorPickerOwner == edgeColorButton) {
        edgeColor = colorPicker->getColor();
        edgeColorButton->setFillColor(edgeColor);
        edgeColorButton->setActiveFillColor(edgeColor);
        EGGraphModifierList modifierList;
        EGGraphEdgeColorType edgeColorType = (EGGraphEdgeColorType)edgeColorComboBox->currentIndex();
        modifierList.push_back(EGGraphModifierPtr(new EGGraphEdgeColorModifier(edgeColorType, edgeColor)));
        EGGraphModifier::modify(graphapp->graph, modifierList);
    } else if (colorPickerOwner == nodeColorButton) {
        nodeColor = colorPicker->getColor();
        nodeColorButton->setFillColor(nodeColor);
        nodeColorButton->setActiveFillColor(nodeColor);
        EGGraphModifierList modifierList;
        EGGraphNodeColorType nodeColorType = (EGGraphNodeColorType)nodeColorComboBox->currentIndex();
        modifierList.push_back(EGGraphModifierPtr(new EGGraphNodeColorModifier(nodeColorType, nodeColor)));
        EGGraphModifier::modify(graphapp->graph, modifierList);
    }
    colorPickerOwner = EWWidgetPtr();
    context->removeWidget(colorPicker);
    colorPicker->hide();
    updateLayoutData();
}

void EGGraphAppUI::colorPickerCancel()
{
    colorPickerOwner = EWWidgetPtr();
    context->removeWidget(colorPicker);
    colorPicker->hide();
}

EGbool EGGraphAppUI::isRunning()
{
    return running && graphapp->layoutImpl->isRunning();
}

EGGraphLayoutFactory* EGGraphAppUI::getLayoutFactory()
{
    std::string algorithmName = algorithmComboBox->currentText();
#if USE_OPENCL
    if (openCLDeviceComboBox->itemCount() == 0) {
        populateOpenCLDevices();
    }
#else
    openCLEnabledCheckBox->setValue(false);
    openCLEnabledCheckBox->setEnabled(false);
    openCLDeviceComboBox->setEnabled(false);
#endif
    EGbool openCLenabled = openCLEnabledCheckBox->getValue();
    EGint nDim = layoutDimensionsComboBox->currentIndex() + 2;
    EGGraphLayoutFactory *layoutFactory = EGGraphLayoutFactory::getFactory(algorithmName, nDim, openCLenabled);
    EGDebug("%s:%s algorithmName=%s nDim=%d openCLenabled=%d layoutFactory=%p\n", class_name, __func__, algorithmName.c_str(), nDim, openCLenabled, layoutFactory);
    return layoutFactory;
}

EGSpringLayoutSettings& EGGraphAppUI::getSettings()
{
    settings.stiffness = (EGfloat)stiffnessSpinBox->getValue();
    settings.repulsion = (EGfloat)repulsionSpinBox->getValue();
    settings.centerAttraction = (EGfloat)centerAttractSpinBox->getValue();
    settings.damping = (EGfloat)dampingSpinBox->getValue();
    settings.timeStep = (EGfloat)timeStepSpinBox->getValue();
    settings.maxSpeed = (EGfloat)maxSpeedSpinBox->getValue();
    settings.maxEnergy = (EGfloat)stopEnergySpinBox->getValue();
    
    EGDebug("%s:%s %s\n", class_name, __func__, settings.toString().c_str());
    
    return settings;
}

void EGGraphAppUI::populateOpenCLDevices()
{
#if USE_OPENCL
    EGOpenCLDevicePtr currentDevice = graphapp->cldevices[0];
    EGOpenCLDeviceList &deviceList = graphapp->cldevices;
    EGint index = 0;
    for (EGOpenCLDeviceList::iterator di = deviceList.begin(); di != deviceList.end(); di++) {
        EGOpenCLDevicePtr &device = *di;
        openCLDeviceComboBox->addItem(device->getName().c_str());
        if (device == currentDevice) {
            openCLDeviceComboBox->setCurrentIndex(index);
        }
        index++;
    }
#endif
}

void EGGraphAppUI::updateViewDimensions()
{
    EGint nDim = layoutDimensionsComboBox->currentIndex() + 2;
    EGint viewDim = viewDimensionsComboBox->currentIndex() + 2;
    if (viewDim > nDim) {
        viewDim = nDim;
        viewDimensionsComboBox->setCurrentIndex(viewDim - 2);
    }
    
    // generate dimension combinations
    EGCombination visibleDimemsionComboGenerator(nDim, viewDim);
    visibleDimensionCombos.clear();
    visibleDimensionsComboBox->clear();
    EGint i = 0;
    while (visibleDimemsionComboGenerator.hasNext()) {
        std::vector<EGint> dc = visibleDimemsionComboGenerator.getNextVector();
        std::stringstream ss;
        for (std::vector<EGint>::iterator dci = dc.begin(); dci != dc.end(); dci++) {
            if (dci != dc.begin()) ss << ",";
            ss << *dci;
        }
        visibleDimensionsComboBox->addItem(ss.str().c_str());
        visibleDimensionCombos.insert(std::pair<EGint,std::vector<EGint>>(i, dc));
        i++;
    }
    
    // set view dimensions
    graphapp->layoutImpl->setViewDimensions(viewDim);
    graphapp->layoutImpl->setVisibleDimensions(visibleDimensionCombos[0]);
}

void EGGraphAppUI::updateLayoutData()
{
    // update layout data on GPU if required
    graphapp->layoutImpl->deleteArrays();
    graphapp->layoutImpl->createArrays();
}

void EGGraphAppUI::setNeedsLayout()
{
    context->setNeedsLayout();
}

void EGGraphAppUI::draw()
{
    context->draw();
}
