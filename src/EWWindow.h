/*
 *  EWWindow.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWWindow_H
#define EWWindow_H

class EWWindow;
typedef std::shared_ptr<EWWindow> EWWindowPtr;
class EWScrollBar;
typedef std::shared_ptr<EWScrollBar> EWScrollBarPtr;

/* EWWindowEvent */

class EWWindowEvent : public EGEvent
{
public:
    static EGEventQualifier CLOSED;
    
    typedef EWWindow emitter;
    
    EWWindow *window;
        
    EWWindowEvent(EGEventQualifier q, EWWindow *window);
    
    std::string toString();
};

/* EWWindow */

class EWWindow : public EWContainer
{
protected:
    EGstring title;
    EGTextPtr titleText;
    EGPadding titlePadding;
    EGMargin decorationMargin;
    EGColor decorationFillColor;
    EGColor decorationStrokeColor;
    EGint decorationBorderWidth;
    EGColor activeDecorationFillColor;
    EGColor closeBoxColor;
    EWScrollBarPtr vscrollbar;
    EWScrollBarPtr hscrollbar;
    
    EGPoint position;
    EGfloat matrix[16];
    EGfloat invmatrix[16];
    
    EGImagePtr closeBoxImage;
    EGMargin dMargin;
    EGint dBorder;
    EGRect closeRect;
    EGPoint dragPoint;
    EGPoint resizePoint;
    EGSize minDragSize;
    EGbool inCloseBox;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;
    
public:
    EWWindow(EGenum widgetFlags = 0);
    virtual ~EWWindow();

    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);
    virtual const char* widgetTypeName();

    virtual void addWidget(EWWidgetPtr widget);

    virtual void setTitle(EGstring title);
    virtual void setTitleText(EGTextPtr titleText);
    virtual void setTitlePadding(EGPadding titlePadding);
    virtual void setDecorationMargin(EGMargin decorationMargin);
    virtual void setDecorationFillColor(EGColor decorationFillColor);
    virtual void setDecorationStrokeColor(EGColor decorationStrokeColor);
    virtual void setDecorationBorderWidth(EGint decorationBorderWidth);
    virtual void setActiveDecorationFillColor(EGColor activeDecorationFillColor);
    virtual void setCloseBoxColor(EGColor closeBoxColor);
    virtual void setHasTitle(EGbool title);
    virtual void setHasDecoration(EGbool decoration);
    virtual void setHasCloseBox(EGbool closebox);
    virtual void setMovable(EGbool movable);
    virtual void setResizable(EGbool resizable);
    virtual void setModal(EGbool modal);
    virtual void setHasVScrollBar(EGbool vscrollbar);
    virtual void setHasHScrollBar(EGbool hscrollbar);
    
    virtual EGstring getTitle();
    virtual EGTextPtr getTitleText();
    virtual EGPadding getTitlePadding();
    virtual EGMargin getDecorationMargin();
    virtual EGColor getDecorationFillColor();
    virtual EGColor getDecorationStrokeColor();
    virtual EGint getDecorationBorderWidth();
    virtual EGColor getActiveDecorationFillColor();
    virtual EGColor getCloseBoxColor();
    virtual EGbool hasTitle();
    virtual EGbool hasDecoration();
    virtual EGbool hasCloseBox();
    virtual EGbool isMovable();
    virtual EGbool isResizable();
    virtual EGbool isModal();
    virtual EGbool hasVScrollBar();
    virtual EGbool hasHScrollBar();

    virtual EGfloat* getTransform();
    virtual EGfloat* getInverseTransform();

    virtual void setNeedsLayout(EGbool propagateUpwards = true);
    virtual EGSize calcMinimumSize();
    virtual void setSize(EGSize size);
    virtual void setPosition(EGPoint position);
    virtual EGPoint getPosition();
    virtual void layout();
    virtual void subdraw();
    
    virtual void raiseWindow(EGbool focus = true);

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool titleDragMouseMotion(EGMouseEvent *evt);
    virtual EGbool titleDragMouseReleased(EGMouseEvent *evt);
    virtual EGbool resizeDragMouseMotion(EGMouseEvent *evt);
    virtual EGbool resizeDragMouseReleased(EGMouseEvent *evt);
    virtual EGbool closeBoxMouseMotion(EGMouseEvent *evt);
    virtual EGbool closeBoxMouseReleased(EGMouseEvent *evt);
};

#endif
