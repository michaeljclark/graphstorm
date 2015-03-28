/*
 *  EGBaseObject.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGBaseObject_H
#define EGBaseObject_H

#include "EG.h"
#include "EGObject.h"
#include "EGBase.h"
#include "EGColor.h"

struct EGTypeBase;
struct EGTypePoint;
struct EGTypeSize;
struct EGTypeRect;
struct EGTypeIndent;
struct EGTypeColor;

/* EGBaseObjectType */

struct EGBaseObjectType : public EGType
{
    static const EGTypePoint        Point;
    static const EGTypeSize         Size;
    static const EGTypeRect         Rect;
    static const EGTypeIndent       Indent;
    static const EGTypeColor        Color;

    EGvoid voidValue(const void *addr) const { }
    EGbool boolValue(const void *addr) const { EGbool val = 0; return val; }
    EGbyte byteValue(const void *addr) const { EGbyte val = 0; return val; }
    EGubyte ubyteValue(const void *addr) const { EGubyte val = 0; return val; }
    EGshort shortValue(const void *addr) const { EGshort val = 0; return val; }
    EGushort ushortValue(const void *addr) const { EGushort val = 0; return val; }
    EGint intValue(const void *addr) const { EGint val = 0; return val; }
    EGuint uintValue(const void *addr) const { EGuint val = 0; return val; }
    EGlong longValue(const void *addr) const { EGlong val = 0; return val; }
    EGulong ulongValue(const void *addr) const { EGulong val = 0; return val; }
    EGllong llongValue(const void *addr) const { EGllong val = 0; return val; }
    EGullong ullongValue(const void *addr) const { EGullong val = 0; return val; }
    EGfloat floatValue(const void *addr) const { EGfloat val = 0; return val; }
    EGdouble doubleValue(const void *addr) const { EGdouble val = 0; return val; }
    EGObject* objectArray(const void *addr) const { return NULL; }
    EGvoid* voidArray(const void *addr) const { return NULL; }
    EGbool* boolArray(const void *addr) const { return NULL; }
    EGbyte* byteArray(const void *addr) const { return NULL; }
    EGubyte* ubyteArray(const void *addr) const { return NULL; }
    EGshort* shortArray(const void *addr) const { return NULL; }
    EGushort* ushortArray(const void *addr) const { return NULL; }
    EGint* intArray(const void *addr) const { return NULL; }
    EGuint* uintArray(const void *addr) const { return NULL; }
    EGlong* longArray(const void *addr) const { return NULL; }
    EGulong* ulongArray(const void *addr) const { return NULL; }
    EGllong* llongArray(const void *addr) const { return NULL; }
    EGullong* ullongArray(const void *addr) const { return NULL; }
    EGfloat* floatArray(const void *addr) const { return NULL; }
    EGdouble* doubleArray(const void *addr) const { return NULL; }
};

struct EGTypePoint : public EGBaseObjectType
{
    EGTypePoint() {}
    EGstring name() const { return "EGPoint"; }
    EGsize size(size_t nelem) const { return sizeof(EGPoint); }
    EGstring toString(const void *addr, size_t nelem) const;
    EGstring toJSONString(const void *addr, size_t nelem) const;
    void fromString(const void *addr, const EGstring &str, size_t nelem) const;
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, addr); }
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const
    {
        if (&val.type == this) {
            *static_cast<EGPoint*>(const_cast<void*>(addr)) = *static_cast<const EGPoint*>(val.addr);
        }
    }
};

struct EGTypeSize : public EGBaseObjectType
{
    EGTypeSize() {}
    EGstring name() const { return "EGSize"; }
    EGsize size(size_t nelem) const { return sizeof(EGSize); }
    EGstring toString(const void *addr, size_t nelem) const;
    EGstring toJSONString(const void *addr, size_t nelem) const;
    void fromString(const void *addr, const EGstring &str, size_t nelem) const;
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, addr); }
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const
    {
        if (&val.type == this) {
            *static_cast<EGSize*>(const_cast<void*>(addr)) = *static_cast<const EGSize*>(val.addr);
        }
    }

};

struct EGTypeRect : public EGBaseObjectType
{
    EGTypeRect() {}
    EGstring name() const { return "EGRect"; }
    EGsize size(size_t nelem) const { return sizeof(EGRect); }
    EGstring toString(const void *addr, size_t nelem) const;
    EGstring toJSONString(const void *addr, size_t nelem) const;
    void fromString(const void *addr, const EGstring &str, size_t nelem) const;
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, addr); }
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const
    {
        if (&val.type == this) {
            *static_cast<EGRect*>(const_cast<void*>(addr)) = *static_cast<const EGRect*>(val.addr);
        }
    }
};

struct EGTypeIndent : public EGBaseObjectType
{
    EGTypeIndent() {}
    EGstring name() const { return "EGIndent"; }
    EGsize size(size_t nelem) const { return sizeof(EGIndent); }
    EGstring toString(const void *addr, size_t nelem) const;
    EGstring toJSONString(const void *addr, size_t nelem) const;
    void fromString(const void *addr, const EGstring &str, size_t nelem) const;
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, addr); }
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const
    {
        if (&val.type == this) {
            *static_cast<EGIndent*>(const_cast<void*>(addr)) = *static_cast<const EGIndent*>(val.addr);
        }
    }
};

struct EGTypeColor : public EGBaseObjectType
{
    EGTypeColor() {}
    EGstring name() const { return "EGColor"; }
    EGsize size(size_t nelem) const { return sizeof(EGColor); }
    EGstring toString(const void *addr, size_t nelem) const;
    EGstring toJSONString(const void *addr, size_t nelem) const;
    void fromString(const void *addr, const EGstring &str, size_t nelem) const;
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, addr); }
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const
    {
        if (&val.type == this) {
            *static_cast<EGColor*>(const_cast<void*>(addr)) = *static_cast<const EGColor*>(val.addr);
        }
    }
};

template <> inline const EGType& EGType::integralType<EGPoint>() { return EGBaseObjectType::Point; }
template <> inline const EGType& EGType::integralType<EGSize>() { return EGBaseObjectType::Size; }
template <> inline const EGType& EGType::integralType<EGRect>() { return EGBaseObjectType::Rect; }
template <> inline const EGType& EGType::integralType<EGIndent>() { return EGBaseObjectType::Indent; }
template <> inline const EGType& EGType::integralType<EGColor>() { return EGBaseObjectType::Color; }

/* EGValue */

inline EGValue pointToValue(const EGPoint &val) { return EGValue(EGBaseObjectType::Point, &val); }
inline EGValue sizeToValue(const EGSize &val) { return EGValue(EGBaseObjectType::Size, &val); }
inline EGValue rectToValue(const EGRect &val) { return EGValue(EGBaseObjectType::Rect, &val); }
inline EGValue indentToValue(const EGIndent &val) { return EGValue(EGBaseObjectType::Indent, &val); }
inline EGValue colorToValue(const EGColor &val) { return EGValue(EGBaseObjectType::Color, &val); }

inline EGPoint valueToPoint(const EGValue &val) { return (&val.type == &EGBaseObjectType::Point) ? *static_cast<const EGPoint*>(val.addr) : EGPoint(); }
inline EGSize valueToSize(const EGValue &val) { return (&val.type == &EGBaseObjectType::Size) ? *static_cast<const EGSize*>(val.addr) : EGSize(); }
inline EGRect valueToRect(const EGValue &val) { return (&val.type == &EGBaseObjectType::Rect) ? *static_cast<const EGRect*>(val.addr) : EGRect(); }
inline EGIndent valueToIndent(const EGValue &val) { return (&val.type == &EGBaseObjectType::Indent) ? *static_cast<const EGIndent*>(val.addr) : EGIndent(); }
inline EGColor valueToColor(const EGValue &val) { return (&val.type == &EGBaseObjectType::Color) ? *static_cast<const EGColor*>(val.addr) : EGColor(); }

#endif
