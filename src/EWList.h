/*
 *  EWList.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWList_H
#define EWList_H

class EWList;
typedef std::shared_ptr<EWList> EWListPtr;
typedef std::vector<EGstring> EWListItemList;
typedef std::vector<EGTextPtr> EWListItemTextList;

/* EWListEvent */

class EWListEvent : public EGEvent
{
public:
    static EGEventQualifier VALUE_CHANGED;
    
    typedef EWList emitter;

    EWList *list;
    EGint index;
    
    EWListEvent(EGEventQualifier q, EWList *list, EGint index);
    
    std::string toString();
};

/* EWList */

class EWList : public EWWidget
{
protected:
    EGstring List;
    EGint index;
    EWListItemList items;
    EWListItemTextList textItems;
    EGSize entrySize;
    
    void updateItems();
  
public:
    EWList(EGenum widgetFlags = 0);
    virtual ~EWList();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    virtual EGint currentIndex();
    virtual EGstring currentText();
    virtual void setCurrentIndex(EGint index);
    
    virtual void currentIndexChanged(EGint index); /* emitter */
    
    virtual void clearItems();
    virtual void addItem(EGstring text);
    
    virtual EWListItemList itemList();
    
    virtual EGSize calcMinimumSize();
    virtual void layout();

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
};

#endif
