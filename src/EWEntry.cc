// See LICENSE for license details.

#include "EWWidget.h"
#include "EWContainer.h"
#include "EWContext.h"
#include "EWWidgetDefaults.h"
#include "EWWidgetRenderer.h"
#include "EWEntry.h"

#include "EGClipboard.h"


/* EWEntryEvent */

EGEventQualifier EWEntryEvent::TEXT_CHANGED   = "entry-text-changed";

EWEntryEvent::EWEntryEvent(EGEventQualifier q, EWEntry *entry, EGstring text)
    : EGEvent(q), entry(entry), text(text) {}

std::string EWEntryEvent::toString() {
    std::stringstream s;
    s << "[EWEntryEvent " << q << " entry=" << entry << " text=" << text << "]";
    return s.str();
};


/* EWEntry */

static char class_name[] = "EWEntry";

const EGSize EWEntry::MIN_ENTRY_SIZE = EGSize(8,8);

EWEntry::EWEntry(EGenum widgetFlags) : EWWidget(widgetFlags), cursorpos(0), selectstartpos(0), selectendpos(0), selectdrag(false)
{
    setString("");
    setDefaultWidgetName();
    setDefaults(class_name);
}

EWEntry::~EWEntry() {}

const EGClass* EWEntry::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EWEntry>()->extends<EWWidget>()
        ->property("string",            &EWEntry::getString,            &EWEntry::setString)
        ->emitter("stringChanged",      &EWEntry::stringChanged);
    return clazz;
}

const EGClass* EWEntry::getClass() const { return classFactory(); }

const char* EWEntry::widgetTypeName() { return class_name; }

EGstring EWEntry::getString() { return string; }
EGTextPtr EWEntry::getText() { return text; }

void EWEntry::setString(EGstring string)
{
    if (!text) {
        text = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignCenter);
    }
    if (this->string != string) {
        this->string = string;
        text->setText(string);
        EWEntryEvent evt(EWEntryEvent::TEXT_CHANGED, this, string);
        EGEventManager::postEvent(&evt);
        stringChanged(string);
        setNeedsLayout();
    }
}

void EWEntry::stringChanged(EGstring string) { emit(&EWEntry::stringChanged, string); }

void EWEntry::setText(EGTextPtr text)
{
    if (this->text != text) {
        this->text = text;
        setNeedsLayout();
    }
}

EGPoint EWEntry::getTextPosition()
{
    EGint x = rect.x, y = rect.y;
    if (text) {
        EGenum textFlags = text->getFlags();
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
    }
    return EGPoint(x, y);
}

void EWEntry::releaseFocus()
{
    cursorpos = selectstartpos = selectendpos = 0;
    EWWidget::releaseFocus();
}

EGSize EWEntry::calcMinimumSize()
{
    EGSize minSize(margin.left + margin.right + padding.left + padding.right + borderWidth * 2 + (text ? text->getWidth() : 0),
                  margin.top + margin.bottom + padding.top + padding.bottom + borderWidth * 2 + (text ? text->getHeight() : 0));
    return EGSize((std::max)(minSize.width, MIN_ENTRY_SIZE.width), (std::max)(minSize.height, MIN_ENTRY_SIZE.height));
}


void EWEntry::layout()
{
    if (!needsLayout) return;
    
    setSize(rect.size());
    if (!renderer) {
        renderer = createRenderer(this);
        cursorRenderer = createRenderer(this);
    }
    
    renderer->begin();
    
    if (isEnabled() && canFocus() && hasFocus() && visibleFocus()) {
        renderer->fill(rect, rect.contract(margin), focusBorderColor);
    }
    
    EGRect entryRect = rect;

    if (isStrokeBorder() && borderWidth > 0) {
        renderer->fill(entryRect.contract(margin),
                       entryRect.contract(margin).contract(borderWidth), strokeColor);
    }
    
    if (isFillBackground()) {
        renderer->fill(entryRect.contract(margin).contract(borderWidth), fillColor);
    }
    
    EGPoint tp = getTextPosition();
    
    if (selectstartpos != selectendpos) {
        EGint selectstartx = xOffsetForChar(selectstartpos);
        EGint selectendx = xOffsetForChar(selectendpos);
        EGRect selectRect(tp.x + selectstartx, entryRect.y + margin.top + borderWidth + padding.top,
                          selectendx - selectstartx, entryRect.height - margin.top - margin.bottom - borderWidth * 2 - padding.top - padding.bottom);
        renderer->fill(selectRect, selectColor);
    }
    
    if (text) {
        renderer->paint(tp, text, textColor);
    }
    
    renderer->end();
    
    cursorRenderer->begin();
    EGint cursorposx = xOffsetForChar(cursorpos);
    EGLine cursorLine(EGPoint(tp.x + cursorposx, entryRect.y + margin.top + borderWidth + padding.top),
                      EGPoint(tp.x + cursorposx, entryRect.y + rect.height - margin.bottom - borderWidth - padding.bottom));
    cursorRenderer->stroke(cursorLine, textColor, 1);
    cursorRenderer->end();
    
    needsLayout = false;
}

void EWEntry::draw()
{
    if (!renderer || needsLayout) {
        layout();
    }
    if (renderer) {
        renderer->draw();
    }
    if (hasFocus() && selectstartpos == selectendpos && !selectdrag) {
        EWContext *context = getContext();
        EGuint msec = (EGuint)(context->getDrawTime().getUSec() / 1000LL);
        EGuint halfsec = msec % 1000;
        if (halfsec < 500) {
            cursorRenderer->draw();
        }
    }
}

EGint EWEntry::charPositionAtPoint(EGPoint p)
{
    if (!text) return 0;
    const EGRectList &glyphRects = text->getGlyphRects();
    if (glyphRects.size() == 0) return 0;
    
    EGint newcursorpos = 0;
    EGPoint tp = getTextPosition();
    const EGRect &firstGlyphRect = glyphRects.front();
    const EGRect &lastGlyphRect = glyphRects.back();
    EGRect leadingRect(rect.x, tp.y + firstGlyphRect.y,
                       (tp.x - rect.x), firstGlyphRect.height);
    EGRect trailingRect(tp.x + lastGlyphRect.x + lastGlyphRect.width, tp.y + lastGlyphRect.y,
                        rect.width - (lastGlyphRect.x + lastGlyphRect.width), lastGlyphRect.height);
    
    if (leadingRect.contains(p)) {
        newcursorpos = 0;
    } else if (trailingRect.contains(p)) {
        newcursorpos = (EGint)glyphRects.size();
    } else {
        for (size_t i = 0; i < glyphRects.size(); i++) {
            const EGRect &glyphRect = glyphRects[i];
            EGRect testRect(tp.x + glyphRect.x, tp.y + glyphRect.y, glyphRect.width, glyphRect.height);
            if (testRect.contains(p)) {
                if (p.x > testRect.x + testRect.width / 2) {
                    newcursorpos = (EGint)i + 1;
                } else {
                    newcursorpos = (EGint)i;
                }
                break;
            }
        }
    }
    
    return newcursorpos;
}

EGint EWEntry::xOffsetForChar(EGint charoffset)
{
    if (!text) return 0;
    const EGRectList &glyphRects = text->getGlyphRects();
    if (glyphRects.size() == 0) return 0;
    const EGRect &lastGlyphRect = glyphRects.back();
    return (charoffset >= (EGint)glyphRects.size()) ? lastGlyphRect.x + lastGlyphRect.width : glyphRects[charoffset].x;
}

EGbool EWEntry::mouseEvent(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    if (evt->q == EGMouseEvent::PRESSED && evt->button == EGMouseEvent::LEFT_BUTTON) {
        mouseMotionEvent = EGEventManager::connect<EGMouseEvent,EWEntry>(EGMouseEvent::MOTION, this, &EWEntry::entryMouseMotion);
        mouseReleasedEvent = EGEventManager::connect<EGMouseEvent,EWEntry>(EGMouseEvent::RELEASED, this, &EWEntry::entryMouseReleased);
        if (!hasFocus()) {
            requestFocus();
        }
        selectdrag = true;
        EGint newcursorpos = charPositionAtPoint(p);
        if (newcursorpos != cursorpos || selectstartpos != 0 || selectendpos != 0) {
            cursorpos = newcursorpos;
            selectstartpos = selectendpos = 0;
            setNeedsLayout();
        }
        return true;
    }
    return false;
}

EGbool EWEntry::entryMouseMotion(EGMouseEvent *evt)
{
    EGPoint p = pointToView(EGPoint(evt->x, evt->y));
    EGint newcursorpos = charPositionAtPoint(p);
    EGint newselectstartpos = -1, newselectendpos = -1;
    if (newcursorpos < cursorpos) {
        newselectstartpos = newcursorpos;
        newselectendpos = cursorpos;
    } else if (newcursorpos > cursorpos) {
        newselectstartpos = cursorpos;
        newselectendpos = newcursorpos;
    } else {
        newselectstartpos = newselectendpos = 0;
    }
    if (newselectstartpos >= 0 || newselectendpos >= 0) {
        if (newselectstartpos >= 0) {
            selectstartpos = newselectstartpos;
        }
        if (newselectendpos >= 0) {
            selectendpos = newselectendpos;
        }
        setNeedsLayout();
    }
    return true;
}

EGbool EWEntry::entryMouseReleased(EGMouseEvent *evt)
{
    selectdrag = false;
    EGEventManager::disconnect(mouseMotionEvent);
    EGEventManager::disconnect(mouseReleasedEvent);
    return true;
}

EGbool EWEntry::keyEvent(EGKeyEvent *evt)
{
#if 0
    EGDebug("%s:%s %s %s\n", class_name, __func__, name.c_str(), evt->toString().c_str());
#endif
    if (evt->q == EGKeyEvent::PRESSED) {
        if (text) {
            EGstring oldstr = text->getText();
            EGstring str = text->getText();
            EGint newcursorpos = -1;
            EGint newselectstartpos = -1;
            EGint newselectendpos = -1;
            if (evt->keycode == EGKeyEvent::KEYCODE_LEFT && evt->modifiers == EGKeyEvent::MODIFIER_NONE)
            {
                if (selectstartpos != selectendpos) {
                    newcursorpos = selectstartpos;
                    newselectstartpos = 0;
                    newselectendpos = 0;
                } else if (cursorpos > 0) {
                    newcursorpos = cursorpos - 1;
                }
            }
            else if (evt->keycode == EGKeyEvent::KEYCODE_RIGHT && evt->modifiers == EGKeyEvent::MODIFIER_NONE)
            {
                if (selectstartpos != selectendpos) {
                    newcursorpos = selectendpos;
                    newselectstartpos = 0;
                    newselectendpos = 0;
                } else if (cursorpos < (EGint)str.size()) {
                    newcursorpos  = cursorpos + 1;
                }
            }
            else if (evt->keycode == EGKeyEvent::KEYCODE_LEFT && evt->modifiers == EGKeyEvent::MODIFIER_SHIFT)
            {
                if (selectstartpos == selectendpos && cursorpos > 0) {
                    newselectstartpos = cursorpos - 1;
                    newselectendpos = cursorpos;
                } else if (selectstartpos != selectendpos) {
                    if (selectendpos == cursorpos && selectstartpos < cursorpos && selectstartpos > 0) {
                        newselectstartpos = selectstartpos - 1;
                    } else if (selectstartpos == cursorpos && selectendpos > cursorpos) {
                        newselectendpos = selectendpos - 1;
                    }
                }
            }
            else if (evt->keycode == EGKeyEvent::KEYCODE_RIGHT && evt->modifiers == EGKeyEvent::MODIFIER_SHIFT)
            {
                if (selectstartpos == selectendpos && cursorpos < (EGint)str.size()) {
                    newselectstartpos = cursorpos;
                    newselectendpos = cursorpos + 1;
                } else if (selectstartpos != selectendpos) {
                    if (selectstartpos == cursorpos && selectendpos > cursorpos && selectendpos < (EGint)str.size()) {
                        newselectendpos = selectendpos + 1;
                    } else if (selectendpos == cursorpos && selectstartpos < cursorpos) {
                        newselectstartpos = selectstartpos + 1;
                    }
                }
            }
            else if ((evt->charcode == 127 || evt->keycode == 8) /* backspace */ && cursorpos > 0)
            {
                if (selectstartpos != selectendpos) {
                    str.erase(selectstartpos, selectendpos - selectstartpos);
                    setString(str);
                    newcursorpos = selectstartpos;
                    newselectstartpos = 0;
                    newselectendpos = 0;
                } else {
                    str.erase(cursorpos - 1, 1);
                    setString(str);
                    newcursorpos = cursorpos - 1;
                }
            }
            else if (evt->charcode == 9 && evt->modifiers == EGKeyEvent::MODIFIER_NONE /* tab */ )
            {
                nextFocus();
                return true;
            }
            else if (evt->charcode == 25 || (evt->charcode == 9 && evt->modifiers == EGKeyEvent::MODIFIER_SHIFT))
            {
                prevFocus();
                return true;
            }
            else if (evt->charcode >= 32 && evt->charcode < 127)
            {
                EGstring oldstr = str;
                if (selectstartpos != selectendpos) {
                    str.erase(selectstartpos, selectendpos - selectstartpos);
                    setString(str);
                    cursorpos = newcursorpos = selectstartpos;
                    newselectstartpos = 0;
                    newselectendpos = 0;
                }
                
                std::stringstream ss;
                ss << (char)evt->charcode;
                str.insert(cursorpos, ss.str());
                setString(str);
                newcursorpos = cursorpos + 1;
                
                // rollback if we can't fit the text (need to add scrolling support to handle more text than the entry width)
                if ((EGint)text->getWidth() > rect.width - margin.left - margin.right - borderWidth * 2 - padding.left - padding.right) {
                    setString(oldstr);
                    newcursorpos = cursorpos;
                }
            }
            else if (evt->charcode == 24 /* ctrl-x cut */) {
                if (selectstartpos != selectendpos) {
                    EGstring cutstr = str.substr(selectstartpos, selectendpos - selectstartpos);
                    EGbool result = EGClipboard::setData(EGClipboardDataTypeTextUTF8, cutstr.c_str(), cutstr.length() + 1);
                    if (result) {
                        str.erase(selectstartpos, selectendpos - selectstartpos);
                        setString(str);
                        cursorpos = newcursorpos = selectstartpos;
                        newselectstartpos = 0;
                        newselectendpos = 0;
                    }
                }
            }
            else if (evt->charcode == 3 /* ctrl-c copy */) {
                if (selectstartpos != selectendpos) {
                    EGstring copystr = str.substr(selectstartpos, selectendpos - selectstartpos);
                    EGClipboard::setData(EGClipboardDataTypeTextUTF8, copystr.c_str(), copystr.length() + 1);
                }
            }
            else if (evt->charcode == 22 /* ctrl-v paste */) {
                size_t len = 0;
                EGbool result = EGClipboard::getData(EGClipboardDataTypeTextUTF8, NULL, 0, &len);
                if (result && len > 0) {
                    char* data = new char[len];
                    EGClipboard::getData(EGClipboardDataTypeTextUTF8, data, len, NULL);
                    EGstring newstr(data);
                    delete [] data;
                    
                    EGstring oldstr = str;
                    if (selectstartpos != selectendpos) {
                        str.erase(selectstartpos, selectendpos - selectstartpos);
                        cursorpos = newcursorpos = selectstartpos;
                        newselectstartpos = 0;
                        newselectendpos = 0;
                    }
                    
                    str.insert(cursorpos, newstr);
                    setString(str);
                    newcursorpos = cursorpos + (EGint)newstr.length();
                    
                    // rollback if we can't fit the text (need to add scrolling support to handle more text than the entry width)
                    if ((EGint)text->getWidth() > rect.width - margin.left - margin.right - borderWidth * 2 - padding.left - padding.right) {
                        setString(oldstr);
                        newcursorpos = cursorpos;
                    }                    
                }
            }
            
            // validate cursor and selection positions with respect to string length
            if (newcursorpos > (EGint)string.length()) {
                newcursorpos = (EGint)string.length();
            }
            if (newselectstartpos > (EGint)string.length()) {
                newselectstartpos = (EGint)string.length();
            }
            if (newselectendpos > (EGint)string.length()) {
                newselectendpos = (EGint)string.length();
            }
            
            // set new cursor and selection positions
            if (newcursorpos >= 0) {
                cursorpos = newcursorpos;
            }
            if (newselectstartpos >= 0) {
                selectstartpos = newselectstartpos;
            }
            if (newselectendpos >= 0) {
                selectendpos = newselectendpos;
            }
            setNeedsLayout();
        }
    }
    return true;
}
