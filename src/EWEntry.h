// See LICENSE for license details.

#pragma once

class EWEntry;
typedef std::shared_ptr<EWEntry> EWEntryPtr;

/* EWEntryEvent */

class EWEntryEvent : public EGEvent
{
public:
    static EGEventQualifier TEXT_CHANGED;
    
    typedef EWEntry emitter;

    EWEntry *entry;
    EGstring text;
    
    EWEntryEvent(EGEventQualifier q, EWEntry *entry, EGstring text);
    
    std::string toString();
};

/* EWEntry */

class EWEntry : public EWWidget
{
protected:
    static const EGSize MIN_ENTRY_SIZE;
    
    EWWidgetRendererPtr cursorRenderer;
    EGstring string;
    EGTextPtr text;
    EGint cursorpos;
    EGint selectstartpos;
    EGint selectendpos;
    EGbool selectdrag;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;
    
    EGPoint getTextPosition();
    
public:
    EWEntry(EGenum widgetFlags = 0);
    virtual ~EWEntry();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    virtual EGstring getString();
    virtual EGTextPtr getText();
    
    virtual void setString(EGstring string);
    virtual void setText(EGTextPtr text);

    virtual void stringChanged(EGstring string); /* emitter */

    virtual void releaseFocus();
    virtual EGSize calcMinimumSize();
    virtual void layout();
    virtual void draw();

    EGint charPositionAtPoint(EGPoint p);
    EGint xOffsetForChar(EGint charoffset);

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool entryMouseMotion(EGMouseEvent *evt);
    virtual EGbool entryMouseReleased(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
};
