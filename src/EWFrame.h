// See LICENSE for license details.

#pragma once

class EWFrame;
typedef std::shared_ptr<EWFrame> EWFramePtr;

/* EWFrame */

class EWFrame : public EWContainer
{
protected:
    EGstring label;
    EGTextPtr text;

    EGSize textSize;
    EGMargin dMargin;
    EGPadding dPadding;

public:
    EWFrame(EGstring label);
    EWFrame(EGenum widgetFlags = 0);
    virtual ~EWFrame();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();
    virtual void addWidget(EWWidgetPtr widget);

    virtual EGstring getLabel();
    virtual EGTextPtr getText();
    
    virtual void setLabel(EGstring label);
    virtual void setText(EGTextPtr text);

    virtual EGSize calcMinimumSize();
    virtual EGSize calcPreferredSize();
    virtual void setSize(EGSize size);
    virtual void layout();
};
