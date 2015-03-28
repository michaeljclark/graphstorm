/*
 *  EWComboBox.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWComboBox_H
#define EWComboBox_H

class EWComboBox;
typedef std::shared_ptr<EWComboBox> EWComboBoxPtr;
class EWComboBoxMenu;
typedef std::shared_ptr<EWComboBoxMenu> EWComboBoxMenuPtr;
typedef std::vector<EGstring> EWComboBoxItemList;
typedef std::vector<EGTextPtr> EWComboBoxItemTextList;

/* EWComboBoxEvent */

class EWComboBoxEvent : public EGEvent
{
public:
    static EGEventQualifier VALUE_CHANGED;
    
    typedef EWComboBox emitter;

    EWComboBox *combobox;
    EGint index;
    
    EWComboBoxEvent(EGEventQualifier q, EWComboBox *combobox, EGint index);
    
    std::string toString();
};

/* EWComboBoxMenu */

class EWComboBoxMenu : public EWWidget
{
protected:
    friend class EWComboBox;
    
    static const EGint CHECKMARK_WIDTH;
    static const EGint CHECKMARK_HEIGHT;
    
    EWComboBox *combobox;
    EGSize entrySize;
    EGPoint positionDelta;
    EWComboBoxItemTextList textItems;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;
    EGint highlightIndex;
    
public:
    EWComboBoxMenu(EWComboBox *combobox, EGenum widgetFlags = 0);
    virtual ~EWComboBoxMenu();

    virtual void setDefaults(const char *class_name);
    virtual const char* widgetTypeName();
    
    virtual void copyTextItems();

    virtual EGSize calcMinimumSize();
    virtual void layout();
    
    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
    virtual EGbool comboBoxMenuMouseMotion(EGMouseEvent *evt);
    virtual EGbool comboBoxMenuMouseReleased(EGMouseEvent *evt);
};

/* EWComboBox */

class EWComboBox : public EWWidget
{
protected:
    friend class EWComboBoxMenu;
    
    static const EGint MAX_ITEM_WIDTH;
    static const EGint TRIANGLE_WIDTH;
    static const EGint TRIANGLE_HEIGHT;
    
    EGImagePtr indicatorImage;
    EGImagePtr checkmarkImage;
    EGint index;
    EWComboBoxItemList items;
    EWComboBoxItemTextList textItems;
    EWComboBoxMenuPtr submenu;
    EGEventInfoRef mouseMotionEvent;
    EGEventInfoRef mouseReleasedEvent;
    
    void updateItems();
    
public:
    EWComboBox(EGenum widgetFlags = 0);
    virtual ~EWComboBox();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);
    virtual const char* widgetTypeName();

    virtual EGint currentIndex();
    virtual EGstring currentText();
    virtual void setCurrentIndex(EGint index);
    
    virtual void currentIndexChanged(EGint index); /* emitter */
    
    virtual void clear();
    virtual void addItem(EGstring text);
    
    virtual size_t itemCount();
    virtual EGstring itemAt(EGint index);
    virtual EWComboBoxItemList itemList();

    virtual EGSize calcMinimumSize();
    virtual void setPosition(EGPoint pos);
    virtual void layout();

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
    virtual EGbool comboBoxMouseMotion(EGMouseEvent *evt);
    virtual EGbool comboBoxMouseReleased(EGMouseEvent *evt);
};

#endif
