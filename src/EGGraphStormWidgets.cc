/*
 *  EGGraphStormWidgets.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */


#include "EGGraphStormWidgets.h"

/* EGGraphStormWidgets */

static const char class_name[] = "EGGraphStormWidgets";

EGGraphStormWidgets::EGGraphStormWidgets() :
    gl(NULL), viewport(new EGViewport()) {}

EGGraphStormWidgets::~EGGraphStormWidgets() {}

void EGGraphStormWidgets::init()
{
    // initialize render api
    const EGint glesVersion = 2;
    EGRenderApi::initRenderApi(glesVersion);
    gl = &EGRenderApi::currentApiImpl();
    
    // create widgets
    context = EWContextPtr(new EWContext(viewport));
    createWidgets();
}

void EGGraphStormWidgets::createWidgets()
{
    EWWindowPtr w1(new EWWindow());
    w1->setPosition(EGPoint(8, 8));
    w1->setPreferredSize(EGSize(80, 160));
    w1->setHasDecoration(true);
    w1->setMovable(true);
    w1->setResizable(true);
    w1->setTitle("VBox Test");
    context->addWidget(w1);
    
    EWBoxPtr b1(new EWBox());
    b1->setVertical();
    b1->setHomogeneous(true);
    b1->setExpand(true);
    w1->addWidget(b1);
    
    EWLabelPtr l1(new EWLabel());
    l1->setLabel("Tom");
    l1->getText()->setFlags(EGTextHAlignLeft | EGTextVAlignTop);
    l1->setMargin(EGMargin(2,2,2,2));
    l1->setBorderWidth(1);
    l1->setPadding(EGPadding(2,2,2,2));
    l1->setStrokeColor(EGColor(1,1,1,1));
    l1->setFillColor(EGColor(1,0.5,0.5,1));
    l1->setFillBackground(true);
    l1->setStrokeBorder(true);
    b1->addWidget(l1, 0);
    
    EWLabelPtr l2(new EWLabel());
    l2->setLabel("Dick");
    l2->getText()->setFlags(EGTextHAlignCenter | EGTextVAlignCenter);
    l2->setMargin(EGMargin(2,2,2,2));
    l2->setBorderWidth(1);
    l2->setPadding(EGPadding(2,2,2,2));
    l2->setStrokeColor(EGColor(1,1,1,1));
    l2->setFillColor(EGColor(0.5,1,0.5,1));
    l2->setFillBackground(true);
    l2->setStrokeBorder(true);
    b1->addWidget(l2, 1);
    
    EWLabelPtr l3(new EWLabel());
    l3->setLabel("Harry");
    l3->getText()->setFlags(EGTextHAlignRight | EGTextVAlignBase);
    l3->setMargin(EGMargin(2,2,2,2));
    l3->setBorderWidth(1);
    l3->setPadding(EGPadding(2,2,2,2));
    l3->setStrokeColor(EGColor(1,1,1,1));
    l3->setFillColor(EGColor(0.5,0.5,1,1));
    l3->setFillBackground(true);
    l3->setStrokeBorder(true);
    b1->addWidget(l3, 2);

    EWWindowPtr w2(new EWWindow());
    w2->setPosition(EGPoint(8, 184));
    w2->setPreferredSize(EGSize(160, 80));
    w2->setHasDecoration(true);
    w2->setMovable(true);
    w2->setResizable(true);
    w2->setTitle("HBox Test");
    context->addWidget(w2);
    
    EWBoxPtr b2(new EWBox());
    b2->setHorizontal();
    b2->setHomogeneous(true);
    b2->setExpand(true);
    w2->addWidget(b2);
    
    EWLabelPtr l4(new EWLabel());
    l4->setLabel("Tom");
    l4->getText()->setFlags(EGTextHAlignLeft | EGTextVAlignTop);
    l4->setMargin(EGMargin(2,2,2,2));
    l4->setBorderWidth(1);
    l4->setPadding(EGPadding(2,2,2,2));
    l4->setStrokeColor(EGColor(1,1,1,1));
    l4->setFillColor(EGColor(1,0.5,0.5,1));
    l4->setFillBackground(true);
    l4->setStrokeBorder(true);
    b2->addWidget(l4, 0);
    
    EWLabelPtr l5(new EWLabel());
    l5->setLabel("Dick");
    l5->getText()->setFlags(EGTextHAlignCenter | EGTextVAlignCenter);
    l5->setMargin(EGMargin(2,2,2,2));
    l5->setBorderWidth(1);
    l5->setPadding(EGPadding(2,2,2,2));
    l5->setStrokeColor(EGColor(1,1,1,1));
    l5->setFillColor(EGColor(0.5,1,0.5,1));
    l5->setFillBackground(true);
    l5->setStrokeBorder(true);
    b2->addWidget(l5, 1);
    
    EWLabelPtr l6(new EWLabel());
    l6->setLabel("Harry");
    l6->getText()->setFlags(EGTextHAlignRight | EGTextVAlignBase);
    l6->setMargin(EGMargin(2,2,2,2));
    l6->setBorderWidth(1);
    l6->setPadding(EGPadding(2,2,2,2));
    l6->setStrokeColor(EGColor(1,1,1,1));
    l6->setFillColor(EGColor(0.5,0.5,1,1));
    l6->setFillBackground(true);
    l6->setStrokeBorder(true);
    b2->addWidget(l6, 2);

    EWWindowPtr w3(new EWWindow());
    w3->setPosition(EGPoint(188, 8));
    w3->setPreferredSize(EGSize(256, 256));
    w3->setHasDecoration(true);
    w3->setMovable(true);
    w3->setResizable(true);
    w3->setTitle("Grid Test");
    context->addWidget(w3);
    
    EWGridPtr g1(new EWGrid());
    g1->setRowsHomogeneous(true);
    g1->setColumnsHomogeneous(true);
    g1->setRowsExpand(true);
    g1->setColumnsExpand(true);
    w3->addWidget(g1);
    
    for (EGint x = 0; x < 5; x++) {
        for (EGint y = 0; y < 5; y++) {
            EWLabelPtr l(new EWLabel());
            std::stringstream ss;
            ss << x << "-" << y;
            l->setLabel(ss.str());
            l->getText()->setFlags(EGTextHAlignCenter | EGTextVAlignCenter);
            l->getText()->setFontStyle(EGFontStyleBoldItalic);
            l->setMargin(EGMargin(2,2,2,2));
            l->setBorderWidth(1);
            l->setPadding(EGPadding(2,2,2,2));
            l->setStrokeColor(EGColor(1,1,1,1));
            l->setFillColor(EGColor(0.8,0.8,0.8,1));
            l->setFillBackground(true);
            l->setStrokeBorder(true);
            g1->addWidget(l, x, y);
        }
    }
    
    EWWindowPtr w4(new EWWindow());
    w4->setPosition(EGPoint(460, 8));
    w4->setPreferredSize(EGSize(200, 200));
    w4->setHasDecoration(true);
    w4->setMovable(true);
    w4->setResizable(true);
    w4->setTitle("Widget Test");
    context->addWidget(w4);
    
    EWBoxPtr b3(new EWBox());
    b3->setVertical();
    b3->setExpand(true);
    w4->addWidget(b3);
    
    EWFramePtr f1(new EWFrame());
    f1->setLabel("Controls");
    b3->addWidget(f1, 0);

    EWFramePtr f2(new EWFrame());
    f2->setLabel("Status");
    b3->addWidget(f2, 1);
    
    statusLabel = EWLabelPtr(new EWLabel());
    statusLabel->setLabel("none");
    f2->addWidget(statusLabel);

    EWGridPtr g2(new EWGrid());
    g2->setColumnsExpand(true);
    f1->addWidget(g2);
    
    EGint v = 0;
    
    // entry
    EWLabelPtr entrylabel1(new EWLabel);
    entrylabel1->setLabel("Entry");
    g2->addWidget(entrylabel1, 0, v);
    
    EWEntryPtr entry1(new EWEntry());
    entry1->setString("test");
    g2->addWidget(entry1, 1, v++);
    
    // spinbox
    EWLabelPtr spinboxlabel1(new EWLabel);
    spinboxlabel1->setLabel("Spin Box");
    g2->addWidget(spinboxlabel1, 0, v);
    
    EWSpinBoxPtr spinbox1(new EWSpinBox());
    spinbox1->setValue(0.1f);
    spinbox1->setIncrement(0.1f);
    spinbox1->setPrecision(5);
    g2->addWidget(spinbox1, 1, v++);

    // list
    EWLabelPtr wl2(new EWLabel);
    wl2->setLabel("List");
    g2->addWidget(wl2, 0, v);
    
    EWListPtr lsw1(new EWList());
    lsw1->addItem("Item 1");
    lsw1->addItem("Item 2");
    g2->addWidget(lsw1, 1, v++);

    // check box
    EWLabelPtr checkboxlabel1(new EWLabel);
    checkboxlabel1->setLabel("Check box");
    g2->addWidget(checkboxlabel1, 0, v);
    
    EWCheckBoxPtr checkbox1(new EWCheckBox());
    checkbox1->setValue(true);
    g2->addWidget(checkbox1, 1, v++);
    
    // toggle button
    EWLabelPtr togglelabel1(new EWLabel);
    togglelabel1->setLabel("Toggle Button");
    g2->addWidget(togglelabel1, 0, v);
    
    EWToggleButtonPtr toggle1(new EWToggleButton());
    toggle1->setValue(true);
    g2->addWidget(toggle1, 1, v++);
    
    // combo box
    EWLabelPtr comboboxlabel1(new EWLabel);
    comboboxlabel1->setLabel("Combo box");
    g2->addWidget(comboboxlabel1, 0, v);

    EWComboBoxPtr combobox1(new EWComboBox());
    combobox1->addItem("Item One");
    combobox1->addItem("Item Two");
    combobox1->addItem("Item Three");
    g2->addWidget(combobox1, 1, v++);

    // slider
    EWLabelPtr sliderlabel1(new EWLabel);
    sliderlabel1->setLabel("Slider");
    g2->addWidget(sliderlabel1, 0, v);
    
    EWSliderPtr slider1(new EWSlider());
    slider1->setValue(0.5f);
    g2->addWidget(slider1, 1, v++);

    // button
    EWLabelPtr buttonlabel1(new EWLabel);
    buttonlabel1->setLabel("Button");
    g2->addWidget(buttonlabel1, 0, v);
    
    EWButtonPtr button1(new EWButton());
    button1->setLabel("Click");
    g2->addWidget(button1, 1, v++);

    // spinner
    EWLabelPtr spinnerlabel1(new EWLabel);
    spinnerlabel1->setLabel("Spinner");
    g2->addWidget(spinnerlabel1, 0, v);
    
    EWSpinnerPtr spinner1(new EWSpinner());
    g2->addWidget(spinner1, 1, v++);

    // icon
    EWLabelPtr iconlabel1(new EWLabel);
    iconlabel1->setLabel("Icon");
    g2->addWidget(iconlabel1, 0, v);
    
    EWIconPtr icon1(new EWIcon());
    icon1->setImage(EGImage::createFromResource("Resources/ui.bundle/green-checkmark-up.png"));
    icon1->setActiveImage(EGImage::createFromResource("Resources/ui.bundle/green-checkmark-down.png"));
    icon1->setImageAlign(EWWidgetFlagsImageHAlignCenter | EWWidgetFlagsImageVAlignCenter);
    g2->addWidget(icon1, 1, v++);

	// progress bar
    EWLabelPtr progresslabel1(new EWLabel);
    progresslabel1->setLabel("Progress Bar");
    g2->addWidget(progresslabel1, 0, v);
    
    progressBar = EWProgressBarPtr(new EWProgressBar());
    progressBar->setProgress(slider1->getValue());
    g2->addWidget(progressBar, 1, v++);
    
    EWWindowPtr w5(new EWWindow());
    w5->setPosition(EGPoint(8, 280));
    w5->setPreferredSize(EGSize(430, 280));
    w5->setHasDecoration(true);
    w5->setMovable(true);
    w5->setResizable(true);
    w5->setTitle("Scroll Test");
    w5->setHasVScrollBar(true);
    context->addWidget(w5);

    // connect event handlers
    EGEventManager::connect<EWSpinBoxEvent,EGGraphStormWidgets>(EWSpinBoxEvent::VALUE_CHANGED, this, &EGGraphStormWidgets::spinBoxEvent);
    EGEventManager::connect<EWSliderEvent,EGGraphStormWidgets>(EWSliderEvent::VALUE_CHANGED, this, &EGGraphStormWidgets::sliderEvent);
    EGEventManager::connect<EWListEvent,EGGraphStormWidgets>(EWListEvent::VALUE_CHANGED, this, &EGGraphStormWidgets::listEvent);
    EGEventManager::connect<EWButtonEvent,EGGraphStormWidgets>(EWButtonEvent::PRESSED, this, &EGGraphStormWidgets::buttonEvent);
    EGEventManager::connect<EWButtonEvent,EGGraphStormWidgets>(EWButtonEvent::RELEASED, this, &EGGraphStormWidgets::buttonEvent);
    EGEventManager::connect<EWIconEvent,EGGraphStormWidgets>(EWIconEvent::PRESSED, this, &EGGraphStormWidgets::iconEvent);
    EGEventManager::connect<EWIconEvent,EGGraphStormWidgets>(EWIconEvent::RELEASED, this, &EGGraphStormWidgets::iconEvent);
    EGEventManager::connect<EWCheckBoxEvent,EGGraphStormWidgets>(EWCheckBoxEvent::VALUE_CHANGED, this, &EGGraphStormWidgets::checkBoxEvent);
    EGEventManager::connect<EWComboBoxEvent,EGGraphStormWidgets>(EWComboBoxEvent::VALUE_CHANGED, this, &EGGraphStormWidgets::comboBoxEvent);
    EGEventManager::connect<EWToggleButtonEvent,EGGraphStormWidgets>(EWToggleButtonEvent::VALUE_CHANGED, this, &EGGraphStormWidgets::toggleButtonEvent);
    EGEventManager::connect<EWWindowEvent,EGGraphStormWidgets>(EWWindowEvent::CLOSED, this, &EGGraphStormWidgets::windowEvent);
    
    // show
    context->showAll();
}

void EGGraphStormWidgets::reshape(int width, int height)
{
    viewport->screenWidth = width;
    viewport->screenHeight = height;
    viewport->scaleFactor = 1.0f;
    viewport->actualWidth = width;
    viewport->actualHeight = height;
    viewport->viewportX = 0;
    viewport->viewportY = 0;
    viewport->viewportWidth = viewport->actualWidth;
    viewport->viewportHeight = viewport->actualHeight;
    viewport->screenViewportX = 0;
    viewport->screenViewportY = 0;
    viewport->screenViewportWidth = viewport->screenWidth;
    viewport->screenViewportHeight = viewport->screenHeight;
    
    if (context) {
        context->reshape();
        context->setNeedsLayout();
    }
}

void EGGraphStormWidgets::draw()
{
    EGfloat m[16];
    
    // set viewport
    glViewport(0, 0, viewport->actualWidth, viewport->actualHeight);
    
    // setup 2D projection
    EGMath::identityMat4f(m);
    gl->loadMatrix(EGRenderMatrixProjection, m);
    
    EGMath::scaleMat4f(m, viewport->scaleFactor * 2.0f / viewport->actualWidth, viewport->scaleFactor * -2.0f / viewport->actualHeight, viewport->scaleFactor);
    EGMath::rotateMat4f(m, viewport->screenRotation, 0, 0, 1);
    EGMath::translateMat4f(m, -viewport->screenWidth * 0.5f, -viewport->screenHeight * 0.5f, 0.0f);
    gl->loadMatrix(EGRenderMatrixModelView, m);
    

#if USE_GLES2
    if (gl->isVersion(EGRenderApiVersionES2)) {
        EGRenderApiES2::apiImpl().updateUniforms();
    }
#endif
    
    // clear the surface
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the 2D context
    context->draw();
}

EGbool EGGraphStormWidgets::spinBoxEvent(EWSpinBoxEvent *evt)
{
    std::stringstream ss;
    ss << "spinbox value = " << evt->value;
    statusLabel->setLabel(ss.str());
    return true;
}

EGbool EGGraphStormWidgets::sliderEvent(EWSliderEvent *evt)
{
    std::stringstream ss;
    ss << "slider value = " << evt->value;
    statusLabel->setLabel(ss.str());
    progressBar->setProgress(evt->value);
    return true;
}

EGbool EGGraphStormWidgets::buttonEvent(EWButtonEvent *evt)
{
    if (evt->q == EWButtonEvent::PRESSED) {
        statusLabel->setLabel("button pressed");
    } else if (evt->q == EWButtonEvent::RELEASED) {
        statusLabel->setLabel("button released");
    }
    return true;
}

EGbool EGGraphStormWidgets::iconEvent(EWIconEvent *evt)
{
    if (evt->q == EWIconEvent::PRESSED) {
        statusLabel->setLabel("icon pressed");
    } else if (evt->q == EWIconEvent::RELEASED) {
        statusLabel->setLabel("icon released");
    }
    return true;
}

EGbool EGGraphStormWidgets::listEvent(EWListEvent *evt)
{
    std::stringstream ss;
    ss << "list index = " << evt->index;
    statusLabel->setLabel(ss.str());
    return true;
}

EGbool EGGraphStormWidgets::checkBoxEvent(EWCheckBoxEvent *evt)
{
    std::stringstream ss;
    ss << "checkbox enabled = " << (evt->value ? "true" : "false");
    statusLabel->setLabel(ss.str());
    return true;
}

EGbool EGGraphStormWidgets::comboBoxEvent(EWComboBoxEvent *evt)
{
    std::stringstream ss;
    ss << "combox item " << evt->index << ": " << evt->combobox->currentText();
    statusLabel->setLabel(ss.str());
    return true;
}

EGbool EGGraphStormWidgets::toggleButtonEvent(EWToggleButtonEvent *evt)
{
    std::stringstream ss;
    ss << "toggleButton enabled = " << (evt->value ? "true" : "false");
    statusLabel->setLabel(ss.str());
    return true;
}

EGbool EGGraphStormWidgets::windowEvent(EWWindowEvent *evt)
{
    std::stringstream ss;
    ss << evt->window->getName() << " close clicked";
    statusLabel->setLabel(ss.str());
    return true;
}
