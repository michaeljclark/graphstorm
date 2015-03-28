/*
 *  EWGrid.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWGrid_H
#define EWGrid_H

class EWGrid;
typedef std::shared_ptr<EWGrid> EWGridPtr;

/* EWGridData */

struct EWGridData : EWContainerData
{
    EGint left;
    EGint top;
    EGint width;
    EGint height;
    
    EGSize minSize;
    EGSize prefSize;
    EGRect rect;
    
    EWGridData(EGint left, EGint top, EGint width = 1, EGint height = 1) : left(left), top(top), width(width), height(height) {}
};

/* EWGrid */

class EWGrid : public EWContainer
{
protected:
    EGint numrows;
    EGint numcols;
    EGRect** sizearray;
    EGint* colwidths;
    EGint* rowheights;
    EGint maxcolwidth;
    EGint maxrowheight;
    EGSize minsize;
    
    void clearSizeArrays();
    
public:
    EWGrid(EGenum widgetFlags = 0);
    virtual ~EWGrid();

    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);    
    virtual const char* widgetTypeName();

    virtual void addWidget(EWWidgetPtr widget, EWGridData *gridData);
    virtual void addWidget(EWWidgetPtr widget, EGint left, EGint top, EGint width = 1, EGint height = 1);
    virtual void removeWidget(EWWidgetPtr widget);

    virtual EGbool isColumnsHomogeneous();
    virtual EGbool isRowsHomogeneous();
    virtual EGbool isColumnsExpand();
    virtual EGbool isRowsExpand();
    
    virtual void setColumnsHomogeneous(EGbool homogeneous);
    virtual void setRowsHomogeneous(EGbool homogeneous);
    virtual void setColumnsExpand(EGbool expand);
    virtual void setRowsExpand(EGbool expand);

    virtual EGSize calcMinimumSize();
    virtual void setSize(EGSize size);
    
    virtual void layout();
};

#endif
