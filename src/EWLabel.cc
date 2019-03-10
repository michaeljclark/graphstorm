// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWLabel.h"


/* EWLabel */

static char class_name[] = "EWLabel";

EWLabel::EWLabel(EGstring label) : EWWidget(0)
{
    setDefaultWidgetName();
    setDefaults(class_name);
    setLabel(label);
}

EWLabel::EWLabel(EGenum widgetFlags) : EWWidget(widgetFlags)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWLabel::~EWLabel() {}

const EGClass* EWLabel::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWLabel>()->extends<EWWidget>();
    return clazz;
}

const EGClass* EWLabel::getClass() const { return classFactory(); }

const char* EWLabel::widgetTypeName() { return class_name; }

EGstring EWLabel::getLabel() { return label; }
EGTextPtr EWLabel::getText() { return text; }

void EWLabel::setLabel(EGstring label)
{
    if (this->label != label) {
        if (!text) {
            text = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignCenter);
        }
        text->setText(label);
        setNeedsLayout();
    }
}

void EWLabel::setText(EGTextPtr text)
{
    if (this->text != text) {
        this->text = text;
        setNeedsLayout();
    }
}

EGSize EWLabel::calcMinimumSize()
{
    return EGSize(margin.left + margin.right + padding.left + padding.right + borderWidth * 2 + (text ? text->getWidth() : 0),
                  margin.top + margin.bottom + padding.top + padding.bottom + borderWidth * 2 + (text ? text->getHeight() : 0));
}

void EWLabel::layout()
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
    
    if (text) {
        EGenum textFlags = text->getFlags();
        EGint x = rect.x, y = rect.y;
        if (textFlags & EGTextHAlignLeft) {
            x += margin.left + padding.left + borderWidth;
        } else if (textFlags & EGTextHAlignCenter) {
            x += rect.width / 2;
        } else if (textFlags & EGTextHAlignRight) {
            x += rect.width - margin.right - padding.right - borderWidth;
        }
        if (textFlags & EGTextVAlignTop) {
            y += margin.top + padding.top + borderWidth;
        } else if (textFlags & EGTextVAlignBase) {
            y += rect.height - margin.bottom - padding.bottom - borderWidth;
        } else if (textFlags & EGTextVAlignCenter) {
            y += rect.height / 2;
        }
        renderer->paint(EGPoint(x, y), text, textColor);
    }
    
    renderer->end();
    
    needsLayout = false;
}
