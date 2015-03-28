/*
 *  EWProgressBar.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWProgressBar.h"


/* EWProgressBar */

static char class_name[] = "EWProgressBar";

const int EWProgressBar::MIN_PROGRESSBAR_LENGTH = 100;

EWProgressBar::EWProgressBar(EGenum widgetFlags) : EWWidget(widgetFlags), progress(0)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWProgressBar::~EWProgressBar() {}

const EGClass* EWProgressBar::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWProgressBar>()->extends<EWWidget>()
        ->property("progressBarColor",          &EWProgressBar::getProgressBarColor,        &EWProgressBar::setProgressBarColor)
        ->property("progressBarThickness",      &EWProgressBar::getProgressBarThickness,    &EWProgressBar::setProgressBarThickness);
    return clazz;
}

const EGClass* EWProgressBar::getClass() const { return classFactory(); }

void EWProgressBar::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
    setProgressBarColor(widgetDefaults->getColor(class_name, "progressBarColor", EGColor(0.7f,0.7f,0.9f,1)));
    setProgressBarThickness(widgetDefaults->getInteger(class_name, "progressBarThickness", 6));
}

const char* EWProgressBar::widgetTypeName() { return class_name; }

void EWProgressBar::setProgressBarColor(EGColor progressBarColor)
{
    if (this->progressBarColor != progressBarColor) {
        this->progressBarColor = progressBarColor;
        setNeedsLayout();
    }
}

void EWProgressBar::setProgressBarThickness(EGint progressBarThickness)
{
    if (this->progressBarThickness != progressBarThickness) {
        this->progressBarThickness = progressBarThickness;
        setNeedsLayout();
    }
}

void EWProgressBar::setProgress(EGfloat progress)
{
    if (this->progress != progress) {
        this->progress = progress;
        setNeedsLayout();
    }
}

EGfloat EWProgressBar::getProgress() { return progress; }
EGint EWProgressBar::getProgressBarThickness() { return progressBarThickness; }
EGColor EWProgressBar::getProgressBarColor() { return progressBarColor; }

EGSize EWProgressBar::calcMinimumSize()
{
    EGSize progressBarSize = EGSize(MIN_PROGRESSBAR_LENGTH, progressBarThickness);
    EGSize size((std::max)(progressBarSize.width, preferredSize.width), (std::max)(progressBarSize.height, preferredSize.height));
    return size.expand(margin).expand(borderWidth).expand(padding);
}

void EWProgressBar::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    renderer->begin();

    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(margin),
                       rect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(margin).contract(borderWidth), fillColor);
    }
    
    EGRect innerRect = rect.contract(margin).contract(borderWidth).contract(padding);
    EGSize progressBarSize = EGSize(innerRect.width, progressBarThickness);
    EGRect progressBarRect = EGRect(rect.x + margin.left + borderWidth + padding.left,
                                    rect.y + margin.top + borderWidth + padding.top + (innerRect.height - progressBarSize.height) / 2,
                                    progressBarSize.width, progressBarSize.height);
    EGRect progressRect = progressBarRect.contract(borderWidth);
    progressRect.width = (EGint)(progressRect.width * progress);

    renderer->fill(progressBarRect, progressBarRect.contract(borderWidth), strokeColor);
    renderer->fill(progressBarRect.contract(borderWidth), fillColor);
    renderer->fill(progressRect, progressBarColor);
    
    renderer->end();
    
    needsLayout = false;
}
