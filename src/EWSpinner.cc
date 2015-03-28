/*
 *  EWSpinner.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EWWidget.h"
#include "EWContainer.h"
#include "EWContext.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWSpinner.h"


/* EWSpinner */

static char class_name[] = "EWSpinner";

const EGSize EWSpinner::SPINNER_SIZE = EGSize(32, 32);
const EGint EWSpinner::NUM_SPINNER_FRAMES = 8;

EWSpinner::EWSpinner(EGenum widgetFlags) : EWWidget(widgetFlags), spinnerFrame(0)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWSpinner::~EWSpinner() {}

void EWSpinner::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
    spinnerImage = widgetDefaults->getImage(class_name, "spinnerImage", EGImagePtr());
}

const EGClass* EWSpinner::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWSpinner>()->extends<EWWidget>();
    return clazz;
}

const EGClass* EWSpinner::getClass() const { return classFactory(); }

const char* EWSpinner::widgetTypeName() { return class_name; }

EGSize EWSpinner::calcMinimumSize()
{
    return EGSize((std::max)(SPINNER_SIZE.width + margin.top + margin.bottom + borderWidth + padding.top + padding.bottom, preferredSize.width),
                  (std::max)(SPINNER_SIZE.height + margin.left + margin.right + borderWidth + padding.left + padding.right, preferredSize.height));
}

void EWSpinner::layout()
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
    EGRect spinnerRect(innerRect.x + innerRect.width/2 - SPINNER_SIZE.width/2, innerRect.y + innerRect.height/2 - SPINNER_SIZE.height/2,
                       SPINNER_SIZE.width, SPINNER_SIZE.height);
    if (spinnerImage) {
        EGfloat u1 = spinnerFrame / (EGfloat)NUM_SPINNER_FRAMES;
        EGfloat u2 = (spinnerFrame + 1) / (EGfloat)NUM_SPINNER_FRAMES;
        EGfloat texuv[8] = {
            u1, 0,
            u1, 1,
            u2, 0,
            u2, 1,
        };
        renderer->paint(spinnerRect, spinnerImage, EGColor(1,1,1,1), texuv);
    } else {
        EGint x = innerRect.x + innerRect.width / 2;
        EGint y = innerRect.y + innerRect.height / 2;
        for (EGint i = 0; i < NUM_SPINNER_FRAMES; i++) {
            EGfloat anglerad = (i * 6.2831854f) / (EGfloat)NUM_SPINNER_FRAMES;
            EGfloat sin_a = sinf(anglerad), cos_a = cosf(anglerad);
            EGPoint p1(x + (EGint)(sin_a * SPINNER_SIZE.width * 0.15f), y + (EGint)(cos_a * SPINNER_SIZE.height * 0.15f));
            EGPoint p2(x + (EGint)(sin_a * SPINNER_SIZE.width * 0.35f), y + (EGint)(cos_a * SPINNER_SIZE.height * 0.35f));
            EGfloat shade = powf(((i + spinnerFrame) % NUM_SPINNER_FRAMES) / (EGfloat)NUM_SPINNER_FRAMES, 0.45f /* rgamma */);
            EGColor color(shade * strokeColor.red, shade * strokeColor.green, shade * strokeColor.blue, strokeColor.alpha);
            renderer->stroke(EGLine(p1, p2), color, 2);
        }
    }

    renderer->end();
    
    needsLayout = false;
}

void EWSpinner::draw()
{
    const EGTime &drawTime = getContext()->getDrawTime();
    EGint newSpinnerFrame = (EGint)(drawTime.getUSec() / (1000000LL / 8) % 8);
    if (spinnerFrame != newSpinnerFrame) {
        spinnerFrame = newSpinnerFrame;
        setNeedsLayout(false);
    }
    EWWidget::draw();
}
