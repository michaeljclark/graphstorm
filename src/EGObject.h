// See LICENSE for license details.

#pragma once

#include "EG.h"

#if !defined(_WIN32) && !defined(EMSCRIPTEN)
#define USE_CXXABI 1
#endif

#if USE_CXXABI
#include <cxxabi.h>
#endif

struct EGType;
struct EGTypeString;
struct EGTypeObject;
struct EGTypeVoid;
struct EGTypeBool;
struct EGTypeByte;
struct EGTypeUByte;
struct EGTypeShort;
struct EGTypeUShort;
struct EGTypeInt;
struct EGTypeUInt;
struct EGTypeLong;
struct EGTypeULong;
struct EGTypeLongLong;
struct EGTypeULongLong;
struct EGTypeFloat;
struct EGTypeDouble;
struct EGTypeObjectPtr;
struct EGTypeVoidPtr;
struct EGTypeBoolPtr;
struct EGTypeBytePtr;
struct EGTypeUBytePtr;
struct EGTypeShortPtr;
struct EGTypeUShortPtr;
struct EGTypeIntPtr;
struct EGTypeUIntPtr;
struct EGTypeLongPtr;
struct EGTypeULongPtr;
struct EGTypeLongLongPtr;
struct EGTypeULongLongPtr;
struct EGTypeFloatPtr;
struct EGTypeDoublePtr;
struct EGTypeBoolArray;
struct EGTypeByteArray;
struct EGTypeUByteArray;
struct EGTypeShortArray;
struct EGTypeUShortArray;
struct EGTypeIntArray;
struct EGTypeUIntArray;
struct EGTypeLongArray;
struct EGTypeULongArray;
struct EGTypeLongLongArray;
struct EGTypeULongLongArray;
struct EGTypeFloatArray;
struct EGTypeDoubleArray;
struct EGValue;

class EGClass;
typedef std::shared_ptr<EGClass> EGClassPtr;
typedef std::map<const std::type_info*,EGClassPtr> EGClassTypeInfoMap;

class EGPropertyImpl;
typedef std::shared_ptr<EGPropertyImpl> EGPropertyPtr;
template <class T, typename R> class EGPropertyField;
template <class T, typename R> class EGPropertyAccessor;
typedef std::map<std::string,EGPropertyPtr> EGPropertyMap;

class EGConstructorImpl;
typedef std::shared_ptr<EGConstructorImpl> EGConstructorPtr;
template <class T> class EGConstructor0Arg;

class EGFunctionImpl;
typedef std::shared_ptr<EGFunctionImpl> EGFunctionPtr;
template <class T> class EGMemberFunction0ArgVoidReturn;
template <class T, typename A1> class EGMemberFunction1ArgVoidReturn;
template <class T, typename A1, typename A2> class EGMemberFunction2ArgVoidReturn;
template <class T, typename A1, typename A2, typename A3> class EGMemberFunction3ArgVoidReturn;
template <class T, typename A1, typename A2, typename A3, typename A4> class EGMemberFunction4ArgVoidReturn;
class EGFunctionApply0ArgVoidReturn;
template <typename A1> class EGFunctionApply1ArgVoidReturn;
template <typename A1, typename A2> class EGFunctionApply2ArgVoidReturn;
template <typename A1, typename A2, typename A3> class EGFunctionApply3ArgVoidReturn;
template <typename A1, typename A2, typename A3, typename A4> class EGFunctionApply4ArgVoidReturn;

class EGMessageImpl;
typedef std::shared_ptr<EGMessageImpl> EGMessagePtr;
typedef std::map<std::string,EGMessagePtr> EGMessageMap;

class EGEmitterImpl;
typedef std::shared_ptr<EGEmitterImpl> EGEmitterPtr;
typedef std::map<std::string,EGEmitterPtr> EGEmitterMap;

class EGConstraintImpl;
typedef std::shared_ptr<EGConstraintImpl> EGConstraintPtr;
typedef std::vector<EGConstraintPtr> EGConstraintList;
template <typename R> class EGConstraint;

class EGObject;
typedef std::shared_ptr<EGObject> EGObjectPtr;

struct EGEmitterConnection
{
    const EGEmitterPtr emitter;
    EGObject* const destObject;
    const EGPropertyPtr destProperty;
    const EGFunctionPtr destFunction;
    
    EGEmitterConnection(const EGEmitterPtr &emitter, EGObject* const destObject, const EGPropertyPtr &destProperty)
        : emitter(emitter), destObject(destObject), destProperty(destProperty), destFunction() {}
    
    EGEmitterConnection(const EGEmitterPtr &emitter, EGObject* const destObject, const EGFunctionPtr &destFunction)
        : emitter(emitter), destObject(destObject), destProperty(), destFunction(destFunction) {}
    
    bool operator==(const EGEmitterConnection &o) const {
        return emitter == o.emitter && destObject == o.destObject && destProperty == o.destProperty && destFunction == o.destFunction;
    }

    bool operator!=(const EGEmitterConnection &o) const {
        return emitter != o.emitter || destObject != o.destObject || destProperty != o.destProperty || destFunction != o.destFunction;
    }
};

typedef std::list<EGEmitterConnection> EGEmitterConnectionList;


template <typename A> inline size_t array_extent() { return std::is_array<A>::value ? std::extent<A,0>::value : 0; }


/* EGAccess */

typedef enum {
    EGAccessReadOnly,
    EGAccessReadWrite
} EGAccess;


/* EGValue */

struct EGValue
{
    static EGint nullZero;
    static EGValue nullValue;
    
    const EGType &type;
    const void *addr;
    const size_t nelem;

    template <typename R> R value();

    inline EGValue(const EGType &type, const void *addr);
    inline EGValue(const std::string &val);
    inline EGValue(const EGbool &val);
    inline EGValue(const EGbyte &val);
    inline EGValue(const EGshort &val);
    inline EGValue(const EGushort &val);
    inline EGValue(const EGint &val);
    inline EGValue(const EGuint &val);
    inline EGValue(const EGlong &val);
    inline EGValue(const EGulong &val);
    inline EGValue(const EGllong &val);
    inline EGValue(const EGullong &val);
    inline EGValue(const EGfloat &val);
    inline EGValue(const EGdouble &val);
    inline EGValue(const EGObject* val);
    inline EGValue(const EGvoid* val);
    inline EGValue(const EGbool* val);
    inline EGValue(const EGbyte* val);
    inline EGValue(const EGshort* val);
    inline EGValue(const EGushort* val);
    inline EGValue(const EGint* val);
    inline EGValue(const EGuint* val);
    inline EGValue(const EGlong* val);
    inline EGValue(const EGulong* val);
    inline EGValue(const EGllong* val);
    inline EGValue(const EGullong* val);
    inline EGValue(const EGfloat* val);
    inline EGValue(const EGdouble* val);
    template<size_t N> inline EGValue(const EGbool(&val)[N]);
    template<size_t N> inline EGValue(const EGbyte(&val)[N]);
    template<size_t N> inline EGValue(const EGshort(&val)[N]);
    template<size_t N> inline EGValue(const EGushort(&val)[N]);
    template<size_t N> inline EGValue(const EGint(&val)[N]);
    template<size_t N> inline EGValue(const EGuint(&val)[N]);
    template<size_t N> inline EGValue(const EGlong(&val)[N]);
    template<size_t N> inline EGValue(const EGulong(&val)[N]);
    template<size_t N> inline EGValue(const EGllong(&val)[N]);
    template<size_t N> inline EGValue(const EGullong(&val)[N]);
    template<size_t N> inline EGValue(const EGfloat(&val)[N]);
    template<size_t N> inline EGValue(const EGdouble(&val)[N]);
    
    inline EGstring toString() const;
    inline EGvoid voidValue() const;
    inline EGbool boolValue() const;
    inline EGbyte byteValue() const;
    inline EGubyte ubyteValue() const;
    inline EGshort shortValue() const;
    inline EGushort ushortValue() const;
    inline EGint intValue() const;
    inline EGuint uintValue() const;
    inline EGlong longValue() const;
    inline EGulong ulongValue() const;
    inline EGllong llongValue() const;
    inline EGullong ullongValue() const;
    inline EGfloat floatValue() const;
    inline EGdouble doubleValue() const;
    inline EGObject* objectArray() const;
    inline EGvoid* voidArray() const;
    inline EGbool* boolArray() const;
    inline EGbyte* byteArray() const;
    inline EGubyte* ubyteArray() const;
    inline EGshort* shortArray() const;
    inline EGushort* ushortArray() const;
    inline EGint* intArray() const;
    inline EGuint* uintArray() const;
    inline EGlong* longArray() const;
    inline EGulong* ulongArray() const;
    inline EGllong* llongArray() const;
    inline EGullong* ullongArray() const;
    inline EGfloat* floatArray() const;
    inline EGdouble* doubleArray() const;
    
    inline operator EGstring() { return toString(); }
    inline operator EGbyte() { return byteValue(); }
    inline operator EGubyte() { return ubyteValue(); }
    inline operator EGshort() { return shortValue(); }
    inline operator EGushort() { return ushortValue(); }
    inline operator EGint() { return intValue(); }
    inline operator EGuint() { return uintValue(); }
    inline operator EGlong() { return longValue(); }
    inline operator EGulong() { return ulongValue(); }
    inline operator EGllong() { return llongValue(); }
    inline operator EGullong() { return ullongValue(); }
    inline operator EGfloat() { return floatValue(); }
    inline operator EGdouble() { return doubleValue(); }
    inline operator EGObject*() { return objectArray(); }
    inline operator EGvoid*() { return voidArray(); }
    inline operator EGbyte*() { return byteArray(); }
    inline operator EGubyte*() { return ubyteArray(); }
    inline operator EGshort*() { return shortArray(); }
    inline operator EGushort*() { return ushortArray(); }
    inline operator EGint*() { return intArray(); }
    inline operator EGuint*() { return uintArray(); }
    inline operator EGlong*() { return longArray(); }
    inline operator EGulong*() { return ulongArray(); }
    inline operator EGllong*() { return llongArray(); }
    inline operator EGullong*() { return ullongArray(); }
    inline operator EGfloat*() { return floatArray(); }
    inline operator EGdouble*() { return doubleArray(); }
};


/* EGType */

struct EGType {
    static const EGTypeVoid           Void;
    static const EGTypeObject         Object;
    static const EGTypeString         String;
    static const EGTypeBool           Bool;
    static const EGTypeByte           Byte;
    static const EGTypeUByte          UByte;
    static const EGTypeShort          Short;
    static const EGTypeUShort         UShort;
    static const EGTypeInt            Int;
    static const EGTypeUInt           UInt;
    static const EGTypeLong           Long;
    static const EGTypeULong          ULong;
    static const EGTypeLongLong       LongLong;
    static const EGTypeULongLong      ULongLong;
    static const EGTypeFloat          Float;
    static const EGTypeDouble         Double;
    static const EGTypeObjectPtr      ObjectPtr;
    static const EGTypeVoidPtr        VoidPtr;
    static const EGTypeBoolPtr        BoolPtr;
    static const EGTypeBytePtr        BytePtr;
    static const EGTypeUBytePtr       UBytePtr;
    static const EGTypeShortPtr       ShortPtr;
    static const EGTypeUShortPtr      UShortPtr;
    static const EGTypeIntPtr         IntPtr;
    static const EGTypeUIntPtr        UIntPtr;
    static const EGTypeLongPtr        LongPtr;
    static const EGTypeULongPtr       ULongPtr;
    static const EGTypeLongLongPtr    LongLongPtr;
    static const EGTypeULongLongPtr   ULongLongPtr;
    static const EGTypeFloatPtr       FloatPtr;
    static const EGTypeDoublePtr      DoublePtr;
    static const EGTypeBoolArray      BoolArray;
    static const EGTypeByteArray      ByteArray;
    static const EGTypeUByteArray     UByteArray;
    static const EGTypeShortArray     ShortArray;
    static const EGTypeUShortArray    UShortArray;
    static const EGTypeIntArray       IntArray;
    static const EGTypeUIntArray      UIntArray;
    static const EGTypeLongArray      LongArray;
    static const EGTypeULongArray     ULongArray;
    static const EGTypeLongLongArray  LongLongArray;
    static const EGTypeULongLongArray ULongLongArray;
    static const EGTypeFloatArray     FloatArray;
    static const EGTypeDoubleArray    DoubleArray;
    
    template <typename R> static const EGType& integralType();
    template <typename R> static const EGType& pointerType();
    template <typename R> static const EGType& arrayType();
    template <typename R> static const EGType& typeOf()
    {
        typedef typename std::remove_extent<R>::type A;
        typedef typename std::remove_pointer<R>::type P;
        if (std::is_array<R>::value) {
            typedef typename std::remove_const<A>::type V;
            return arrayType<V>();
        } else if (std::is_pointer<R>::value) {
            typedef typename std::remove_const<P>::type V;
            return pointerType<V>();
        } else {
            typedef typename std::remove_const<R>::type V;
            return integralType<V>();
        }
    }
    
    EGstring jsonStringEncode(EGstring input) const;

    virtual ~EGType() {}
    virtual EGstring name() const = 0;
    virtual size_t size(size_t nelem) const = 0;
    virtual EGstring toString(const void *addr, size_t nelem) const = 0;
    virtual EGstring toJSONString(const void *addr, size_t nelem) const = 0;
    virtual void fromString(const void *addr, const EGstring &str, size_t nelem) const = 0;
    virtual EGValue toValue(const void *addr, size_t nelem) const = 0;
    virtual void fromValue(const void *addr, const EGValue &val, size_t nelem) const = 0;
        
    virtual EGvoid voidValue(const void *addr) const = 0;
    virtual EGbool boolValue(const void *addr) const = 0;
    virtual EGbyte byteValue(const void *addr) const = 0;
    virtual EGubyte ubyteValue(const void *addr) const = 0;
    virtual EGshort shortValue(const void *addr) const = 0;
    virtual EGushort ushortValue(const void *addr) const = 0;
    virtual EGint intValue(const void *addr) const = 0;
    virtual EGuint uintValue(const void *addr) const = 0;
    virtual EGlong longValue(const void *addr) const = 0;
    virtual EGulong ulongValue(const void *addr) const = 0;
    virtual EGllong llongValue(const void *addr) const = 0;
    virtual EGullong ullongValue(const void *addr) const = 0;
    virtual EGfloat floatValue(const void *addr) const = 0;
    virtual EGdouble doubleValue(const void *addr) const = 0;
    virtual EGObject* objectArray(const void *addr) const = 0;
    virtual EGvoid* voidArray(const void *addr) const = 0;
    virtual EGbool* boolArray(const void *addr) const = 0;
    virtual EGbyte* byteArray(const void *addr) const = 0;
    virtual EGubyte* ubyteArray(const void *addr) const = 0;
    virtual EGshort* shortArray(const void *addr) const = 0;
    virtual EGushort* ushortArray(const void *addr) const = 0;
    virtual EGint* intArray(const void *addr) const = 0;
    virtual EGuint* uintArray(const void *addr) const = 0;
    virtual EGlong* longArray(const void *addr) const = 0;
    virtual EGulong* ulongArray(const void *addr) const = 0;
    virtual EGllong* llongArray(const void *addr) const = 0;
    virtual EGullong* ullongArray(const void *addr) const = 0;
    virtual EGfloat* floatArray(const void *addr) const = 0;
    virtual EGdouble* doubleArray(const void *addr) const = 0;
};

struct EGTypeVoid : public EGType
{
    EGTypeVoid() {}
    EGstring name() const { return "Void"; }
    EGsize size(size_t nelem) const { return 0; }
    EGstring toString(const void *addr, size_t nelem) const { return ""; }
    EGstring toJSONString(const void *addr, size_t nelem) const { return "null"; }
    void fromString(const void *addr, const EGstring &str, size_t nelem) const { }
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, NULL); }
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { }
    
    EGvoid voidValue(const void *addr) const { }
    EGbool boolValue(const void *addr) const { return 0; }
    EGbyte byteValue(const void *addr) const { return 0; }
    EGubyte ubyteValue(const void *addr) const { return 0; }
    EGshort shortValue(const void *addr) const { return 0; }
    EGushort ushortValue(const void *addr) const { return 0; }
    EGint intValue(const void *addr) const { return 0; }
    EGuint uintValue(const void *addr) const { return 0; }
    EGlong longValue(const void *addr) const { return 0; }
    EGulong ulongValue(const void *addr) const { return 0; }
    EGllong llongValue(const void *addr) const { return 0; }
    EGullong ullongValue(const void *addr) const { return 0; }
    EGfloat floatValue(const void *addr) const { return 0; }
    EGdouble doubleValue(const void *addr) const { return 0; }
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

struct EGTypeObject : public EGType
{
    EGTypeObject() {}
    EGstring name() const { return "Object"; }
    EGsize size(size_t nelem) const { return 0; }
    EGstring toString(const void *addr, size_t nelem) const { return ""; }
    EGstring toJSONString(const void *addr, size_t nelem) const { return "{}"; }
    void fromString(const void *addr, const EGstring &str, size_t nelem) const { }
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, NULL); }
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { }
    
    EGvoid voidValue(const void *addr) const { }
    EGbool boolValue(const void *addr) const { return 0; }
    EGbyte byteValue(const void *addr) const { return 0; }
    EGubyte ubyteValue(const void *addr) const { return 0; }
    EGshort shortValue(const void *addr) const { return 0; }
    EGushort ushortValue(const void *addr) const { return 0; }
    EGint intValue(const void *addr) const { return 0; }
    EGuint uintValue(const void *addr) const { return 0; }
    EGlong longValue(const void *addr) const { return 0; }
    EGulong ulongValue(const void *addr) const { return 0; }
    EGllong llongValue(const void *addr) const { return 0; }
    EGullong ullongValue(const void *addr) const { return 0; }
    EGfloat floatValue(const void *addr) const { return 0; }
    EGdouble doubleValue(const void *addr) const { return 0; }
    EGObject* objectArray(const void *addr) const { return static_cast<EGObject*>(const_cast<void*>(addr)); }
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


template <typename T>
struct EGTypePrimitive : public EGType
{
    const char *_name;
    const char *_fmt;
    
public:
    EGTypePrimitive(const char *name, const char *fmt) : _name(name), _fmt(fmt) {}
    
    EGstring name() const { return _name; }
    
    EGsize size(size_t nelem) const { return sizeof(T); }
     
    EGstring toString(const void *addr, size_t nelem) const
    {
        char buf[32];
        snprintf(buf, sizeof(buf), _fmt, *((T*)addr));
        return buf;
    }

    EGstring toJSONString(const void *addr, size_t nelem) const
    {
        char buf[32];
        snprintf(buf, sizeof(buf), _fmt, *((T*)addr));
        return buf;
    }

    void fromString(const void *addr, const EGstring &str, size_t nelem) const
    {
        sscanf(str.c_str(), _fmt, (T*)addr);
    }
    
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, addr); }
    
    inline const T& const_value(const void *addr) const { return *(static_cast<const T*>(addr)); }

    inline T& value(const void *addr) const { return *(static_cast<T*>(const_cast<void*>(addr))); }

    template <typename R>
    inline R cast_value(const void *addr) const { return static_cast<R>(const_value(addr)); }

    EGvoid voidValue(const void *addr) const {}
    EGbool boolValue(const void *addr) const { return cast_value<EGbool>(addr); }
    EGbyte byteValue(const void *addr) const { return cast_value<EGbyte>(addr); }
    EGubyte ubyteValue(const void *addr) const { return cast_value<EGubyte>(addr); }
    EGshort shortValue(const void *addr) const { return cast_value<EGshort>(addr); }
    EGushort ushortValue(const void *addr) const { return cast_value<EGushort>(addr); }
    EGint intValue(const void *addr) const { return cast_value<EGint>(addr); }
    EGuint uintValue(const void *addr) const { return cast_value<EGuint>(addr); }
    EGlong longValue(const void *addr) const { return cast_value<EGlong>(addr); }
    EGulong ulongValue(const void *addr) const { return cast_value<EGulong>(addr); }
    EGllong llongValue(const void *addr) const { return cast_value<EGllong>(addr); }
    EGullong ullongValue(const void *addr) const { return cast_value<EGullong>(addr); }
    EGfloat floatValue(const void *addr) const { return cast_value<EGfloat>(addr); }
    EGdouble doubleValue(const void *addr) const { return cast_value<EGdouble>(addr); }
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

template <typename T>
struct EGTypePtr : public EGType
{
    const char *_name;
    const char *_fmt;
    
public:
    EGTypePtr(const char *name, const char *fmt) : _name(name), _fmt(fmt) {}
    
    EGstring name() const { return _name; }
    
    EGsize size(size_t nelem) const { return sizeof(T); }
    
    EGstring toString(const void *addr, size_t nelem) const
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%p", *((T*)addr));
        return buf;
    }

    EGstring toJSONString(const void *addr, size_t nelem) const
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "\"%p\"", *((T*)addr));
        return buf;
    }

    void fromString(const void *addr, const EGstring &str, size_t nelem) const {}
    
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, addr); }
    
    inline const T& const_value(const void *addr) const { return *(static_cast<const T*>(addr)); }
    
    inline T& value(const void *addr) const { return *(static_cast<T*>(const_cast<void*>(addr))); }
    
    template <typename R>
    inline R cast_array(const void *addr) const { return reinterpret_cast<R>(const_cast<void*>(addr)); }
    
    EGvoid voidValue(const void *addr) const { }
    EGbool boolValue(const void *addr) const { return 0; }
    EGbyte byteValue(const void *addr) const { return 0; }
    EGubyte ubyteValue(const void *addr) const { return 0; }
    EGshort shortValue(const void *addr) const { return 0; }
    EGushort ushortValue(const void *addr) const { return 0; }
    EGint intValue(const void *addr) const { return 0; }
    EGuint uintValue(const void *addr) const { return 0; }
    EGlong longValue(const void *addr) const { return 0; }
    EGulong ulongValue(const void *addr) const { return 0; }
    EGllong llongValue(const void *addr) const { return 0; }
    EGullong ullongValue(const void *addr) const { return 0; }
    EGfloat floatValue(const void *addr) const { return 0; }
    EGdouble doubleValue(const void *addr) const { return 0; }
    EGObject* objectArray(const void *addr) const { return cast_array<EGObject*>(addr); }
    EGvoid* voidArray(const void *addr) const { return cast_array<EGvoid*>(addr); }
    EGbool* boolArray(const void *addr) const { return cast_array<EGbool*>(addr); }
    EGbyte* byteArray(const void *addr) const { return cast_array<EGbyte*>(addr); }
    EGubyte* ubyteArray(const void *addr) const { return cast_array<EGubyte*>(addr); }
    EGshort* shortArray(const void *addr) const { return cast_array<EGshort*>(addr); }
    EGushort* ushortArray(const void *addr) const { return cast_array<EGushort*>(addr); }
    EGint* intArray(const void *addr) const { return cast_array<EGint*>(addr); }
    EGuint* uintArray(const void *addr) const { return cast_array<EGuint*>(addr); }
    EGlong* longArray(const void *addr) const { return cast_array<EGlong*>(addr); }
    EGulong* ulongArray(const void *addr) const { return cast_array<EGulong*>(addr); }
    EGllong* llongArray(const void *addr) const { return cast_array<EGllong*>(addr); }
    EGullong* ullongArray(const void *addr) const { return cast_array<EGullong*>(addr); }
    EGfloat* floatArray(const void *addr) const { return cast_array<EGfloat*>(addr); }
    EGdouble* doubleArray(const void *addr) const { return cast_array<EGdouble*>(addr); }
};

template <typename T>
struct EGTypeArray : public EGType
{
    const char *_name;
    const char *_fmt;
    
public:
    EGTypeArray(const char *name, const char *fmt) : _name(name), _fmt(fmt) {}
    
    EGstring name() const { return _name; }
    
    EGsize size(size_t nelem) const
    {
        typedef typename std::remove_extent<T>::type A;
        return sizeof(A) * nelem;
    }
    
    EGstring toString(const void *addr, size_t nelem) const
    {
        typedef typename std::remove_extent<T>::type A;
        const EGType &elemType = typeOf<A>();
        std::stringstream ss;
        ss << "[ ";
        for (size_t i = 0; i < nelem; i++) {
            if (i != 0) ss << ", ";
            ss << elemType.toJSONString((void*)((EGubyte*)addr + sizeof(A) * i), 1);
        }
        ss << " ]";
        return ss.str();
    }

    EGstring toJSONString(const void *addr, size_t nelem) const
    {
        typedef typename std::remove_extent<T>::type A;
        const EGType &elemType = typeOf<A>();
        std::stringstream ss;
        ss << "[ ";
        for (size_t i = 0; i < nelem; i++) {
            if (i != 0) ss << ", ";
            ss << elemType.toJSONString((void*)((EGubyte*)addr + sizeof(A) * i), 1);
        }
        ss << " ]";
        return ss.str();
    }
    
    void fromString(const void *addr, const EGstring &s, size_t nelem) const
    {
        typedef typename std::remove_extent<T>::type A;
        const EGType &elemType = typeOf<A>();
        
        enum { Start, PreElement, Element };
        EGenum state = Start;
        std::stringstream ss;
        size_t i = 0;
        for (std::string::const_iterator iter = s.begin(); iter != s.end(); ) {
            switch (state) {
                case Start:
                    switch (*iter) {
                        case ' ':
                            iter++;
                            break;
                        case '[':
                            iter++;
                            state = PreElement;
                            break;
                        default:
                            goto out;
                    }
                    break;
                case PreElement:
                    switch (*iter) {
                        case ' ':
                            iter++;
                            break;
                        default:
                            state = Element;
                    }
                    break;
                case Element:
                    switch (*iter) {
                        case ',':
                        case ']':
                            if (i < nelem) {
                                elemType.fromString((void*)((EGubyte*)addr + sizeof(A) * i), ss.str(), 1);
                            }
                            if (*iter == ']') {
                                goto out;
                            } else {
                                iter++;
                                i++;
                                state = PreElement;
                                ss.str("");
                            }
                            break;
                        default:
                            ss << *iter;
                            iter++;
                    }
                    break;
            }
        }
        out: return;
    }
    
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, addr); }
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const
    {
        if (this != &val.type) return;
        size_t ncopy = (std::max)(nelem, val.nelem);
        memcpy((void*)addr, val.addr, size(ncopy));
    }
        
    template <typename R>
    inline R cast_array(const void *addr) const { return reinterpret_cast<R>(const_cast<void*>(addr)); }

    EGvoid voidValue(const void *addr) const { }
    EGbool boolValue(const void *addr) const { return 0; }
    EGbyte byteValue(const void *addr) const { return 0; }
    EGubyte ubyteValue(const void *addr) const { return 0; }
    EGshort shortValue(const void *addr) const { return 0; }
    EGushort ushortValue(const void *addr) const { return 0; }
    EGint intValue(const void *addr) const { return 0; }
    EGuint uintValue(const void *addr) const { return 0; }
    EGlong longValue(const void *addr) const { return 0; }
    EGulong ulongValue(const void *addr) const { return 0; }
    EGllong llongValue(const void *addr) const { return 0; }
    EGullong ullongValue(const void *addr) const { return 0; }
    EGfloat floatValue(const void *addr) const { return 0; }
    EGdouble doubleValue(const void *addr) const { return 0; }
    EGObject* objectArray(const void *addr) const { return cast_array<EGObject*>(addr); }
    EGvoid* voidArray(const void *addr) const { return cast_array<EGvoid*>(addr); }
    EGbool* boolArray(const void *addr) const { return cast_array<EGbool*>(addr); }
    EGbyte* byteArray(const void *addr) const { return cast_array<EGbyte*>(addr); }
    EGubyte* ubyteArray(const void *addr) const { return cast_array<EGubyte*>(addr); }
    EGshort* shortArray(const void *addr) const { return cast_array<EGshort*>(addr); }
    EGushort* ushortArray(const void *addr) const { return cast_array<EGushort*>(addr); }
    EGint* intArray(const void *addr) const { return cast_array<EGint*>(addr); }
    EGuint* uintArray(const void *addr) const { return cast_array<EGuint*>(addr); }
    EGlong* longArray(const void *addr) const { return cast_array<EGlong*>(addr); }
    EGulong* ulongArray(const void *addr) const { return cast_array<EGulong*>(addr); }
    EGllong* llongArray(const void *addr) const { return cast_array<EGllong*>(addr); }
    EGullong* ullongArray(const void *addr) const { return cast_array<EGullong*>(addr); }
    EGfloat* floatArray(const void *addr) const { return cast_array<EGfloat*>(addr); }
    EGdouble* doubleArray(const void *addr) const { return cast_array<EGdouble*>(addr); }
};

struct EGTypeBool : public EGTypePrimitive<EGbool>
{
    EGTypeBool() : EGTypePrimitive<EGbool>("Bool", "%hhu") {}
    
    EGstring toString(const void *addr, size_t nelem) const { return *((EGbool*)addr) ? "true" : "false"; }
    EGstring toJSONString(const void *addr, size_t nelem) const { return *((EGbool*)addr) ? "true" : "false"; }

    void fromString(const void *addr, const EGstring &str, size_t nelem) const
    {
        if ((str.length() == 5 && strncasecmp(str.c_str(), "false", 5) == 0) || atoi(str.c_str()) == 0) {
            *((EGbool*)addr) = false;
        } else if ((str.length() == 4 && strncasecmp(str.c_str(), "true", 4)) == 0 || atoi(str.c_str()) != 0) {
            *((EGbool*)addr) = true;
        }
    }

    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.boolValue(); }
};

struct EGTypeByte : public EGTypePrimitive<EGbyte>
{
    EGTypeByte() : EGTypePrimitive<EGbyte>("Byte", "%hhd") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.byteValue(); }
};

struct EGTypeUByte : public EGTypePrimitive<EGubyte>
{
    EGTypeUByte() : EGTypePrimitive<EGubyte>("UByte", "%hhu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.ubyteValue(); }
};

struct EGTypeShort : public EGTypePrimitive<EGshort>
{
    EGTypeShort() : EGTypePrimitive<EGshort>("Short", "%hd") {}

    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.shortValue(); }
};

struct EGTypeUShort : public EGTypePrimitive<EGushort>
{
    EGTypeUShort() : EGTypePrimitive<EGushort>("UShort", "%hu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.ushortValue(); }
};

struct EGTypeInt : public EGTypePrimitive<EGint>
{
    EGTypeInt() : EGTypePrimitive<EGint>("Int", "%d") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.intValue(); }
};

struct EGTypeUInt : public EGTypePrimitive<EGuint>
{
    EGTypeUInt() : EGTypePrimitive<EGuint>("UInt", "%u") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.uintValue(); }
};

struct EGTypeLong : public EGTypePrimitive<EGlong>
{
    EGTypeLong() : EGTypePrimitive<EGlong>("Long", "%ld") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.longValue(); }
};

struct EGTypeULong : public EGTypePrimitive<EGulong>
{
    EGTypeULong() : EGTypePrimitive<EGulong>("ULong", "%lu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.ulongValue(); }
};

struct EGTypeLongLong : public EGTypePrimitive<EGllong>
{
    EGTypeLongLong() : EGTypePrimitive<EGllong>("LongLong", "%lld") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.llongValue(); }
};

struct EGTypeULongLong : public EGTypePrimitive<EGullong>
{
    EGTypeULongLong() : EGTypePrimitive<EGullong>("ULongLong", "%llu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.ullongValue(); }
};

struct EGTypeFloat : public EGTypePrimitive<EGfloat>
{
    EGTypeFloat() : EGTypePrimitive<EGfloat>("Float", "%f") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.floatValue(); }
};

struct EGTypeDouble : public EGTypePrimitive<EGdouble>
{
    EGTypeDouble() : EGTypePrimitive<EGdouble>("Double", "%lf") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.doubleValue(); }
};

struct EGTypeObjectPtr : public EGTypePtr<EGObject*>
{
    EGTypeObjectPtr() : EGTypePtr<EGObject*>("Object*", "%hhu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.objectArray(); }
};

struct EGTypeVoidPtr : public EGTypePtr<EGvoid*>
{
    EGTypeVoidPtr() : EGTypePtr<EGvoid*>("Void*", "%hhu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.voidArray(); }
};

struct EGTypeBoolPtr : public EGTypePtr<EGbool*>
{
    EGTypeBoolPtr() : EGTypePtr<EGbool*>("Bool*", "%hhu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.boolArray(); }
};

struct EGTypeBytePtr : public EGTypePtr<EGbyte*>
{
    EGTypeBytePtr() : EGTypePtr<EGbyte*>("Byte*", "%hhd") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.byteArray(); }
};

struct EGTypeUBytePtr : public EGTypePtr<EGubyte*>
{
    EGTypeUBytePtr() : EGTypePtr<EGubyte*>("UByte*", "%hhu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.ubyteArray(); }
};

struct EGTypeShortPtr : public EGTypePtr<EGshort*>
{
    EGTypeShortPtr() : EGTypePtr<EGshort*>("Short*", "%hd") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.shortArray(); }
};

struct EGTypeUShortPtr : public EGTypePtr<EGushort*>
{
    EGTypeUShortPtr() : EGTypePtr<EGushort*>("UShort*", "%hu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.ushortArray(); }
};

struct EGTypeIntPtr : public EGTypePtr<EGint*>
{
    EGTypeIntPtr() : EGTypePtr<EGint*>("Int*", "%d") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.intArray(); }
};

struct EGTypeUIntPtr : public EGTypePtr<EGuint*>
{
    EGTypeUIntPtr() : EGTypePtr<EGuint*>("UInt*", "%u") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.uintArray(); }
};

struct EGTypeLongPtr : public EGTypePtr<EGlong*>
{
    EGTypeLongPtr() : EGTypePtr<EGlong*>("Long*", "%ld") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.longArray(); }
};

struct EGTypeULongPtr : public EGTypePtr<EGulong*>
{
    EGTypeULongPtr() : EGTypePtr<EGulong*>("ULong*", "%lu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.ulongArray(); }
};

struct EGTypeLongLongPtr : public EGTypePtr<EGllong*>
{
    EGTypeLongLongPtr() : EGTypePtr<EGllong*>("LongLong*", "%lld") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.llongArray(); }
};

struct EGTypeULongLongPtr : public EGTypePtr<EGullong*>
{
    EGTypeULongLongPtr() : EGTypePtr<EGullong*>("ULongLong*", "%llu") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.ullongArray(); }
};

struct EGTypeFloatPtr : public EGTypePtr<EGfloat*>
{
    EGTypeFloatPtr() : EGTypePtr<EGfloat*>("Float*", "%f") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.floatArray(); }
};

struct EGTypeDoublePtr : public EGTypePtr<EGdouble*>
{
    EGTypeDoublePtr() : EGTypePtr<EGdouble*>("Double*", "%lf") {}
    
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { value(addr) = val.doubleArray(); }
};

struct EGTypeBoolArray : public EGTypeArray<EGbool[]>
{
    EGTypeBoolArray() : EGTypeArray<EGbool[]>("Bool[]", "%hhu") {}
    
    EGstring toString(const void *addr, size_t nelem) const { return "[]"; }
};

struct EGTypeByteArray : public EGTypeArray<EGbyte[]>
{
    EGTypeByteArray() : EGTypeArray<EGbyte[]>("Byte[]", "%hhd") {}
};

struct EGTypeUByteArray : public EGTypeArray<EGubyte[]>
{
    EGTypeUByteArray() : EGTypeArray<EGubyte[]>("UByte[]", "%hhu") {}
};

struct EGTypeShortArray : public EGTypeArray<EGshort[]>
{
    EGTypeShortArray() : EGTypeArray<EGshort[]>("Short[]", "%hd") {}
};

struct EGTypeUShortArray : public EGTypeArray<EGushort[]>
{
    EGTypeUShortArray() : EGTypeArray<EGushort[]>("UShort[]", "%hu") {}
};

struct EGTypeIntArray : public EGTypeArray<EGint[]>
{
    EGTypeIntArray() : EGTypeArray<EGint[]>("Int[]", "%d") {}
};

struct EGTypeUIntArray : public EGTypeArray<EGuint[]>
{
    EGTypeUIntArray() : EGTypeArray<EGuint[]>("UInt[]", "%u") {}
};

struct EGTypeLongArray : public EGTypeArray<EGlong[]>
{
    EGTypeLongArray() : EGTypeArray<EGlong[]>("Long[]", "%ld") {}
};

struct EGTypeULongArray : public EGTypeArray<EGulong[]>
{
    EGTypeULongArray() : EGTypeArray<EGulong[]>("ULong[]", "%lu") {}
};

struct EGTypeLongLongArray : public EGTypeArray<EGllong[]>
{
    EGTypeLongLongArray() : EGTypeArray<EGllong[]>("LongLong[]", "%lld") {}
};

struct EGTypeULongLongArray : public EGTypeArray<EGullong[]>
{
    EGTypeULongLongArray() : EGTypeArray<EGullong[]>("ULongLong[]", "%llu") {}
};

struct EGTypeFloatArray : public EGTypeArray<EGfloat[]>
{
    EGTypeFloatArray() : EGTypeArray<EGfloat[]>("Float[]", "%f") {}
};

struct EGTypeDoubleArray : public EGTypeArray<EGdouble[]>
{
    EGTypeDoubleArray() : EGTypeArray<EGdouble[]>("Double[]", "%lf") {}
};

struct EGTypeString : public EGType
{
    EGTypeString() {}
    EGstring name() const { return "String"; }
    EGsize size(size_t nelem) const { return sizeof(EGstring); }
    EGstring toString(const void *addr, size_t nelem) const { return *((EGstring*)addr); }
    EGstring toJSONString(const void *addr, size_t nelem) const { return std::string("\"") + jsonStringEncode(*((EGstring*)addr)) + std::string("\""); }
    void fromString(const void *addr, const EGstring &str, size_t nelem) const { *((EGstring*)addr) = str; }
    EGValue toValue(const void *addr, size_t nelem) const { return EGValue(*this, addr); }
    void fromValue(const void *addr, const EGValue &val, size_t nelem) const { *((EGstring*)addr) = val.toString(); }

    EGvoid voidValue(const void *addr) const { }
    EGbool boolValue(const void *addr) const { EGbool val = 0; EGType::Bool.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGbyte byteValue(const void *addr) const { EGbyte val = 0; EGType::Byte.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGubyte ubyteValue(const void *addr) const { EGubyte val = 0; EGType::UByte.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGshort shortValue(const void *addr) const { EGshort val = 0; EGType::Short.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGushort ushortValue(const void *addr) const { EGushort val = 0; EGType::UShort.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGint intValue(const void *addr) const { EGint val = 0; EGType::Int.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGuint uintValue(const void *addr) const { EGuint val = 0; EGType::UInt.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGlong longValue(const void *addr) const { EGlong val = 0; EGType::Long.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGulong ulongValue(const void *addr) const { EGulong val = 0; EGType::ULong.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGllong llongValue(const void *addr) const { EGllong val = 0; EGType::LongLong.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGullong ullongValue(const void *addr) const { EGullong val = 0; EGType::ULongLong.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGfloat floatValue(const void *addr) const { EGfloat val = 0; EGType::Float.fromString(&val, *((EGstring*)addr), 1); return val; }
    EGdouble doubleValue(const void *addr) const { EGdouble val = 0; EGType::Double.fromString(&val, *((EGstring*)addr), 1); return val; }
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

template <> inline const EGType& EGType::integralType<EGstring>() { return EGType::String; }
template <> inline const EGType& EGType::integralType<EGObject>() { return EGType::Object; }
template <> inline const EGType& EGType::integralType<EGvoid>() { return EGType::Void; }
template <> inline const EGType& EGType::integralType<EGbool>() { return EGType::Bool; }
template <> inline const EGType& EGType::integralType<EGbyte>() { return EGType::Byte; }
template <> inline const EGType& EGType::integralType<EGshort>() { return EGType::Short; }
template <> inline const EGType& EGType::integralType<EGushort>() { return EGType::UShort; }
template <> inline const EGType& EGType::integralType<EGint>() { return EGType::Int; }
template <> inline const EGType& EGType::integralType<EGuint>() { return EGType::UInt; }
template <> inline const EGType& EGType::integralType<EGlong>() { return EGType::Long; }
template <> inline const EGType& EGType::integralType<EGulong>() { return EGType::ULong; }
template <> inline const EGType& EGType::integralType<EGllong>() { return EGType::LongLong; }
template <> inline const EGType& EGType::integralType<EGullong>() { return EGType::ULongLong; }
template <> inline const EGType& EGType::integralType<EGfloat>() { return EGType::Float; }
template <> inline const EGType& EGType::integralType<EGdouble>() { return EGType::Double; }
template <> inline const EGType& EGType::pointerType<EGvoid>() { return EGType::VoidPtr; }
template <> inline const EGType& EGType::pointerType<EGObject>() { return EGType::ObjectPtr; }
template <> inline const EGType& EGType::pointerType<EGbool>() { return EGType::BoolPtr; }
template <> inline const EGType& EGType::pointerType<EGbyte>() { return EGType::BytePtr; }
template <> inline const EGType& EGType::pointerType<EGshort>() { return EGType::ShortPtr; }
template <> inline const EGType& EGType::pointerType<EGushort>() { return EGType::UShortPtr; }
template <> inline const EGType& EGType::pointerType<EGint>() { return EGType::IntPtr; }
template <> inline const EGType& EGType::pointerType<EGuint>() { return EGType::UIntPtr; }
template <> inline const EGType& EGType::pointerType<EGlong>() { return EGType::LongPtr; }
template <> inline const EGType& EGType::pointerType<EGulong>() { return EGType::ULongPtr; }
template <> inline const EGType& EGType::pointerType<EGllong>() { return EGType::LongLongPtr; }
template <> inline const EGType& EGType::pointerType<EGullong>() { return EGType::ULongLongPtr; }
template <> inline const EGType& EGType::pointerType<EGfloat>() { return EGType::FloatPtr; }
template <> inline const EGType& EGType::pointerType<EGdouble>() { return EGType::DoublePtr; }
template <> inline const EGType& EGType::arrayType<EGbool>() { return EGType::BoolArray; }
template <> inline const EGType& EGType::arrayType<EGbyte>() { return EGType::ByteArray; }
template <> inline const EGType& EGType::arrayType<EGshort>() { return EGType::ShortArray; }
template <> inline const EGType& EGType::arrayType<EGushort>() { return EGType::UShortArray; }
template <> inline const EGType& EGType::arrayType<EGint>() { return EGType::IntArray; }
template <> inline const EGType& EGType::arrayType<EGuint>() { return EGType::UIntArray; }
template <> inline const EGType& EGType::arrayType<EGlong>() { return EGType::LongArray; }
template <> inline const EGType& EGType::arrayType<EGulong>() { return EGType::ULongArray; }
template <> inline const EGType& EGType::arrayType<EGllong>() { return EGType::LongLongArray; }
template <> inline const EGType& EGType::arrayType<EGullong>() { return EGType::ULongLongArray; }
template <> inline const EGType& EGType::arrayType<EGfloat>() { return EGType::FloatArray; }
template <> inline const EGType& EGType::arrayType<EGdouble>() { return EGType::DoubleArray; }


/* EGValue */

inline EGValue::EGValue(const EGType &type, const void *addr) : type(type), addr(addr), nelem(1) {}
inline EGValue::EGValue(const std::string &val) : type(EGType::String), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGbool &val) : type(EGType::Bool), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGbyte &val) : type(EGType::Byte), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGshort &val) : type(EGType::Short), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGushort &val) : type(EGType::UShort), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGint &val) : type(EGType::Int), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGuint &val) : type(EGType::UInt), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGlong &val) : type(EGType::Long), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGulong &val) : type(EGType::ULong), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGllong &val) : type(EGType::LongLong), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGullong &val) : type(EGType::ULongLong), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGfloat &val) : type(EGType::Float), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGdouble &val) : type(EGType::Double), addr((void*)&val), nelem(1) {}
inline EGValue::EGValue(const EGObject* val) : type(EGType::ObjectPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGvoid* val) : type(EGType::VoidPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGbool* val) : type(EGType::BoolPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGbyte* val) : type(EGType::BytePtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGshort* val) : type(EGType::ShortPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGushort* val) : type(EGType::UShortPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGint* val) : type(EGType::IntPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGuint* val) : type(EGType::UIntPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGlong* val) : type(EGType::LongPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGulong* val) : type(EGType::ULongPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGllong* val) : type(EGType::LongLongPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGullong* val) : type(EGType::ULongLongPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGfloat* val) : type(EGType::FloatPtr), addr((void*)val), nelem(1) {}
inline EGValue::EGValue(const EGdouble* val) : type(EGType::DoublePtr), addr((void*)val), nelem(1) {}
template<size_t N> inline EGValue::EGValue(const EGbool(&val)[N]) : type(EGType::BoolArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGbyte(&val)[N]) : type(EGType::ByteArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGshort(&val)[N]) : type(EGType::ShortArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGushort(&val)[N]) : type(EGType::UShortArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGint(&val)[N]) : type(EGType::IntArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGuint(&val)[N]) : type(EGType::UIntArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGlong(&val)[N]) : type(EGType::LongArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGulong(&val)[N]) : type(EGType::ULongArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGllong(&val)[N]) : type(EGType::LongLongArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGullong(&val)[N]) : type(EGType::ULongLongArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGfloat(&val)[N]) : type(EGType::FloatArray), addr((void*)val), nelem(N) {}
template<size_t N> inline EGValue::EGValue(const EGdouble(&val)[N]) : type(EGType::DoubleArray), addr((void*)val), nelem(N) {}

template <> inline EGvoid   EGValue::value<EGvoid>() { return voidValue(); }
template <> inline EGbool   EGValue::value<EGbool>() { return boolValue(); }
template <> inline EGbyte   EGValue::value<EGbyte>() { return byteValue(); }
template <> inline EGshort  EGValue::value<EGshort>() { return shortValue(); }
template <> inline EGushort EGValue::value<EGushort>() { return ushortValue(); }
template <> inline EGint    EGValue::value<EGint>() { return intValue(); }
template <> inline EGuint   EGValue::value<EGuint>() { return uintValue(); }
template <> inline EGlong   EGValue::value<EGlong>() { return longValue(); }
template <> inline EGulong  EGValue::value<EGulong>() { return ulongValue(); }
template <> inline EGllong  EGValue::value<EGllong>() { return llongValue(); }
template <> inline EGullong EGValue::value<EGullong>() { return ullongValue(); }
template <> inline EGfloat  EGValue::value<EGfloat>() { return floatValue(); }
template <> inline EGdouble EGValue::value<EGdouble>() { return doubleValue(); }

inline EGstring   EGValue::toString() const { return type.toString(addr, nelem); }
inline EGvoid     EGValue::voidValue() const { return type.voidValue(addr); }
inline EGbool     EGValue::boolValue() const { return type.boolValue(addr); }
inline EGbyte     EGValue::byteValue() const { return type.byteValue(addr); }
inline EGubyte    EGValue::ubyteValue() const { return type.ubyteValue(addr); }
inline EGshort    EGValue::shortValue() const { return type.shortValue(addr); }
inline EGushort   EGValue::ushortValue() const { return type.ushortValue(addr); }
inline EGint      EGValue::intValue() const { return type.intValue(addr); }
inline EGuint     EGValue::uintValue() const { return type.uintValue(addr); }
inline EGlong     EGValue::longValue() const { return type.longValue(addr); }
inline EGulong    EGValue::ulongValue() const { return type.ulongValue(addr); }
inline EGllong    EGValue::llongValue() const { return type.llongValue(addr); }
inline EGullong   EGValue::ullongValue() const { return type.ullongValue(addr); }
inline EGfloat    EGValue::floatValue() const { return type.floatValue(addr); }
inline EGdouble   EGValue::doubleValue() const { return type.doubleValue(addr); }
inline EGObject*  EGValue::objectArray() const { return type.objectArray(addr); }
inline EGvoid*    EGValue::voidArray() const { return type.voidArray(addr); }
inline EGbool*    EGValue::boolArray() const { return type.boolArray(addr); }
inline EGbyte*    EGValue::byteArray() const { return type.byteArray(addr); }
inline EGubyte*   EGValue::ubyteArray() const { return type.ubyteArray(addr); }
inline EGshort*   EGValue::shortArray() const { return type.shortArray(addr); }
inline EGushort*  EGValue::ushortArray() const { return type.ushortArray(addr); }
inline EGint*     EGValue::intArray() const { return type.intArray(addr); }
inline EGuint*    EGValue::uintArray() const { return type.uintArray(addr); }
inline EGlong*    EGValue::longArray() const { return type.longArray(addr); }
inline EGulong*   EGValue::ulongArray() const { return type.ulongArray(addr); }
inline EGllong*   EGValue::llongArray() const { return type.llongArray(addr); }
inline EGullong*  EGValue::ullongArray() const { return type.ullongArray(addr); }
inline EGfloat*   EGValue::floatArray() const { return type.floatArray(addr); }
inline EGdouble*  EGValue::doubleArray() const { return type.doubleArray(addr); }


/* EGClass */

class EGClass
{
protected:
    friend class EGObject;
    
    const std::string name;
    const std::type_info *classInfo;
    const EGClass *baseClazz;
    
    EGConstructorPtr construct;
    EGPropertyMap properties;
    EGMessageMap messages;
    EGEmitterMap emitters;
    EGPropertyPtr lastprop;
    
    EGClass(std::string name, const std::type_info *classInfo);
    
    void addProperty(EGPropertyPtr property);
    void addMessage(EGMessagePtr message);
    void addEmitter(EGEmitterPtr emitter);
    
public:
    static EGClassTypeInfoMap& getClassTypeInfoMap();
    static EGClassPtr getClassByType(const std::type_info *classInfo);
    
    EGstring toString() const;
    const EGstring& getName() const;
    const EGPropertyMap& getProperties() const;
    const EGMessageMap& getMessages() const;
    const EGEmitterMap& getEmitters() const;
    
    template <class T>
    static EGClassPtr getClassByType();
    
    template <class T>
    EGClass* extends();
    
    template <class T>
    EGClass* constructor(T* (*constructorfunc)());

    template <class T, typename R>
    EGClass* property(EGstring name, R T::*M);
    
    template <class T, typename R>
    EGClass* property(EGstring name, R T::*M, EGAccess access);
    
    template <class T, typename R>
    EGClass* property(EGstring name, R (T::*getter)(), void (T::*setter)(R) = NULL);
    
    template <class T>
    EGClass* message(EGstring name, void (T::*msgfunc)());

    template <class T, typename A1>
    EGClass* message(EGstring name, void (T::*msgfunc)(A1));
    
    template <class T, typename A1, typename A2>
    EGClass* message(EGstring name, void (T::*msgfunc)(A1, A2));
    
    template <class T, typename A1, typename A2, typename A3>
    EGClass* message(EGstring name, void (T::*msgfunc)(A1, A2, A3));

    template <class T, typename A1, typename A2, typename A3, typename A4>
    EGClass* message(EGstring name, void (T::*msgfunc)(A1, A2, A3, A4));

    template <class T>
    EGClass* emitter(EGstring name, void (T::*emitfunc)());
    
    template <class T, typename A1>
    EGClass* emitter(EGstring name, void (T::*emitfunc)(A1));
    
    template <class T, typename A1, typename A2>
    EGClass* emitter(EGstring name, void (T::*emitfunc)(A1, A2));
    
    template <class T, typename A1, typename A2, typename A3>
    EGClass* emitter(EGstring name, void (T::*emitfunc)(A1, A2, A3));

    template <class T, typename A1, typename A2, typename A3, typename A4>
    EGClass* emitter(EGstring name, void (T::*emitfunc)(A1, A2, A3, A4));

    template <typename R>
    EGClass* minVal(R minval);
    
    template <typename R>
    EGClass* maxVal(R maxVal);
    
    template <class T>
    static EGClass* clazz();
    
    template <class T>
    static EGClass* clazz(EGstring name);
};


/* EGConstructorImpl */

class EGConstructorImpl
{
private:
    const EGClass *clazz;
    
public:
    EGConstructorImpl(const EGClass *clazz) : clazz(clazz) {}
    virtual ~EGConstructorImpl() {}

    const EGClass* getClass() const { return clazz; }
};


/* EGConstructor0Arg */

template <typename T>
class EGConstructor0Arg : public EGConstructorImpl
{
public:
    typedef T* (*ConstructorFunctionPtr)();
    
    ConstructorFunctionPtr func;
    
    EGConstructor0Arg(const EGClass *clazz, T* (*func)()) : EGConstructorImpl(clazz), func(func) {}
    virtual ~EGConstructor0Arg() {}
};


/* EGFunctionImpl */

class EGFunctionImpl
{
private:
    const EGClass *clazz;
    const EGstring name;
    
public:
    EGFunctionImpl(const EGClass *clazz, const EGstring &name) : clazz(clazz), name(name) {}
    virtual ~EGFunctionImpl() {}
    
    const EGClass* getClass() const { return clazz; }
    const EGstring& getName() const { return name; }
    
    virtual const EGType* getReturnType() const = 0;
    virtual std::vector<const EGType*> getArgumentTypes() const = 0;
    virtual std::vector<size_t> getArgumentElements() const = 0;
    
    EGstring getTypeSignature() const
    {
        std::stringstream ss;
        ss << getReturnType()->name() << "(";
        std::vector<const EGType*> argTypes = getArgumentTypes();
        for (size_t i = 0; i < argTypes.size(); i++) {
            if (i > 0) ss << ",";
            ss << argTypes[i]->name();
        }
        ss << ")";
        return ss.str();
    }
    
    EGstring getLongName() const
    {
        std::stringstream ss;
        ss << getReturnType()->name() << " ";
        if (clazz) {
            ss << clazz->getName() << "::";
        }
        ss << name << "(";
        std::vector<const EGType*> argTypes = getArgumentTypes();
        std::vector<size_t> argElements = getArgumentElements();
        for (size_t i = 0; i < argTypes.size(); i++) {
            if (i > 0) ss << ",";
            ss << argTypes[i]->name();
            if (argElements[i] > 0) ss << "[" << argElements[i] << "]";
        }
        ss << ")";
        return ss.str();
    }
};



/* EGFunctionApply0ArgVoidReturn */

class EGFunctionApply0ArgVoidReturn : public EGFunctionImpl
{
public:
    EGFunctionApply0ArgVoidReturn(const EGClass *clazz, const EGstring &name) : EGFunctionImpl(clazz, name) {}
    virtual ~EGFunctionApply0ArgVoidReturn() {}
    
    virtual void apply(void *obj) = 0;
    
    const EGType* getReturnType() const { return &EGType::typeOf<EGvoid>(); }
    
    std::vector<const EGType*> getArgumentTypes() const
    {
        return std::vector<const EGType*>();
    }
    
    std::vector<size_t> getArgumentElements() const
    {
        return std::vector<size_t>();
    }
};


/* EGFunctionApply1ArgVoidReturn */

template <typename A1>
class EGFunctionApply1ArgVoidReturn : public EGFunctionImpl
{
public:
    EGFunctionApply1ArgVoidReturn(const EGClass *clazz, const EGstring &name) : EGFunctionImpl(clazz, name) {}
    virtual ~EGFunctionApply1ArgVoidReturn() {}
    
    virtual void apply(void *obj, A1 &arg1) = 0;
    
    const EGType* getReturnType() const { return &EGType::typeOf<EGvoid>(); }
    
    std::vector<const EGType*> getArgumentTypes() const
    {
        std::vector<const EGType*> types;
        types.push_back(&EGType::typeOf<A1>());
        return types;
    }
    
    std::vector<size_t> getArgumentElements() const
    {
        std::vector<size_t> nelems;
        nelems.push_back(array_extent<A1>());
        return nelems;
    }
};


/* EGFunctionApply2ArgVoidReturn */

template <typename A1, typename A2>
class EGFunctionApply2ArgVoidReturn : public EGFunctionImpl
{
public:
    EGFunctionApply2ArgVoidReturn(const EGClass *clazz, const EGstring &name) : EGFunctionImpl(clazz, name) {}
    virtual ~EGFunctionApply2ArgVoidReturn() {}
    
    virtual void apply(void *obj, A1 &arg1, A2 &arg2) = 0;
    
    
    const EGType* getReturnType() const { return &EGType::typeOf<EGvoid>(); }
    
    std::vector<const EGType*> getArgumentTypes() const
    {
        std::vector<const EGType*> types;
        types.push_back(&EGType::typeOf<A1>());
        types.push_back(&EGType::typeOf<A2>());
        return types;
    }
    
    std::vector<size_t> getArgumentElements() const
    {
        std::vector<size_t> nelems;
        nelems.push_back(array_extent<A1>());
        nelems.push_back(array_extent<A2>());
        return nelems;
    }
};


/* EGFunctionApply3ArgVoidReturn */

template <typename A1, typename A2, typename A3>
class EGFunctionApply3ArgVoidReturn : public EGFunctionImpl
{
public:
    EGFunctionApply3ArgVoidReturn(const EGClass *clazz, const EGstring &name) : EGFunctionImpl(clazz, name) {}
    virtual ~EGFunctionApply3ArgVoidReturn() {}
    
    virtual void apply(void *obj, A1 &arg1, A2 &arg2, A3 &arg3) = 0;
    
    
    const EGType* getReturnType() const { return &EGType::typeOf<EGvoid>(); }
    
    std::vector<const EGType*> getArgumentTypes() const
    {
        std::vector<const EGType*> types;
        types.push_back(&EGType::typeOf<A1>());
        types.push_back(&EGType::typeOf<A2>());
        types.push_back(&EGType::typeOf<A3>());
        return types;
    }
    
    std::vector<size_t> getArgumentElements() const
    {
        std::vector<size_t> nelems;
        nelems.push_back(array_extent<A1>());
        nelems.push_back(array_extent<A2>());
        nelems.push_back(array_extent<A3>());
        return nelems;
    }
};


/* EGFunctionApply4ArgVoidReturn */

template <typename A1, typename A2, typename A3, typename A4>
class EGFunctionApply4ArgVoidReturn : public EGFunctionImpl
{
public:
    EGFunctionApply4ArgVoidReturn(const EGClass *clazz, const EGstring &name) : EGFunctionImpl(clazz, name) {}
    virtual ~EGFunctionApply4ArgVoidReturn() {}
    
    virtual void apply(void *obj, A1 &arg1, A2 &arg2, A3 &arg3, A4 &arg4) = 0;
    
    const EGType* getReturnType() const
    {
        return &EGType::typeOf<EGvoid>();
    }
    
    std::vector<const EGType*> getArgumentTypes() const
    {
        std::vector<const EGType*> types;
        types.push_back(&EGType::typeOf<A1>());
        types.push_back(&EGType::typeOf<A2>());
        types.push_back(&EGType::typeOf<A3>());
        types.push_back(&EGType::typeOf<A4>());
        return types;
    }
    
    std::vector<size_t> getArgumentElements() const
    {
        std::vector<size_t> nelems;
        nelems.push_back(array_extent<A1>());
        nelems.push_back(array_extent<A2>());
        nelems.push_back(array_extent<A3>());
        nelems.push_back(array_extent<A4>());
        return nelems;
    }
};


/* EGFunctionObject0ArgVoidReturn */

class EGFunctionObject0ArgVoidReturn : public EGFunctionApply0ArgVoidReturn
{
public:
    typedef std::function<void()> FunctionObject;
    
    FunctionObject func;
    
    EGFunctionObject0ArgVoidReturn(const EGstring &name, FunctionObject func)
        : EGFunctionApply0ArgVoidReturn(NULL, name), func(func) {}
    
    void apply(void *) { func(); }
};


/* EGFunctionObject1ArgVoidReturn */

template <typename A1>
class EGFunctionObject1ArgVoidReturn : public EGFunctionApply1ArgVoidReturn<A1>
{
public:
    typedef std::function<void(A1)> FunctionObject;
    
    FunctionObject func;
    
    EGFunctionObject1ArgVoidReturn(const EGstring &name, FunctionObject func)
        : EGFunctionApply1ArgVoidReturn<A1>(NULL, name), func(func) {}
    
    void apply(void *, A1 &arg1) { func(arg1); }
};

/* EGFunctionObject2ArgVoidReturn */

template <typename A1, typename A2>
class EGFunctionObject2ArgVoidReturn : public EGFunctionApply2ArgVoidReturn<A1, A2>
{
public:
    typedef std::function<void(A1, A2)> FunctionObject;
    
    FunctionObject func;
    
    EGFunctionObject2ArgVoidReturn(const EGstring &name, FunctionObject func)
        : EGFunctionApply2ArgVoidReturn<A1, A2>(NULL, name), func(func) {}
    
    void apply(void *, A1 &arg1, A2 &arg2) { func(arg1, arg2); }
};


/* EGFunctionObject3ArgVoidReturn */

template <typename A1, typename A2, typename A3>
class EGFunctionObject3ArgVoidReturn : public EGFunctionApply3ArgVoidReturn<A1, A2, A3>
{
public:
    typedef std::function<void(A1, A2, A3)> FunctionObject;
    
    FunctionObject func;
    
    EGFunctionObject3ArgVoidReturn(const EGstring &name, FunctionObject func)
        : EGFunctionApply3ArgVoidReturn<A1, A2, A3>(NULL, name), func(func) {}
    
    void apply(void *, A1 &arg1, A2 &arg2, A3 &arg3) { func(arg1, arg2, arg3); }
};


/* EGFunctionObject4ArgVoidReturn */

template <typename A1, typename A2, typename A3, typename A4>
class EGFunctionObject4ArgVoidReturn : public EGFunctionApply4ArgVoidReturn<A1, A2, A3, A4>
{
public:
    typedef std::function<void(A1, A2, A3, A4)> FunctionObject;
    
    FunctionObject func;
    
    EGFunctionObject4ArgVoidReturn(const EGstring &name, FunctionObject func)
        : EGFunctionApply4ArgVoidReturn<A1, A2, A3, A4>(NULL, name), func(func) {}
    
    void apply(void *, A1 &arg1, A2 &arg2, A3 &arg3, A4 &arg4) { func(arg1, arg2, arg3, arg4); }
};


/* EGMemberFunction0ArgVoidReturn */

template <class T>
class EGMemberFunction0ArgVoidReturn : public EGFunctionApply0ArgVoidReturn
{
public:
    typedef void (T::*MemberFunctionPtr)();
    
    MemberFunctionPtr func;
    
    EGMemberFunction0ArgVoidReturn(const EGClass *clazz, const EGstring &name, MemberFunctionPtr func)
        : EGFunctionApply0ArgVoidReturn(clazz, name), func(func) {}
    
    void apply(void *obj) { (static_cast<T*>(obj)->*func)(); }
};


/* EGMemberFunction1ArgVoidReturn */

template <class T, typename A1>
class EGMemberFunction1ArgVoidReturn : public EGFunctionApply1ArgVoidReturn<A1>
{
public:
    typedef void (T::*MemberFunctionPtr)(A1);
    
    MemberFunctionPtr func;
    
    EGMemberFunction1ArgVoidReturn(const EGClass *clazz, const EGstring &name, MemberFunctionPtr func)
        : EGFunctionApply1ArgVoidReturn<A1>(clazz, name), func(func) {}
    
    void apply(void *obj, A1 &arg1) { (static_cast<T*>(obj)->*func)(arg1); }
};


/* EGMemberFunction2ArgVoidReturn */

template <class T, typename A1, typename A2>
class EGMemberFunction2ArgVoidReturn : public EGFunctionApply2ArgVoidReturn<A1, A2>
{
public:
    typedef void (T::*MemberFunctionPtr)(A1, A2);
    
    MemberFunctionPtr func;
    
    EGMemberFunction2ArgVoidReturn(const EGClass *clazz, const EGstring &name, MemberFunctionPtr func)
        : EGFunctionApply2ArgVoidReturn<A1, A2>(clazz, name), func(func) {}
    
    void apply(void *obj, A1 &arg1, A2 &arg2) { (static_cast<T*>(obj)->*func)(arg1, arg2); }
};


/* EGMemberFunction3ArgVoidReturn */

template <class T, typename A1, typename A2, typename A3>
class EGMemberFunction3ArgVoidReturn : public EGFunctionApply3ArgVoidReturn<A1, A2, A3>
{
public:
    typedef void (T::*MemberFunctionPtr)(A1, A2, A3);
    
    MemberFunctionPtr func;
    
    EGMemberFunction3ArgVoidReturn(const EGClass *clazz, const EGstring &name, MemberFunctionPtr func)
        : EGFunctionApply3ArgVoidReturn<A1, A2, A3>(clazz, name), func(func) {}
    
    void apply(void *obj, A1 &arg1, A2 &arg2, A3 &arg3) { (static_cast<T*>(obj)->*func)(arg1, arg2, arg3); }
};


/* EGMemberFunction4ArgVoidReturn */

template <class T, typename A1, typename A2, typename A3, typename A4>
class EGMemberFunction4ArgVoidReturn : public EGFunctionApply4ArgVoidReturn<A1, A2, A3, A4>
{
public:
    typedef void (T::*MemberFunctionPtr)(A1, A2, A3, A4);
    
    MemberFunctionPtr func;
    
    EGMemberFunction4ArgVoidReturn(const EGClass *clazz, const EGstring &name, MemberFunctionPtr func)
        : EGFunctionApply4ArgVoidReturn<A1, A2, A3, A4>(clazz, name), func(func) {}
    
    void apply(void *obj, A1 &arg1, A2 &arg2, A3 &arg3, A4 &arg4) { (static_cast<T*>(obj)->*func)(arg1, arg2, arg3, arg4); }
};


/* EGObject */

class EGObject
{
private:
    EGEmitterConnectionList connections;
    
public:
    static const EGbool debug;
    
    virtual ~EGObject();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    
    virtual void destroyed(EGObject *object); /* emitter */
    
    EGstring toString() const;
    
    std::vector<EGstring> keys() const;
    
    EGstring getString(const EGPropertyPtr &prop) const;
    void setString(const EGPropertyPtr &prop, EGstring &val);
    
    EGValue getValue(const EGPropertyPtr &prop) const;
    void setValue(const EGPropertyPtr &prop, EGValue &val);
    
    EGstring getString(EGstring propname) const;
    void setString(EGstring propname, EGstring val);
    
    EGValue getValue(EGstring propname) const;
    void setValue(EGstring propname, EGValue val);
    
    template <class T>
    void emit(void (T::*emitfunc)());
    
    template <class T, typename A1>
    void emit(void (T::*emitfunc)(A1), A1 arg1);
    
    template <class T, typename A1, typename A2>
    void emit(void (T::*emitfunc)(A1, A2), A1 arg1, A2 arg2);
    
    template <class T, typename A1, typename A2, typename A3>
    void emit(void (T::*emitfunc)(A1, A2, A3), A1 arg1, A2 arg2, A3 arg3);
    
    template <class T, typename A1, typename A2, typename A3, typename A4>
    void emit(void (T::*emitfunc)(A1, A2, A3, A4), A1 arg1, A2 arg2, A3 arg3, A4 arg4);
    
    static EGbool connect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                          const EGFunctionPtr &func);
    static EGbool connect(EGObject *sourceObject, EGstring emitterName,
                          const EGFunctionPtr &func);
    static EGbool connect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                          EGObject *destObject, const EGPropertyPtr &property);
    static EGbool connect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                          EGObject *destObject, const EGMessagePtr &message);
    static EGbool connect(EGObject *sourceObject, EGstring emitterName,
                          EGObject *destObject, EGstring targetName);
    
    static EGbool disconnect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                             const EGFunctionPtr &func);
    static EGbool disconnect(EGObject *sourceObject, EGstring emitterName,
                             const EGFunctionPtr &func);
    static EGbool disconnect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                             EGObject *destObject, const EGPropertyPtr &property);
    static EGbool disconnect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                             EGObject *destObject, const EGMessagePtr &message);
    static EGbool disconnect(EGObject *sourceObject, EGstring emitterName,
                             EGObject *destObject, EGstring targetName);
    
    
    static inline EGbool connect(EGObjectPtr sourceObject, const EGEmitterPtr &emitter,
                                 const EGFunctionPtr &func)
    { return connect(sourceObject.get(), emitter, func); }

    static inline EGbool connect(EGObjectPtr sourceObject, EGstring emitterName,
                                 const EGFunctionPtr &func)
    { return connect(sourceObject.get(), emitterName, func); }

    static inline EGbool connect(EGObjectPtr sourceObject, const EGEmitterPtr &emitter,
                                 EGObjectPtr destObject, const EGPropertyPtr &property)
    { return connect(sourceObject.get(), emitter, destObject.get(), property); }
    
    static inline EGbool connect(EGObjectPtr sourceObject, const EGEmitterPtr &emitter,
                                 EGObjectPtr destObject, const EGMessagePtr &message)
    { return connect(sourceObject.get(), emitter, destObject.get(), message); }
    
    static inline EGbool connect(EGObjectPtr sourceObject, EGstring emitterName,
                                 EGObjectPtr destObject, EGstring targetName)
    { return connect(sourceObject.get(), emitterName, destObject.get(), targetName); }
    
    
    static inline EGbool disconnect(EGObjectPtr sourceObject, const EGEmitterPtr &emitter,
                                    const EGFunctionPtr &func)
    { return disconnect(sourceObject.get(), emitter, func); }

    static inline EGbool disconnect(EGObjectPtr sourceObject, EGstring emitterName,
                                 const EGFunctionPtr &func)
    { return disconnect(sourceObject.get(), emitterName, func); }

    static inline EGbool disconnect(EGObjectPtr sourceObject, const EGEmitterPtr &emitter,
                                    EGObjectPtr destObject, const EGPropertyPtr &property)
    { return disconnect(sourceObject.get(), emitter, destObject.get(), property); }
    
    static inline EGbool disconnect(EGObjectPtr sourceObject, const EGEmitterPtr &emitter,
                                    EGObjectPtr destObject, const EGMessagePtr &message)
    { return disconnect(sourceObject.get(), emitter, destObject.get(), message); }

    static inline EGbool disconnect(EGObjectPtr sourceObject, EGstring emitterName,
                                    EGObjectPtr destObject, EGstring targetName)
    { return disconnect(sourceObject.get(), emitterName, destObject.get(), targetName); }
    
    
    inline EGbool connect(EGstring emitterName, const EGFunctionPtr &func)
    { return connect(this, emitterName, func); }
    
    inline EGbool connect(const EGEmitterPtr &emitter, const EGFunctionPtr &func)
    { return connect(this, emitter, func); }

    inline EGbool connect(const EGEmitterPtr &emitter, EGObject *destObject, const EGPropertyPtr &property)
    { return connect(this, emitter, destObject, property); }
    
    inline EGbool connect(const EGEmitterPtr &emitter, EGObject *destObject, const EGMessagePtr &message)
    { return connect(this, emitter, destObject, message); }
    
    inline EGbool connect(EGstring emitterName, EGObject *destObject, EGstring targetName)
    { return connect(this, emitterName, destObject, targetName); }

    inline EGbool connect(const EGEmitterPtr &emitter, EGObjectPtr destObject, const EGPropertyPtr &property)
    { return connect(this, emitter, destObject.get(), property); }
    
    inline EGbool connect(const EGEmitterPtr &emitter, EGObjectPtr destObject, const EGMessagePtr &message)
    { return connect(this, emitter, destObject.get(), message); }
    
    inline EGbool connect(EGstring emitterName, EGObjectPtr destObject, EGstring targetName)
    { return connect(this, emitterName, destObject.get(), targetName); }


    inline EGbool disconnect(EGstring emitterName, const EGFunctionPtr &func)
    { return disconnect(this, emitterName, func); }
    
    inline EGbool disconnect(const EGEmitterPtr &emitter, const EGFunctionPtr &func)
    { return disconnect(this, emitter, func); }

    inline EGbool disconnect(const EGEmitterPtr &emitter, EGObject *destObject, const EGPropertyPtr &property)
    { return disconnect(this, emitter, destObject, property); }
    
    inline EGbool disconnect(const EGEmitterPtr &emitter, EGObject *destObject, const EGMessagePtr &message)
    { return disconnect(this, emitter, destObject, message); }
    
    inline EGbool disconnect(EGstring emitterName, EGObject *destObject, EGstring targetName)
    { return disconnect(this, emitterName, destObject, targetName); }
    
    inline EGbool disconnect(const EGEmitterPtr &emitter, EGObjectPtr destObject, const EGPropertyPtr &property)
    { return disconnect(this, emitter, destObject.get(), property); }
    
    inline EGbool disconnect(const EGEmitterPtr &emitter, EGObjectPtr destObject, const EGMessagePtr &message)
    { return disconnect(this, emitter, destObject.get(), message); }
    
    inline EGbool disconnect(EGstring emitterName, EGObjectPtr destObject, EGstring targetName)
    { return disconnect(this, emitterName, destObject.get(), targetName); }
    
    
    /* connect to lambda / function object / pointer to function */

	inline EGbool connect(EGstring emitterName, std::function<void()> func)
    { return connect(emitterName, EGFunctionPtr(new EGFunctionObject0ArgVoidReturn(typeid(func).name(), func))); }
    
    template <typename A1>
	inline EGbool connect(EGstring emitterName, std::function<void(A1)> func)
    { return connect(emitterName, EGFunctionPtr(new EGFunctionObject1ArgVoidReturn<A1>(typeid(func).name(), func))); }

    template <typename A1, typename A2>
	inline EGbool connect(EGstring emitterName, std::function<void(A1, A2)> func)
    { return connect(emitterName, EGFunctionPtr(new EGFunctionObject2ArgVoidReturn<A1, A2>(typeid(func).name(), func))); }

    template <typename A1, typename A2, typename A3>
	inline EGbool connect(EGstring emitterName, std::function<void(A1, A2, A3)> func)
    { return connect(emitterName, EGFunctionPtr(new EGFunctionObject3ArgVoidReturn<A1, A2, A3>(typeid(func).name(), func))); }

    template <typename A1, typename A2, typename A3, typename A4>
	inline EGbool connect(EGstring emitterName, std::function<void(A1, A2, A3, A4)> func)
    { return connect(emitterName, EGFunctionPtr(new EGFunctionObject4ArgVoidReturn<A1, A2, A3, A4>(typeid(func).name(), func))); }

    
    virtual void disconnect(EGObject* destObject);
    
    virtual void connectDestroyedDisconnect(EGObject* destObject);
};


/* EGConstraint */

class EGConstraintImpl
{
public:
    virtual ~EGConstraintImpl();
    
    virtual EGbool evaluate(EGValue &val) const = 0;
    virtual EGstring toString() const = 0;
};

template <typename R>
class EGConstraintMin : public EGConstraintImpl
{
public:
    R minVal;
    
    EGConstraintMin(R minVal) : minVal(minVal) {}

    EGbool evaluate(EGValue &val) const
    {
        return val.value<R>() >= minVal;
    }
    
    EGstring toString() const
    {
        std::stringstream ss;
        ss << "(>=" << minVal << ")";
        return ss.str();
    }
};

template <typename R>
class EGConstraintMax : public EGConstraintImpl
{
public:
    R maxVal;
    
    EGConstraintMax(R maxVal) : maxVal(maxVal) {}
    
    EGbool evaluate(EGValue &val) const
    {
        return val.value<R>() <= maxVal;
    }

    EGstring toString() const
    {
        std::stringstream ss;
        ss << "(<=" << maxVal << ")";
        return ss.str();
    }
};


/* EGPropertyImpl */

class EGPropertyImpl
{
protected:
    friend class EGObject;
    friend class EGClass;
    
    const EGClass *clazz;
    const EGstring name;
    const EGType &type;
    const EGAccess access;
    const size_t nelem;
    EGConstraintList constraints;
    
public:
    virtual ~EGPropertyImpl() {}
    
    const EGClass* getClass() { return clazz; }
    const EGstring& getName() { return name; }
    const EGType& getType() { return type; }
    const EGAccess& getAccess() { return access; }
    const size_t getElements() { return nelem; }
    const EGConstraintList& getConstraints() { return constraints; }
    
    virtual EGstring toString() const = 0;
    virtual EGstring getString(void *obj) const = 0;
    virtual EGstring getStringJSON(void *obj) const = 0;
    virtual void setString(void *obj, EGstring &val) = 0;
    virtual EGValue getValue(void *obj) const = 0;
    virtual void setValue(void *obj, EGValue &val) = 0;
    
protected:
    EGPropertyImpl(const EGClass *clazz, EGstring name, const EGType &type, const EGAccess access, const size_t nelem) :
        clazz(clazz), name(name), type(type), access(access), nelem(nelem) {}
};


/* EGPropertyField */

template <class T, typename R> size_t offset_of(R T::*M) { return reinterpret_cast<std::size_t>(&(((T*)0)->*M)); }

template <class T, typename R>
class EGPropertyField : public EGPropertyImpl
{
public:
    const EGoffset offset;

    EGstring toString() const
    {
        std::stringstream ss;
        ss << "{ \"property\": \"field\", \"name\": \"" << name << "\", \"type\": \"" << type.name() << "\", \"offset\": " << offset << ", \"nelem\": " << nelem << ", \"access\": \"" << (access == EGAccessReadWrite ? "readwrite" : "readonly") << "\" }";
        return ss.str();
    }
    
    EGstring getString(void *obj) const
    {
        return type.toString((void*)((EGubyte*)obj + offset), nelem);
    }
    
    EGstring getStringJSON(void *obj) const
    {
        return type.toJSONString((void*)((EGsize)obj + offset), nelem);
    }
    
    void setString(void *obj, EGstring &val)
    {
        type.fromString((void*)((EGubyte*)obj + offset), val, nelem);
    }
    
    EGValue getValue(void *obj) const
    {
        return type.toValue((void*)((EGubyte*)obj + offset), nelem);
    }
    
    void setValue(void *obj, EGValue &val)
    {
        type.fromValue((void*)((EGubyte*)obj + offset), val, nelem);
    }

    EGPropertyField(EGClass *clazz, EGstring name, R T::*M)
        : EGPropertyImpl(clazz, name, EGType::typeOf<R>(), std::is_const<R>::value ? EGAccessReadOnly : EGAccessReadWrite, array_extent<R>()),
          offset(offset_of<T,R>(M)) {}
    EGPropertyField(EGClass *clazz, EGstring name, R T::*M, EGAccess access)
        : EGPropertyImpl(clazz, name, EGType::typeOf<R>(), access, array_extent<R>()),
          offset(offset_of<T,R>(M)) {}
};


/* EGPropertyAccessor */

template <class T, typename R>
class EGPropertyAccessor : public EGPropertyImpl
{
public:
    R (T::*getter)();
    void (T::*setter)(R);
    
    EGstring toString() const
    {
        std::stringstream ss;
        ss << "{ \"property\": \"property\", \"name\": \"" << name << "\", \"type\": \"" << type.name() << "\", \"nelem\": " << nelem << ", \"access\": \"" << (access == EGAccessReadWrite ? "readwrite" : "readonly") << "\" }";
        return ss.str();
    }
    
    EGstring getString(void *obj) const
    {
        R v = (static_cast<T*>(obj)->*getter)();
        return type.toString((void*)&v, nelem);
    }
    
    EGstring getStringJSON(void *obj) const
    {
        R v = (static_cast<T*>(obj)->*getter)();
        return type.toJSONString((void*)&v, nelem);
    }
    
    void setString(void *obj, EGstring &val)
    {
        R v;
        type.fromString((void*)&v, val, nelem);
        (static_cast<T*>(obj)->*setter)(v);
    }
    
    EGValue getValue(void *obj) const
    {
        R v = (static_cast<T*>(obj)->*getter)();
        return type.toValue((void*)&v, nelem);
    }
    
    void setValue(void *obj, EGValue &val)
    {
        R v;
        type.fromValue((void*)&v, val, nelem);
        (static_cast<T*>(obj)->*setter)(v);
    }
        
    EGPropertyAccessor(const EGClass* clazz, EGstring name, R (T::*getter)(), void (T::*setter)(R) = NULL)
        : EGPropertyImpl(clazz, name, EGType::typeOf<R>(), setter == NULL ? EGAccessReadOnly : EGAccessReadWrite, array_extent<R>()),
          getter(getter), setter(setter) {}
};


/* EGMessageImpl */

class EGMessageImpl
{
private:
    EGFunctionPtr func;
    
public:
    EGMessageImpl(EGFunctionImpl *func) : func(func) {}
    
    const EGFunctionPtr& getFunction() const { return func; }
};

    
/* EGEmitterImpl */

class EGEmitterImpl
{
private:
    EGFunctionPtr func;
    
public:
    EGEmitterImpl(EGFunctionImpl *func) : func(func) {}
    
    const EGFunctionPtr& getFunction() const { return func; }
};


/* EGClass */

template <class T>
EGClassPtr EGClass::getClassByType() { return getClassByType(&typeid(T)); }

template <class T>
EGClass* EGClass::extends()
{
    const EGClass *inheritClazz = baseClazz = T::classFactory();
    while (inheritClazz != NULL) {
        for (EGPropertyMap::const_iterator pi = inheritClazz->properties.begin(); pi != inheritClazz->properties.end(); pi++) {
            properties.insert(*pi);
        }
        for (EGMessageMap::const_iterator mi = inheritClazz->messages.begin(); mi != inheritClazz->messages.end(); mi++) {
            messages.insert(*mi);
        }
        for (EGEmitterMap::const_iterator ei = inheritClazz->emitters.begin(); ei != inheritClazz->emitters.end(); ei++) {
            emitters.insert(*ei);
        }
        inheritClazz = inheritClazz->baseClazz;
    }
    return this;
}
    
template <class T>
EGClass* EGClass::constructor(T* (*constructorfunc)())
{
    construct = EGConstructorPtr(new EGConstructor0Arg<T>(clazz, constructorfunc));
    return this;
}

template <class T, typename R>
EGClass* EGClass::property(EGstring name, R T::*M)
{
    addProperty(EGPropertyPtr(new EGPropertyField<T,R>(this, name, M)));
    return this;
}

template <class T, typename R>
EGClass* EGClass::property(EGstring name, R T::*M, EGAccess access)
{
    addProperty(withProperty(new EGPropertyField<T,R>(this, name, M, access)));
    return this;
}

template <class T, typename R>
EGClass* EGClass::property(EGstring name, R (T::*getter)(), void (T::*setter)(R))
{
    addProperty(EGPropertyPtr(new EGPropertyAccessor<T,R>(this, name, getter, setter)));
    return this;
}

template <class T>
EGClass* EGClass::message(EGstring name, void (T::*msgfunc)())
{
    addMessage(EGMessagePtr(new EGMessageImpl(new EGMemberFunction0ArgVoidReturn<T>(this, name, msgfunc))));
    return this;
}

template <class T, typename A1>
EGClass* EGClass::message(EGstring name, void (T::*msgfunc)(A1))
{
    addMessage(EGMessagePtr(new EGMessageImpl(new EGMemberFunction1ArgVoidReturn<T, A1>(this, name, msgfunc))));
    return this;
}

template <class T, typename A1, typename A2>
EGClass* EGClass::message(EGstring name, void (T::*msgfunc)(A1, A2))
{
    addMessage(EGMessagePtr(new EGMessageImpl(new EGMemberFunction2ArgVoidReturn<T, A1, A2>(this, name, msgfunc))));
    return this;
}

template <class T, typename A1, typename A2, typename A3>
EGClass* EGClass::message(EGstring name, void (T::*msgfunc)(A1, A2, A3))
{
    addMessage(EGMessagePtr(new EGMessageImpl(new EGMemberFunction3ArgVoidReturn<T, A1, A2, A3>(this, name, msgfunc))));
    return this;
}

template <class T, typename A1, typename A2, typename A3, typename A4>
EGClass* EGClass::message(EGstring name, void (T::*msgfunc)(A1, A2, A3, A4))
{
    addMessage(EGMessagePtr(new EGMessageImpl(new EGMemberFunction4ArgVoidReturn<T, A1, A2, A3, A4>(this, name, msgfunc))));
    return this;
}

template <class T>
EGClass* EGClass::emitter(EGstring name, void (T::*emitfunc)())
{
    addEmitter(EGEmitterPtr(new EGEmitterImpl(new EGMemberFunction0ArgVoidReturn<T>(this, name, emitfunc))));
    return this;
}

template <class T, typename A1>
EGClass* EGClass::emitter(EGstring name, void (T::*emitfunc)(A1))
{
    addEmitter(EGEmitterPtr(new EGEmitterImpl(new EGMemberFunction1ArgVoidReturn<T, A1>(this, name, emitfunc))));
    return this;
}

template <class T, typename A1, typename A2>
EGClass* EGClass::emitter(EGstring name, void (T::*emitfunc)(A1, A2))
{
    addEmitter(EGEmitterPtr(new EGEmitterImpl(new EGMemberFunction2ArgVoidReturn<T, A1, A2>(this, name, emitfunc))));
    return this;
}

template <class T, typename A1, typename A2, typename A3>
EGClass* EGClass::emitter(EGstring name, void (T::*emitfunc)(A1, A2, A3))
{
    addEmitter(EGEmitterPtr(new EGEmitterImpl(new EGMemberFunction3ArgVoidReturn<T, A1, A2, A3>(this, name, emitfunc))));
    return this;
}

template <class T, typename A1, typename A2, typename A3, typename A4>
EGClass* EGClass::emitter(EGstring name, void (T::*emitfunc)(A1, A2, A3, A4))
{
    addEmitter(EGEmitterPtr(new EGEmitterImpl(new EGMemberFunction4ArgVoidReturn<T, A1, A2, A3, A4>(this, name, emitfunc))));
    return this;
}

template <typename R>
EGClass* EGClass::minVal(R minval)
{
    lastprop->constraints.push_back(EGConstraintPtr(new EGConstraintMin<R>(minval)));
    return this;
}

template <typename R>
EGClass* EGClass::maxVal(R maxVal)
{
    lastprop->constraints.push_back(EGConstraintPtr(new EGConstraintMax<R>(maxVal)));
    return this;
}

template <class T>
EGClass* EGClass::clazz()
{
#if USE_CXXABI
    int status;
    char *realname = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
    EGClassPtr clazz(new EGClass(realname, &typeid(T)));
    free(realname);
#else
    EGClassPtr clazz(new EGClass(typeid(T).name(), &typeid(T)));
#endif
    EGClass::getClassTypeInfoMap().insert(std::pair<const std::type_info*,EGClassPtr>(clazz->classInfo, clazz));
    return clazz.get();
}

template <class T>
EGClass* EGClass::clazz(EGstring name)
{
    EGClassPtr clazz(new EGClass(name, &typeid(T)));
    EGClass::getClassTypeInfoMap().insert(std::pair<const std::type_info*,EGClassPtr>(clazz->classInfo, clazz));
    return clazz.get();
}


/* EGObject */ 

template <class T>
void EGObject::emit(void (T::*emitfunc)())
{
    typedef EGMemberFunction0ArgVoidReturn<T> FuncImpl;
    typedef EGFunctionApply0ArgVoidReturn CallImpl;
    for (EGEmitterConnectionList::iterator ci = connections.begin(); ci != connections.end(); ci++) {
        const EGFunctionPtr &emitterFunction = ci->emitter->getFunction();
        if (typeid(*emitterFunction) != typeid(FuncImpl)) continue;
        if (static_cast<FuncImpl&>(*emitterFunction).func != emitfunc) continue;
        static_cast<CallImpl&>(*ci->destFunction).apply(ci->destObject);
    }
}

template <class T, typename A1>
void EGObject::emit(void (T::*emitfunc)(A1), A1 arg1)
{
    typedef EGMemberFunction1ArgVoidReturn<T, A1> FuncImpl;
    typedef EGFunctionApply1ArgVoidReturn<A1> CallImpl;
    for (EGEmitterConnectionList::iterator ci = connections.begin(); ci != connections.end(); ci++) {
        const EGFunctionPtr &emitterFunction = ci->emitter->getFunction();
        if (typeid(*emitterFunction) != typeid(FuncImpl)) continue;
        if (static_cast<FuncImpl&>(*emitterFunction).func != emitfunc) continue;
        if (ci->destProperty) {
            std::vector<const EGType*> emitterArgTypes = emitterFunction->getArgumentTypes();
            std::vector<size_t> emitterArgElements = emitterFunction->getArgumentElements();
            EGValue v = emitterArgTypes.at(0)->toValue(&arg1, emitterArgElements.at(0));
            ci->destProperty->setValue(ci->destObject, v);
        } else {
            static_cast<CallImpl&>(*ci->destFunction).apply(ci->destObject, arg1);
        }
    }
}

template <class T, typename A1, typename A2>
void EGObject::emit(void (T::*emitfunc)(A1, A2), A1 arg1, A2 arg2)
{
    typedef EGMemberFunction2ArgVoidReturn<T, A1, A2> FuncImpl;
    typedef EGFunctionApply2ArgVoidReturn<A1, A2> CallImpl;
    for (EGEmitterConnectionList::iterator ci = connections.begin(); ci != connections.end(); ci++) {
        const EGFunctionPtr &emitterFunction = ci->emitter->getFunction();
        if (typeid(*emitterFunction) != typeid(FuncImpl)) continue;
        if (static_cast<FuncImpl&>(*emitterFunction).func != emitfunc) continue;
        static_cast<CallImpl&>(*ci->destFunction).apply(ci->destObject, arg1, arg2);
    }
}

template <class T, typename A1, typename A2, typename A3>
void EGObject::emit(void (T::*emitfunc)(A1, A2, A3), A1 arg1, A2 arg2, A3 arg3)
{
    typedef EGMemberFunction3ArgVoidReturn<T, A1, A2, A3> FuncImpl;
    typedef EGFunctionApply3ArgVoidReturn<A1, A2, A3> CallImpl;
    for (EGEmitterConnectionList::iterator ci = connections.begin(); ci != connections.end(); ci++) {
        const EGFunctionPtr &emitterFunction = ci->emitter->getFunction();
        if (typeid(*emitterFunction) != typeid(FuncImpl)) continue;
        if (static_cast<FuncImpl&>(*emitterFunction).func != emitfunc) continue;
        static_cast<CallImpl&>(*ci->destFunction).apply(ci->destObject, arg1, arg2, arg3);
    }
}

template <class T, typename A1, typename A2, typename A3, typename A4>
void EGObject::emit(void (T::*emitfunc)(A1, A2, A3, A4), A1 arg1, A2 arg2, A3 arg3, A4 arg4)
{
    typedef EGMemberFunction4ArgVoidReturn<T, A1, A2, A3, A4> FuncImpl;
    typedef EGFunctionApply4ArgVoidReturn<A1, A2, A3, A4> CallImpl;
    for (EGEmitterConnectionList::iterator ci = connections.begin(); ci != connections.end(); ci++) {
        const EGFunctionPtr &emitterFunction = ci->emitter->getFunction();
        if (typeid(*emitterFunction) != typeid(FuncImpl)) continue;
        if (static_cast<FuncImpl&>(*emitterFunction).func != emitfunc) continue;
        static_cast<CallImpl&>(*ci->destFunction).apply(ci->destObject, arg1, arg2, arg3, arg4);
    }
}
