/*
 *  EWTable.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */


#ifndef EWTable_H
#define EWTable_H

class EWLabel;
class EWEntry;

class EWTable;
typedef std::shared_ptr<EWTable> EWTablePtr;
class EWTableModel;
typedef std::shared_ptr<EWTableModel> EWTableModelPtr;
class EWTableCellRenderer;
typedef std::shared_ptr<EWTableCellRenderer> EWTableCellRendererPtr;
class EWTableCellEditor;
typedef std::shared_ptr<EWTableCellEditor> EWTableCellEditorPtr;
class EWTableCellFactory;
typedef std::shared_ptr<EWTableCellFactory> EWTableCellFactoryPtr;


/* EWTableCellRenderer */

class EWTableCellRenderer : public EWWidget
{
public:
    virtual void setItem(EWDataItemPtr item) = 0;
    virtual EWDataItemPtr getItem() = 0;
};


/* EWTableCellEditor */

class EWTableCellEditor : public EWWidget
{
public:
    virtual void setItem(EWDataItemPtr item) = 0;
    virtual EWDataItemPtr getItem() = 0;
};


/* EWTableCellRendererDefault */

class EWTableCellRendererDefault : public EWTableCellRenderer, public EWLabel
{
protected:
    EWDataItemPtr item;
    
public:
    virtual void setItem(EWDataItemPtr item);
    virtual EWDataItemPtr getItem();
};


/* EWTableCellEditorDefault */

class EWTableCellEditorDefault : public EWTableCellEditor, public EWEntry
{
protected:
    EWDataItemPtr item;
    
public:    
    virtual void setItem(EWDataItemPtr item);
    virtual EWDataItemPtr getItem();
};


/* EWTableCellFactory */

class EWTableCellFactory
{
public:
    virtual ~EWTableCellFactory() {}
    virtual EWTableCellRendererPtr getCellRenderer(EWDataTypePtr type) = 0;
    virtual EWTableCellEditorPtr getCellEditor(EWDataTypePtr type) = 0;
};


/* EWTableCellFactoryDefault */

class EWTableCellFactoryDefault : EWTableCellFactory
{
public:
    virtual EWTableCellRendererPtr getCellRenderer(EWDataTypePtr type);
    virtual EWTableCellEditorPtr getCellEditor(EWDataTypePtr type);
};


/* EWTableModel */

class EWTableModel
{
public:
    virtual ~EWTableModel();
    
    virtual void setDataSource(EWDataSourcePtr dataSource) = 0;
    virtual EWDataSourcePtr getDataSource() = 0;
    virtual EGint tableColumnCount() = 0;
    virtual EGint modelColumnCount() = 0;
    virtual void clearColumns() = 0;
    virtual void addColumn(EGint column, EGint modelColumn) = 0;
    virtual void removeColumn(EGint column) = 0;
    virtual EGint getModelColumn(EGint column) = 0;
};


/* EWTableModelDefault */

class EWTableModelDefault : public EWTableModel
{
protected:
    EWDataSourcePtr dataSource;
    
public:    
    virtual void setDataSource(EWDataSourcePtr dataSource);
    virtual EWDataSourcePtr getDataSource();
    virtual EGint tableColumnCount();
    virtual EGint modelColumnCount();
    virtual void clearColumns();
    virtual void addColumn(EGint column, EGint modelColumn);
    virtual void removeColumn(EGint column);
    virtual EGint getModelColumn(EGint column);
};


/* EWTable */

class EWTable : public EWWidget
{
protected:
    EWTableModelPtr tableModel;

public:
    EWTable(EGenum widgetFlags = 0);
    virtual ~EWTable();

    virtual void addCellRendererFactory(EWTableCellFactoryPtr cellRendererFactory);

    virtual void setTableModel(EWTableModelPtr tableModel);
    virtual EWTableModelPtr getTableModel();
    
    virtual EGSize calcMinimumSize();
    virtual void layout();
    
    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
};

#endif
