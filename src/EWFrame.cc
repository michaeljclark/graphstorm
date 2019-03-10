// See LICENSE for license details.

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWContainer.h"
#include "EWFrame.h"


/* EWFrame */

static char class_name[] = "EWFrame";

EWFrame::EWFrame(EGstring label) : EWContainer(0)
{
    setDefaultWidgetName();
    setDefaults(class_name);
    setLabel(label);
}

EWFrame::EWFrame(EGenum widgetFlags) : EWContainer(widgetFlags)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWFrame::~EWFrame() {}

const EGClass* EWFrame::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWFrame>()->extends<EWContainer>();
    return clazz;
}

const EGClass* EWFrame::getClass() const { return classFactory(); }

const char* EWFrame::widgetTypeName() { return class_name; }

void EWFrame::addWidget(EWWidgetPtr widget)
{
    if (children.size() == 0) {
        children.push_back(widget);
        widget->setParent(this);
    } else if (children.size() == 1) {
        children[0]->setParent(NULL);
        if (widget) {
            children[0] = widget;
            widget->setParent(this);
        } else {
            children.clear();
        }
    }
}

EGstring EWFrame::getLabel() { return label; }
EGTextPtr EWFrame::getText() { return text; }

void EWFrame::setLabel(EGstring label)
{
    if (this->label != label) {
        if (!text) {
            text = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignCenter);
        }
        text->setText(label);
        setNeedsLayout();
    }
}

void EWFrame::setText(EGTextPtr text)
{
    if (this->text != text) {
        this->text = text;
        setNeedsLayout();
    }
}

EGSize EWFrame::calcMinimumSize()
{
    EGSize minSize;
    if (children.size() == 1) {
        minSize = children[0]->calcMinimumSize();
    }
    textSize = EGSize();
    dMargin = margin;
    dPadding = padding;
    if (text) {
        textSize = EGSize(text->getWidth(), text->getHeight());
        dMargin.top = (std::max)(margin.top, textSize.height / 2);
        dPadding.top = (std::max)(margin.top, textSize.height / 2);
    }
    return minSize.expand(dMargin).expand(borderWidth).expand(dPadding);
}

EGSize EWFrame::calcPreferredSize()
{
    EGSize minSize;
    if (children.size() == 1) {
        minSize = children[0]->calcPreferredSize();
    }
    textSize = EGSize();
    dMargin = margin;
    dPadding = padding;
    if (text) {
        textSize = EGSize(text->getWidth(), text->getHeight());
        dMargin.top = (std::max)(margin.top, textSize.height / 2);
        dPadding.top = (std::max)(margin.top, textSize.height / 2);
    }
    return minSize.expand(dMargin).expand(borderWidth).expand(dPadding);
}

void EWFrame::setSize(EGSize size)
{
    rect = EGRect(rect.x, rect.y, size.width, size.height);
    if (children.size() == 1) {
        EGRect childRect = rect.contract(dMargin).contract(borderWidth).contract(dPadding);
        children[0]->setSize(EGSize(childRect.width, childRect.height));
        children[0]->setPosition(EGPoint(childRect.x, childRect.y));
    }
}

void EWFrame::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    
    renderer->begin();
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(dMargin).contract(borderWidth), fillColor);
    }
    
    EGfloat x = rect.x, y = rect.y;
    EGfloat tw = 0;
    if (text) {
        tw = text->getWidth();
        EGenum textFlags = text->getFlags();
        if (textFlags & EGTextHAlignLeft) {
            x += margin.left + padding.left + borderWidth;
        } else if (textFlags & EGTextHAlignCenter) {
            x += rect.width / 2;
        } else if (textFlags & EGTextHAlignRight) {
            x += rect.width - margin.right - padding.right - borderWidth;
        }
        if (textFlags & EGTextVAlignTop) {
            //
        } else if (textFlags & EGTextVAlignBase) {
            y += dMargin.top + borderWidth + dPadding.top;
        } else if (textFlags & EGTextVAlignCenter) {
            y += dMargin.top + borderWidth / 2;
        }
        renderer->paint(EGPoint(x, y), text, textColor);

		if (borderWidth > 0) {
			EGRect orect = rect.contract(dMargin);
			EGRect irect = rect.contract(dMargin).contract(borderWidth);
			EGPointList borderPoly;
			borderPoly.push_back(EGPoint(x - borderWidth,       orect.y));
			borderPoly.push_back(EGPoint(x - borderWidth,       irect.y));
			borderPoly.push_back(EGPoint(irect.x,               irect.y));
			borderPoly.push_back(EGPoint(irect.x,               irect.y + irect.height));
			borderPoly.push_back(EGPoint(irect.x + irect.width, irect.y + irect.height));
			borderPoly.push_back(EGPoint(irect.x + irect.width, irect.y));
			borderPoly.push_back(EGPoint(x + tw + borderWidth,  irect.y));
			borderPoly.push_back(EGPoint(x + tw + borderWidth,  orect.y));
			borderPoly.push_back(EGPoint(orect.x + orect.width, orect.y));
			borderPoly.push_back(EGPoint(orect.x + orect.width, orect.y + orect.height));
			borderPoly.push_back(EGPoint(orect.x,               orect.y + orect.height));
			borderPoly.push_back(EGPoint(orect.x,               orect.y));
			renderer->fill(borderPoly, strokeColor);
		}
	} else if (borderWidth > 0) {
		EGRect orect = rect.contract(dMargin);
		EGRect irect = rect.contract(dMargin).contract(borderWidth);
		renderer->fill(orect, irect, strokeColor);
	}

    renderer->end();
    
    if (children.size() == 1 && children[0]->getNeedsLayout()) {
        children[0]->layout();
    }
    
    needsLayout = false;
}
