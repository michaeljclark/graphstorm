// See LICENSE for license details.

#pragma once

struct EWDataType;
typedef std::shared_ptr<EWDataType> EWDataTypePtr;
struct EWDataItem;
typedef std::shared_ptr<EWDataItem> EWDataItemPtr;
class EWDataSource;
typedef std::shared_ptr<EWDataSource> EWDataSourcePtr;


/* EWDataType */

struct EWDataType
{
    virtual ~EWDataType() {}
    virtual EGstring itemToString(EWDataItemPtr item) = 0;
    virtual EWDataItemPtr stringToItem(EGstring str) = 0;
};

class EWDataTypeString : EWDataType
{
    virtual EGstring itemToString(EWDataItemPtr item);
    virtual EWDataItemPtr stringToItem(EGstring str);
};

class EWDataTypeBoolean : EWDataType
{
    virtual EGstring itemToString(EWDataItemPtr item);
    virtual EWDataItemPtr stringToItem(EGstring str);
};

class EWDataTypeLong : EWDataType
{
    virtual EGstring itemToString(EWDataItemPtr item);
    virtual EWDataItemPtr stringToItem(EGstring str);
};

class EWDataTypeDouble : EWDataType
{
    virtual EGstring itemToString(EWDataItemPtr item);
    virtual EWDataItemPtr stringToItem(EGstring str);
};


/* EWDataItem */

struct EWDataItem
{
    virtual ~EWDataItem() {}
    virtual EWDataTypePtr getDataType() = 0;
};

struct EWDataItemString : EWDataItem
{
    const EGstring value;
    
    EWDataItemString(EGstring value);
    virtual ~EWDataItemString();
    virtual EWDataTypePtr getDataType();
    const EGstring getValue();
};

struct EWDataItemBoolean : EWDataItem
{
    const EGbool value;
    
    EWDataItemBoolean(EGbool value);
    virtual ~EWDataItemBoolean();
    virtual EWDataTypePtr getDataType();
    const EGbool getValue();
};

struct EWDataItemLong : EWDataItem
{
    const EGlong value;
    
    EWDataItemLong(EGlong value);
    virtual ~EWDataItemLong();
    virtual EWDataTypePtr getDataType();
    const EGlong getValue();
};

struct EWDataItemDouble : EWDataItem
{
    const EGdouble value;
    
    EWDataItemDouble(EGdouble value);
    virtual ~EWDataItemDouble();
    virtual EWDataTypePtr getDataType();
    const EGdouble getValue();
};


/* EWDataSource */

class EWDataSource
{
public:
    virtual ~EWDataSource();
    
    virtual EGint rowCount() = 0;
    virtual EGint columnCount() = 0;
    virtual EGstring columnHeading(EGint col) = 0;
    virtual EWDataTypePtr columnType(EGint col) = 0;
    virtual EWDataItemPtr getItem(EGint row, EGint col) = 0;
    virtual void setItem(EGint row, EGint col, EWDataItemPtr item) = 0;
};
