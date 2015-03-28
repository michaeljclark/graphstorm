/*
 *  EWComboBox.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGViewport.h"

#include "EWWidget.h"
#include "EWContainer.h"
#include "EWContext.h"
#include "EWWindow.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWComboBox.h"


/* EWComboBoxEvent */

EGEventQualifier EWComboBoxEvent::VALUE_CHANGED   = "combobox-value-changed";

EWComboBoxEvent::EWComboBoxEvent(EGEventQualifier q, EWComboBox *combobox, EGint index)
    : EGEvent(q), combobox(combobox), index(index) {}

std::string EWComboBoxEvent::toString() {
    std::stringstream s;
    s << "[EWComboBoxEvent " << q << " combobox=" << combobox << " index=" << index << "]";
    return s.str();
};


/* EWComboBoxMenu */

static char class_name[] = "EWComboBox";
static char menu_class_name[] = "EWComboBoxMenu";

const EGint EWComboBoxMenu::CHECKMARK_WIDTH = 16;
const EGint EWComboBoxMenu::CHECKMARK_HEIGHT = 16;

EWComboBoxMenu::EWComboBoxMenu(EWComboBox *combobox, EGenum widgetFlags) : EWWidget(widgetFlags), combobox(combobox), highlightIndex(-1)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

void EWComboBoxMenu::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
}

EWComboBoxMenu::~EWComboBoxMenu() {}

const char* EWComboBoxMenu::widgetTypeName() { return menu_class_name; }

void EWComboBoxMenu::copyTextItems()
{
    textItems.resize(combobox->textItems.size());
    for (size_t i = 0; i < combobox->textItems.size(); i++) {
        if (!textItems[i]) {
            textItems[i] = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignCenter);
        }
        textItems[i]->setText(combobox->textItems[i]->getText());
    }
}

EGSize EWComboBoxMenu::calcMinimumSize()
{
    combobox->updateItems();
    entrySize = EGSize();
    for (size_t i = 0; i < textItems.size(); i++) {
        EGTextPtr &text = textItems[i];
        entrySize.width = (std::max)(entrySize.width, (EGint)text->getWidth() + padding.left + padding.right + CHECKMARK_WIDTH);
        entrySize.height = (std::max)(entrySize.height, (EGint)text->getHeight() + padding.top + padding.bottom);
    }
    entrySize.height = (std::max)(entrySize.height, CHECKMARK_HEIGHT);
    positionDelta.x = -(margin.left + borderWidth + CHECKMARK_WIDTH);
    positionDelta.y = -(margin.top + borderWidth + entrySize.height * combobox->index);
    return EGSize(margin.left + margin.right + borderWidth * 2 + entrySize.width,
                  margin.top + margin.bottom + borderWidth * 2 + entrySize.height * (EGint)textItems.size());
}

void EWComboBoxMenu::layout()
{
    if (!needsLayout) return;
    
    setSize(calcMinimumSize());
    
    EWContext *context = getContext();
    EWWindow *comboboxwindow = combobox->getWindow();
    
    // get position in context view coordinates
    EGPoint position = comboboxwindow->getPosition() + combobox->getPosition() + EGPoint(combobox->margin.left, combobox->margin.top) + combobox->borderWidth + positionDelta;
        
    // calculate position adjustment if we are near the screen boundaries
    // (perform calculation after projecting to sreen coordnates)
    EGViewportPtr viewport = context->getViewport();
    EGPoint projTopLeft = context->pointFromView(position);
    EGPoint projTopRight = context->pointFromView(position + EGPoint(rect.width, 0));
    EGPoint projBottomLeft = context->pointFromView(position + EGPoint(0, rect.height));
    EGPoint projBottomRight = context->pointFromView(position + EGPoint(rect.width, rect.height));
    EGPoint minBound((std::min)((std::min)(projTopLeft.x,projTopRight.x),(std::min)(projBottomLeft.x,projBottomRight.x)),
                (std::min)((std::min)(projTopLeft.y,projTopRight.y),(std::min)(projBottomLeft.y,projBottomRight.y)));
    EGPoint maxBound((std::max)((std::max)(projTopLeft.x,projTopRight.x),(std::max)(projBottomLeft.x,projBottomRight.x)),
                (std::max)((std::max)(projTopLeft.y,projTopRight.y),(std::max)(projBottomLeft.y,projBottomRight.y)));
    EGPoint screenDelta(((maxBound.x > viewport->screenWidth) ? viewport->screenWidth - maxBound.x : 0) + ((minBound.x < 0) ? -minBound.x : 0),
                        ((maxBound.y > viewport->screenHeight) ? viewport->screenHeight - maxBound.y : 0) + ((minBound.y < 0) ? -minBound.y : 0));
    
    // set position in context view coordinates (projecting position adjustment if required)
    if (screenDelta.x != 0 || screenDelta.y != 0) {
        setPosition(context->pointToView(context->pointFromView(position) + screenDelta));
    } else {
        setPosition(position);
    }
    
    if (!renderer) {
        renderer = createRenderer(this);
    }
    renderer->begin();
    
    if (canFocus() && hasFocus()) {
        renderer->fill(rect, rect.contract(borderWidth), focusBorderColor);
    }
    
    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(margin),
                       rect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(margin).contract(borderWidth), fillColor);
    }
    
    EGRect innerRect = rect.contract(margin).contract(borderWidth);
        
    if (highlightIndex >= 0 && highlightIndex < (EGint)combobox->items.size()) {
        EGRect selectRect(innerRect.x, innerRect.y + entrySize.height * highlightIndex,
                          innerRect.width, entrySize.height);
        renderer->fill(selectRect, selectColor);
    }
    
    if (combobox->index >= 0 && combobox->index < (EGint)combobox->items.size()) {
        if (combobox->checkmarkImage) {
            EGRect checkmarkRect(innerRect.x, innerRect.y + entrySize.height * combobox->index + entrySize.height/2 - CHECKMARK_HEIGHT/2,
                                 CHECKMARK_WIDTH, CHECKMARK_HEIGHT);
            renderer->paint(checkmarkRect, combobox->checkmarkImage);
        }
    }
    
    for (size_t i = 0; i < textItems.size(); i++) {
        EGTextPtr &text = textItems[i];
        renderer->paint(EGPoint(innerRect.x + padding.left + CHECKMARK_WIDTH, innerRect.y + entrySize.height * (EGint)i + entrySize.height / 2), text, textColor);
    }
    
    renderer->end();
    
    needsLayout = false;
}

EGbool EWComboBoxMenu::mouseEvent(EGMouseEvent *evt)
{
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON) {
        mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWComboBoxMenu>(EGMouseEvent::MOTION, this, &EWComboBoxMenu::comboBoxMenuMouseMotion);
        mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWComboBoxMenu>(EGMouseEvent::RELEASED, this, &EWComboBoxMenu::comboBoxMenuMouseReleased);
        return true;
    }
    return false;
}

EGbool EWComboBoxMenu::keyEvent(EGKeyEvent *evt)
{
    return false;
}

EGbool EWComboBoxMenu::comboBoxMenuMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGint newHighlightIndex = -1;
    EGRect innerRect = rect.contract(margin).contract(borderWidth);
    for (size_t i = 0; i < textItems.size(); i++) {
        EGRect selectRect(innerRect.x, innerRect.y + entrySize.height * (EGint)i,
                          innerRect.width, entrySize.height);
        if (selectRect.contains(p)) {
            newHighlightIndex = (EGint)i;
            break;
        }
    }
    if (newHighlightIndex != highlightIndex) {
        highlightIndex = newHighlightIndex;
        setNeedsLayout();
    }
    return true;
}

EGbool EWComboBoxMenu::comboBoxMenuMouseReleased(EGMouseEvent *evt)
{
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    if (highlightIndex >= 0) {
        combobox->setCurrentIndex(highlightIndex);
    }
    highlightIndex = -1;
    setNeedsLayout();
    return false;
}


/* EWComboBox */

const EGint EWComboBox::MAX_ITEM_WIDTH = 100;
const EGint EWComboBox::TRIANGLE_WIDTH = 6;
const EGint EWComboBox::TRIANGLE_HEIGHT = 12;

EWComboBox::EWComboBox(EGenum widgetFlags) : EWWidget(widgetFlags), index(0)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWComboBox::~EWComboBox() {}

const EGClass* EWComboBox::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWComboBox>()->extends<EWWidget>()
        ->property("currentIndex",             &EWComboBox::currentIndex,              &EWComboBox::setCurrentIndex)
        ->emitter("currentIndexChanged",       &EWComboBox::currentIndexChanged);
    return clazz;
}

const EGClass* EWComboBox::getClass() const { return classFactory(); }

void EWComboBox::setDefaults(const char *class_name)
{
    EWWidget::setDefaults(class_name);
    indicatorImage = widgetDefaults->getImage(class_name, "indicatorImage", EGImagePtr());
    checkmarkImage = widgetDefaults->getImage(class_name, "checkmarkImage", EGImagePtr());
}

const char* EWComboBox::widgetTypeName() { return class_name; }

EGint EWComboBox::currentIndex() { return index; }
EGstring EWComboBox::currentText() { return (EGint)items.size() > index ? items.at(index) : ""; }

void EWComboBox::setCurrentIndex(EGint index)
{
    if (this->index != index && index >=0 && index < (EGint)items.size()) {
        this->index = index;
        EWComboBoxEvent evt(EWComboBoxEvent::VALUE_CHANGED, this, index);
        EGEventManager::postEvent(&evt);
        currentIndexChanged(index);
        setNeedsLayout();
    }
}

void EWComboBox::currentIndexChanged(EGint index) { emit(&EWComboBox::currentIndexChanged, index); }

void EWComboBox::updateItems()
{
    for (size_t i = 0; i < items.size(); i++) {
        if (textItems.size() <= i) {
            EGTextPtr text = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignCenter);
            textItems.push_back(text);
        }
        textItems[i]->setText(items[i]);
    }
    textItems.resize(items.size());
}

void EWComboBox::clear()
{
    index = 0;
    items.clear();
    setNeedsLayout();
}

void EWComboBox::addItem(EGstring text)
{
    items.push_back(text);
    setNeedsLayout();
}

size_t EWComboBox::itemCount() { return items.size(); }

EGstring EWComboBox::itemAt(EGint index) { return items.at(index); }

EWComboBoxItemList EWComboBox::itemList() { return items; }

EGSize EWComboBox::calcMinimumSize()
{
    updateItems();
    
    EGSize maxItemSize;
    for (EGsize i = 0; i < textItems.size(); i++) {
        EGTextPtr &text = textItems.at(i);
        maxItemSize = EGSize((std::max)(maxItemSize.width, (EGint)text->getWidth()), (std::max)(maxItemSize.height, (EGint)text->getHeight()));
    }
    maxItemSize.width = (std::min)(maxItemSize.width, MAX_ITEM_WIDTH);
    
    // add room for combo box indicator
    maxItemSize.width += TRIANGLE_WIDTH;
    maxItemSize.height = (std::max)(maxItemSize.height, TRIANGLE_HEIGHT);
    
    return EGSize(margin.left + margin.right + padding.left + padding.right + borderWidth * 2 + maxItemSize.width,
                  margin.top + margin.bottom + padding.top + padding.bottom + borderWidth * 2 + maxItemSize.height);
}

void EWComboBox::setPosition(EGPoint pos)
{
    EWWidget::setPosition(pos);
    if (submenu && submenu->isVisible()) {
        submenu->setNeedsLayout();
    }
}

void EWComboBox::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
    }
    renderer->begin();
    
    if (isEnabled() && canFocus() && hasFocus() && visibleFocus()) {
        renderer->fill(rect, rect.contract(margin), focusBorderColor);
    }

    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(rect.contract(margin),
                       rect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(rect.contract(margin).contract(borderWidth), fillColor);
    }
        
    // draw triangle indicator
    if (indicatorImage) {
        EGRect indicatorRect(rect.x + rect.width - padding.right - borderWidth - margin.right - 12,
                             rect.y + rect.height / 2 - 8, 16, 16);
        renderer->paint(indicatorRect, indicatorImage);
    } else {
        EGRect innerRect = rect.contract(margin).contract(borderWidth).contract(padding);
        EGPointList triangle1;
        triangle1.push_back(EGPoint(innerRect.x + innerRect.width - TRIANGLE_WIDTH,         innerRect.y + innerRect.height * 0.5f - 1));
        triangle1.push_back(EGPoint(innerRect.x + innerRect.width,                          innerRect.y + innerRect.height * 0.5f - 1));
        triangle1.push_back(EGPoint(innerRect.x + innerRect.width - TRIANGLE_WIDTH * 0.5f,  innerRect.y + innerRect.height * 0.5f - TRIANGLE_HEIGHT * 0.5f));
        renderer->fill(triangle1, textColor);
        EGPointList triangle2;
        triangle2.push_back(EGPoint(innerRect.x + innerRect.width - TRIANGLE_WIDTH,         innerRect.y + innerRect.height * 0.5f + 1));
        triangle2.push_back(EGPoint(innerRect.x + innerRect.width - TRIANGLE_WIDTH * 0.5f,  innerRect.y + innerRect.height * 0.5f + TRIANGLE_HEIGHT * 0.5f));
        triangle2.push_back(EGPoint(innerRect.x + innerRect.width,                          innerRect.y + innerRect.height * 0.5f + 1));
        renderer->fill(triangle2, textColor);
    }
    
    EGTextPtr text = (EGint)textItems.size() > index ? textItems.at(index) : EGTextPtr();
    
    if (text) {
        EGint maxWidth = rect.width - margin.left - margin.right - borderWidth * 2 - padding.left - padding.right - TRIANGLE_WIDTH - 4;
        text->setMaximumWidth(maxWidth);
        
        EGenum textFlags = text->getFlags();
        EGfloat x = rect.x, y = rect.y;
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

EGbool EWComboBox::mouseEvent(EGMouseEvent *evt)
{
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON) {
        requestFocus();
        if (items.size() == 0) return true;
        if (!submenu) {
            submenu = EWComboBoxMenuPtr(new EWComboBoxMenu(this));
            submenu->setZOrder(1000);
        }
        submenu->copyTextItems();
        submenu->show();
        submenu->highlightIndex = index;
        EWContext *context = getContext();
        if (context) {
            context->addWidget(submenu);
        }
        mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWComboBox>(EGMouseEvent::MOTION, this, &EWComboBox::comboBoxMouseMotion);
        mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWComboBox>(EGMouseEvent::RELEASED, this, &EWComboBox::comboBoxMouseReleased);
        return submenu->mouseEvent(evt);
    }
    return false;
}

EGbool EWComboBox::keyEvent(EGKeyEvent *evt)
{
    EGint newindex = index;
    if (evt->q == EGKeyEvent::PRESSED) {
        if (evt->keycode == EGKeyEvent::KEYCODE_LEFT || evt->keycode == EGKeyEvent::KEYCODE_UP) {
            if (newindex > 0) newindex--;
        } else if (evt->keycode == EGKeyEvent::KEYCODE_RIGHT || evt->keycode == EGKeyEvent::KEYCODE_DOWN) {
            if (newindex + 1 < (EGint)items.size()) newindex++;
        } else if (evt->charcode == 9 && evt->modifiers == EGKeyEvent::MODIFIER_NONE /* tab */ ) {
            nextFocus();
            return true;
        } else if (evt->charcode == 25 || (evt->charcode == 9 && evt->modifiers == EGKeyEvent::MODIFIER_SHIFT)) {
            prevFocus();
            return true;
        }
    }
    if (index != newindex) {
        index = newindex;
        setNeedsLayout();
        EWComboBoxEvent evt(EWComboBoxEvent::VALUE_CHANGED, this, index);
        EGEventManager::postEvent(&evt);
    }
    return true;
}

EGbool EWComboBox::comboBoxMouseMotion(EGMouseEvent *evt)
{
    return false;
}

EGbool EWComboBox::comboBoxMouseReleased(EGMouseEvent *evt)
{
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    EWContext *context = getContext();
    if (context) {
        context->removeWidget(submenu);
        submenu->hide();
    }
    return false;
}
