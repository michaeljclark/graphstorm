/*
 *  testobject.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGObject.h"

/*
 * C++ introspection and late binding example
 *
 * EGClass uses function template type inference to deduce field types
 * EGClass creates field accessor wrappers with type coercion support.
 * EGClass deduces array dimensions and constness (readonly accesors).
 * EGClass allows addition of field value constraints.
 * EGClass supports direct field access or acessors (get and set methods)
 * EGClass adds ability to output objects as JSON.
 */

/*
 * EGExampleDerived
 *
 * Plain old class definition, except we extend EGObject and implement getClass
 */

class EGExample : public EGObject
{
private:
    EGbool aBool;
    EGint anInt;
    EGfloat aFloat;
    EGstring aString;
    EGint *anIntPtr;
    EGfloat aFloatArray[4];
    const EGint aConstInt;
    
public:
    EGExample(EGbool aBool, EGint anInt, EGfloat aFloat, EGstring aString, EGint *anIntPtr)
        : aBool(aBool), anInt(anInt), aFloat(aFloat), aString(aString), anIntPtr(anIntPtr), aFloatArray{1.1f, 2.2f, 3.3f, 4.4f}, aConstInt(99) {}

    /* extend EGObject and implement getClass to add late binding and runtime introspection support */

    static const EGClass* classFactory()
    {
        static const EGClass *clazz = EGClass::clazz<EGExample>()
            ->property("aBool",         &EGExample::aBool)
            ->property("anInt",         &EGExample::anInt)
            ->property("aFloat",        &EGExample::aFloat)->minVal(-100.0f)->maxVal(100.0f)
            ->property("aString",       &EGExample::aString)
            ->property("anIntPtr",      &EGExample::anIntPtr)
            ->property("aFloatArray",   &EGExample::aFloatArray)
            ->property("aConstIntProp", &EGExample::aConstInt);
        return clazz;
    }

    virtual const EGClass* getClass() const { return classFactory(); }
};


/*
 * EGExampleDerived
 *
 * derived class example
 */

class EGExampleDerived : public EGExample
{
private:
    EGint privInt;
    
public:
    EGint anotherInt;
    
    EGint getPrivInt() { return privInt; }
    void setPrivInt(EGint privInt)
    {
        if (this->privInt != privInt) {
            this->privInt = privInt;
        }
    }
    
    EGExampleDerived(EGbool aBool, EGint anInt, EGfloat aFloat, EGstring aString, EGint *anIntPtr, EGint anotherInt)
        : EGExample(aBool, anInt, aFloat, aString, anIntPtr), privInt(66), anotherInt(anotherInt) {}
    
    /* extend EGObject and implement getClass to add late binding and runtime introspection support */
    
    static const EGClass* classFactory()
    {
        static const EGClass *clazz = EGClass::clazz<EGExampleDerived>()->extends<EGExample>()
            ->property("anotherInt",    &EGExampleDerived::anotherInt)
            ->property("privInt",       &EGExampleDerived::getPrivInt, &EGExampleDerived::setPrivInt);
        return clazz;
    }
    
    virtual const EGClass* getClass() const { return classFactory(); }
};


/*
 * EGExampleEmitter
 *
 * signal receiver example
 */

class EGExampleEmitter : public EGObject
{
private:
    EGint x;

public:
    typedef std::shared_ptr<EGExampleEmitter> Ptr;
    
    EGExampleEmitter() : x(0) {}
    
    EGint getX() { return x; }
    
    void setX(EGint x)
    {
        if (this->x != x) {
            this->x = x;
            xChanged(x);
        }
    }
    
    void xChanged(EGint x) { emit(&EGExampleEmitter::xChanged, x); }

    void stringFloat(EGstring s, EGfloat y) { emit(&EGExampleEmitter::stringFloat, s, y); }

    static const EGClass* classFactory()
    {
        static const EGClass *clazz = EGClass::clazz<EGExampleEmitter>()
            ->property("x",             &EGExampleEmitter::getX, &EGExampleEmitter::setX)
            ->emitter("xChanged",       &EGExampleEmitter::xChanged)
            ->emitter("stringFloat",    &EGExampleEmitter::stringFloat);
        return clazz;
    }
    
    virtual const EGClass* getClass() const { return classFactory(); }
};


/*
 * EGExampleReceiver
 *
 * signal receiver example
 */

class EGExampleReceiver : public EGObject
{
private:
    EGint x;
    
public:
    typedef std::shared_ptr<EGExampleReceiver> Ptr;
    
    EGExampleReceiver() : x(0) {}
    
    void gotx(EGint x)
    {
        EGDebug("gotx x=%d\n", x);
    }
    
    void gotStringFloat(EGstring s, EGfloat y)
    {
        EGDebug("gotStringFloat s=%s, y=%f\n", s.c_str(), y);
    }

    static const EGClass* classFactory()
    {
        static const EGClass *clazz = EGClass::clazz<EGExampleReceiver>()
            ->property("x",             &EGExampleReceiver::x)
            ->message("gotx",           &EGExampleReceiver::gotx)
            ->message("gotStringFloat", &EGExampleReceiver::gotStringFloat);
        return clazz;
    }
    
    virtual const EGClass* getClass() const { return classFactory(); }
};


/*
 * Test program
 */

int main (int argc, char **argv)
{
    EGExample ex(true, 9, 1.3f, "foo", NULL);
    
    // print class definition
    std::cout << ex.getClass()->toString() << std::endl;
    
    // iterate over keys and print fields
    for (std::string key : ex.keys()) {
        std::cout << key << "=" << ex.getString(key) << std::endl;
    }

    // print object in JSON
    std::cout << ex.toString() << std::endl;

    // test getting properties
    std::cout << "aBool=" << ex.getValue("aBool").toString() << std::endl;
    std::cout << "anInt=" << ex.getValue("anInt").intValue() << std::endl;
    std::cout << "aFloat=" << ex.getValue("aFloat").floatValue() << std::endl;
    std::cout << "aString=" << ex.getString("aString") << std::endl;
    
    // test setting string types
    ex.setString("aBool", "false");
    ex.setString("anInt", "11");
    ex.setString("aFloat", "1.5");
    ex.setString("aString", "bar");
    EGfloat *floatArr = ex.getValue("aFloatArray").floatArray();
    floatArr[0] = 1.2f;
    floatArr[1] = 2.3f;
    floatArr[2] = 3.4f;
    floatArr[3] = 4.5f;
    std::cout << ex.toString() << std::endl;
    std::cout << "aBool=" << ex.getValue("aBool").toString() << std::endl;
    std::cout << "anInt=" << ex.getValue("anInt").intValue() << std::endl;
    std::cout << "aFloat=" << ex.getValue("aFloat").floatValue() << std::endl;
    std::cout << "aString=" << ex.getString("aString") << std::endl;
    
    // test setting values
    ex.setValue("aBool", true);
    ex.setValue("anInt", 13.5f);
    ex.setValue("aFloat", 2);
    ex.setValue("aString", 12.0f);
    ex.setValue("aConstIntProp", 98);
    ex.setValue("anIntPtr", (EGint*)0x1);
    ex.setString("aFloatArray", "[ 6.1, 7.2, 8.3, 9.4 ]");
    std::cout << ex.toString() << std::endl;
    std::cout << "aBool=" << ex.getValue("aBool").toString() << std::endl;
    std::cout << "anInt=" << ex.getValue("anInt").intValue() << std::endl;
    std::cout << "aFloat=" << ex.getValue("aFloat").floatValue() << std::endl;
    std::cout << "aString=" << ex.getString("aString") << std::endl;
    
    // test automatic type coercion
    EGstring aBool = ex.getValue("aBool");
    EGstring anInt = ex.getValue("anInt");
    EGint aFloat = ex.getValue("aFloat");
    EGfloat aString = ex.getValue("aString");
    std::cout << "EGstring aBool=" << aBool << std::endl;
    std::cout << "EGstring anInt=" << anInt << std::endl;
    std::cout << "EGint    aFloat=" << aFloat << std::endl;
    std::cout << "EGfloat  aString=" << aString << std::endl;
    
    // test constraints
    ex.setValue("aFloat", -101.0f);
    ex.setValue("aFloat", 101.0f);
    ex.setString("aFloat", "-101.0");
    ex.setString("aFloat", "101.0");
    std::cout << ex.toString() << std::endl;

    // test arrays
    EGfloat test[4] = { 1, 2, 3, 4 };
    EGValue testarrval(test);
    std::cout << "testarrval=" << testarrval.toString() << std::endl;
    ex.setValue("aFloatArray", test);
    std::cout << ex.toString() << std::endl;

    
    // Derived class test

    EGExampleDerived exd(true, 9, 1.3f, "foo", NULL, 7337);

    // print class definition
    std::cout << exd.getClass()->toString() << std::endl;
    
    // iterate over keys and print fields
    for (std::string key : exd.keys()) {
        std::cout << key << "=" << exd.getString(key) << std::endl;
    }
    
    // print object in JSON
    std::cout << exd.toString() << std::endl;

    // set base field, dervied field and accessor field
    exd.setValue("aFloat", 7.7f);
    exd.setValue("anotherInt", 8338);
    exd.setValue("privInt", 77);
    
    // print object in JSON
    std::cout << exd.toString() << std::endl;

    
    // Emitter Receiver test

    // create emitter and receiver
    EGExampleEmitter::Ptr objSender(new EGExampleEmitter());
    EGExampleReceiver::Ptr objReceiver(new EGExampleReceiver());
    
    // connect them
    EGObject::connect(objSender, "xChanged", objReceiver, "x");
    EGObject::connect(objSender, "xChanged", objReceiver, "gotx");
    EGObject::connect(objSender, "stringFloat", objReceiver, "gotStringFloat");
    
    std::cout << "objSender=" << objSender->toString() << std::endl;
    std::cout << "objReceiver=" << objReceiver->toString() << std::endl;
    
    // set a property
    objSender->setValue("x", 5);

    std::cout << "objSender=" << objSender->toString() << std::endl;
    std::cout << "objReceiver=" << objReceiver->toString() << std::endl;
    
    // directly send a message
    objSender->stringFloat("hello", 99.99f);

    // disconnect them
    EGObject::disconnect(objSender, "xChanged", objReceiver, "x");
    EGObject::disconnect(objSender, "xChanged", objReceiver, "gotx");
    EGObject::disconnect(objSender, "stringFloat", objReceiver, "gotStringFloat");

    // set a property
    objSender->setValue("x", 6);

    std::cout << "objSender=" << objSender->toString() << std::endl;
    std::cout << "objReceiver=" << objReceiver->toString() << std::endl;

    // directly send a message
    objSender->stringFloat("goodbye", 101.01f);
    
    // connect to a lambda
    objSender->connect<EGint>("xChanged", [] (EGint x) { EGDebug("lambda x=%d\n", x); });
    objSender->setX(11);

    return 0;
}
