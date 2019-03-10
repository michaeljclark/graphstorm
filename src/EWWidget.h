// See LICENSE for license details.

#pragma once

#include "EG.h"
#include "EGObject.h"
#include "EGBase.h"
#include "EGBaseObject.h"
#include "EGTime.h"
#include "EGColor.h"
#include "EGText.h"
#include "EGImage.h"
#include "EGEvent.h"
#include "EGKeyEvent.h"
#include "EGMouseEvent.h"

class EWWidget;
class EWWindow;
class EWContext;
typedef std::shared_ptr<EWWidget> EWWidgetPtr;
typedef std::vector<EWWidgetPtr> EWWidgetList;
class EWWidgetRenderer;
typedef std::shared_ptr<EWWidgetRenderer> EWWidgetRendererPtr;
class EWWidgetDefaults;
typedef std::shared_ptr<EWWidgetDefaults> EWWidgetDefaultsPtr;
  
        
/* EWWidgetFlags */

enum EWWidgetFlags
{
    EWWidgetFlagsVisible                        = 0x00000001,
    EWWidgetFlagsEnabled                        = 0x00000002,
    EWWidgetFlagsCanFocus                       = 0x00000004,
    EWWidgetFlagsHasFocus                       = 0x00000008,
    EWWidgetFlagsVisibleFocus                   = 0x00000010,
    EWWidgetFlagsFillBackground                 = 0x00000020,
    EWWidgetFlagsStrokeBorder                   = 0x00000040,
    EWWidgetFlagsImageVAlignTop                 = 0x00000080,
    EWWidgetFlagsImageVAlignBottom              = 0x00000100,
    EWWidgetFlagsImageVAlignCenter              = 0x00000180,
    EWWidgetFlagsImageVAlignJustify             = 0x00000000,
    EWWidgetFlagsImageHAlignLeft                = 0x00000200,
    EWWidgetFlagsImageHAlignRight               = 0x00000400,
    EWWidgetFlagsImageHAlignCenter              = 0x00000700,
    EWWidgetFlagsImageHAlignJustify             = 0x00000000,
    EWWidgetFlagsLayoutHorizontal               = 0x00000800,
    EWWidgetFlagsLayoutVertical                 = 0x00001000,
    EWWidgetFlagsLayoutHomogeneous              = 0x00002000,
    EWWidgetFlagsLayoutHomogeneousHorizontal    = 0x00004000,
    EWWidgetFlagsLayoutHomogeneousVertical      = 0x00008000,
    EWWidgetFlagsLayoutExpand                   = 0x00010000,
    EWWidgetFlagsLayoutExpandHorizontal         = 0x00020000,
    EWWidgetFlagsLayoutExpandVertical           = 0x00040000,
    EWWidgetFlagsWindowHasTitle                 = 0x00080000,
    EWWidgetFlagsWindowHasDecoration            = 0x00100000,
    EWWidgetFlagsWindowHasCloseBox              = 0x00200000,
    EWWidgetFlagsWindowIsMovable                = 0x00400000,
    EWWidgetFlagsWindowIsResizable              = 0x00800000,
    EWWidgetFlagsWindowIsModal                  = 0x01000000,
    EWWidgetFlagsWindowHasVScrollBar            = 0x02000000,
    EWWidgetFlagsWindowHasHScrollBar            = 0x04000000,
};

/* EWWidget */

class EWWidget : public std::enable_shared_from_this<EWWidget>, public EGObject
{
protected:
    friend class EWContext;
    
    static EGint nextWidgetId;
    static EWWidgetDefaultsPtr widgetDefaults;
    
    EGint widgetId;
    EGenum widgetFlags;
    EGstring name;
    EWWidget *parent;
    
    EGSize minimumSize;
    EGSize maximumSize;
    EGSize preferredSize;
    EGint zOrder;

    EGColor fillColor;
    EGColor strokeColor;
    EGColor textColor;
    EGColor activeFillColor;
    EGColor activeStrokeColor;
    EGColor activeTextColor;
    EGColor focusBorderColor;
    EGColor selectColor;
    EGMargin margin;
    EGPadding padding;
    EGint borderWidth;

    EGbool needsLayout;
    EGSize size;
    EGRect rect;
    
    EWWidgetRendererPtr renderer;
    
    static EWWidgetRendererPtr createRenderer(EWWidget *widget);
    
public:
    EWWidget(EGenum widgetFlags = 0);
    virtual ~EWWidget();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);
    virtual const char* widgetTypeName();
    virtual void setDefaultWidgetName();
    
    virtual EGenum getFlags();
    virtual EGstring getName();
    virtual EWWidget* getParent();
    virtual EGbool isVisible();
    virtual EGbool isEnabled();
    virtual EGbool canFocus();
    virtual EGbool visibleFocus();
    virtual EGbool hasFocus();
    virtual EGbool isFillBackground();
    virtual EGbool isStrokeBorder();
    virtual EGColor getFillColor();
    virtual EGColor getStrokeColor();
    virtual EGColor getTextColor();
    virtual EGColor getSelectColor();
    virtual EGColor getActiveFillColor();
    virtual EGColor getActiveStrokeColor();
    virtual EGColor getActiveTextColor();
    virtual EGColor getFocusBorderColor();
    virtual EGMargin getMargin();
    virtual EGPadding getPadding();
    virtual EGint getBorderWidth();
    virtual EGSize getMinimumSize();
    virtual EGSize getMaximumSize();
    virtual EGSize getPreferredSize();
    virtual EGint getZOrder();
    
    virtual void setFlags(EGenum flags);
    virtual void setName(EGstring name);
    virtual void setParent(EWWidget* parent);
    virtual void setVisible(EGbool visible);
    virtual void setEnabled(EGbool enabled);
    virtual void setCanFocus(EGbool canfocus);
    virtual void setVisibleFocus(EGbool canfocus);
    virtual void setFillBackground(EGbool fillBackground);
    virtual void setStrokeBorder(EGbool strokeBorder);
    virtual void setFillColor(EGColor fillColor);
    virtual void setStrokeColor(EGColor strokeColor);
    virtual void setTextColor(EGColor textColor);
    virtual void setSelectColor(EGColor selectColor);
    virtual void setActiveFillColor(EGColor activeFillColor);
    virtual void setActiveStrokeColor(EGColor activeStrokeColor);
    virtual void setActiveTextColor(EGColor activeTextColor);
    virtual void setFocusBorderColor(EGColor focusBorderColor);
    virtual void setMargin(EGMargin margin);
    virtual void setPadding(EGPadding padding);
    virtual void setBorderWidth(EGint borderWidth);
    virtual void setMinimumSize(EGSize minimumSize);
    virtual void setMaximumSize(EGSize maximumSize);
    virtual void setPreferredSize(EGSize preferredSize);
    virtual void setZOrder(EGint zOrder);
    
    virtual EWWidgetList* getChildren();
    virtual EWContext* getContext();
    virtual EWWindow* getWindow();
    
    virtual EGbool getNeedsLayout();
    virtual void setNeedsLayout(EGbool propagateUpwards = true);
    virtual EGSize calcMinimumSize();
    virtual EGSize calcPreferredSize();
    virtual void setSize(EGSize size);
    virtual void setPosition(EGPoint position);
    virtual EGRect getRect();
    virtual EGSize getSize();
    virtual EGPoint getPosition();
    
    virtual void show();
    virtual void showAll();
    virtual void hide();

    virtual EWWidget* nextWidget(EWWidget *current);
    virtual EWWidget* prevWidget(EWWidget *current);
    
    virtual void nextFocus();
    virtual void prevFocus();
    virtual void releaseFocus();
    virtual void requestFocus();
    
    virtual EGPoint pointFromView(EGPoint point);
    virtual EGPoint pointToView(EGPoint point);
    virtual EGfloat* getTransform();
    virtual EGfloat* getInverseTransform();
    
    virtual void layout();
    virtual void draw();
    virtual void subdraw();

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
};
