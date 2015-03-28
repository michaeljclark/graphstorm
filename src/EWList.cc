/*
 *  EWList.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EWWidget.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWList.h"


/* EWListEvent */

EGEventQualifier EWListEvent::VALUE_CHANGED   = "list-value-changed";

EWListEvent::EWListEvent(EGEventQualifier q, EWList *list, EGint index)
    : EGEvent(q), list(list), index(index) {}

std::string EWListEvent::toString() {
    std::stringstream s;
    s << "[EWListEvent " << q << " List=" << list << " index=" << index << "]";
    return s.str();
};


/* EWList */

static char class_name[] = "EWList";

EWList::EWList(EGenum widgetFlags) : EWWidget(widgetFlags), index(0)
{
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWList::~EWList() {}

const EGClass* EWList::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWList>()->extends<EWWidget>()
        ->property("currentIndex",             &EWList::currentIndex,              &EWList::setCurrentIndex)
        ->emitter("currentIndexChanged",       &EWList::currentIndexChanged);
    return clazz;
}

const EGClass* EWList::getClass() const { return classFactory(); }

const char* EWList::widgetTypeName() { return class_name; }

EGint EWList::currentIndex() { return index; }
EGstring EWList::currentText() { return (EGint)items.size() > index ? items.at(index) : ""; }

void EWList::setCurrentIndex(EGint index)
{
    if (this->index != index && index >=0 && index < (EGint)items.size()) {
        this->index = index;
        EWListEvent evt(EWListEvent::VALUE_CHANGED, this, index);
        EGEventManager::postEvent(&evt);
        currentIndexChanged(index);
        setNeedsLayout();
    }
}

void EWList::currentIndexChanged(EGint index) { emit(&EWList::currentIndexChanged, index); }

void EWList::updateItems()
{
    for (EGsize i = 0; i < items.size(); i++) {
        if (textItems.size() <= i) {
            EGTextPtr text = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignCenter);
            textItems.push_back(text);
        }
        textItems[i]->setText(items[i]);
    }
    textItems.resize(items.size());
}

void EWList::clearItems()
{
    items.clear();
    setNeedsLayout();
}

void EWList::addItem(EGstring text)
{
    items.push_back(text);
    setNeedsLayout();
}

EWListItemList EWList::itemList() { return items; }

EGSize EWList::calcMinimumSize()
{
    updateItems();
    entrySize = EGSize();
    for (size_t i = 0; i < textItems.size(); i++) {
        EGTextPtr &text = textItems[i];
        entrySize.width = (std::max)(entrySize.width, (EGint)text->getWidth() + padding.left + padding.right);
        entrySize.height = (std::max)(entrySize.height, (EGint)text->getHeight() + padding.top + padding.bottom);
    }
    return EGSize(margin.left + margin.right + borderWidth * 2 + entrySize.width,
                  margin.top + margin.bottom + borderWidth * 2 + entrySize.height * (EGint)textItems.size());
}

void EWList::layout()
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
    
    EGRect innerRect = rect.contract(margin).contract(borderWidth);
    
    if (index >= 0 && index < (EGint)items.size()) {
        EGRect selectRect(innerRect.x, innerRect.y + entrySize.height * index,
                          innerRect.width, entrySize.height);
        renderer->fill(selectRect, selectColor);
    }
    
    for (size_t i = 0; i < textItems.size(); i++) {
        EGTextPtr &text = textItems[i];
        renderer->paint(EGPoint(innerRect.x + padding.left, innerRect.y + entrySize.height * (EGint)i + entrySize.height / 2), text, textColor);
    }
    
    renderer->end();
    
    needsLayout = false;
}

EGbool EWList::mouseEvent(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON) {
        requestFocus();
        EGint newindex = index;
        EGRect innerRect = rect.contract(margin).contract(borderWidth);
        for (size_t i = 0; i < textItems.size(); i++) {
            EGRect selectRect(innerRect.x, innerRect.y + entrySize.height * (EGint)i, innerRect.width, entrySize.height);
            if (selectRect.contains(p)) {
                newindex = (EGint)i;
            }
        }
        setCurrentIndex(newindex);
        return true;
    }
    return false;
}

EGbool EWList::keyEvent(EGKeyEvent *evt)
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
    setCurrentIndex(newindex);
    return true;
}
