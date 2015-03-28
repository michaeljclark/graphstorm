/*
 *  EGObject.cc
 *
 *  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGObject.h"


/* EGType */

const EGTypeString          EGType::String;
const EGTypeObject          EGType::Object;
const EGTypeVoid            EGType::Void;
const EGTypeBool            EGType::Bool;
const EGTypeUByte           EGType::UByte;
const EGTypeByte            EGType::Byte;
const EGTypeShort           EGType::Short;
const EGTypeUShort          EGType::UShort;
const EGTypeInt             EGType::Int;
const EGTypeUInt            EGType::UInt;
const EGTypeLong            EGType::Long;
const EGTypeULong           EGType::ULong;
const EGTypeLongLong        EGType::LongLong;
const EGTypeULongLong       EGType::ULongLong;
const EGTypeFloat           EGType::Float;
const EGTypeDouble          EGType::Double;
const EGTypeObjectPtr       EGType::ObjectPtr;
const EGTypeVoidPtr         EGType::VoidPtr;
const EGTypeBoolPtr         EGType::BoolPtr;
const EGTypeUBytePtr        EGType::UBytePtr;
const EGTypeBytePtr         EGType::BytePtr;
const EGTypeShortPtr        EGType::ShortPtr;
const EGTypeUShortPtr       EGType::UShortPtr;
const EGTypeIntPtr          EGType::IntPtr;
const EGTypeUIntPtr         EGType::UIntPtr;
const EGTypeLongPtr         EGType::LongPtr;
const EGTypeULongPtr        EGType::ULongPtr;
const EGTypeLongLongPtr     EGType::LongLongPtr;
const EGTypeULongLongPtr    EGType::ULongLongPtr;
const EGTypeFloatPtr        EGType::FloatPtr;
const EGTypeDoublePtr       EGType::DoublePtr;
const EGTypeBoolArray       EGType::BoolArray;
const EGTypeUByteArray      EGType::UByteArray;
const EGTypeByteArray       EGType::ByteArray;
const EGTypeShortArray      EGType::ShortArray;
const EGTypeUShortArray     EGType::UShortArray;
const EGTypeIntArray        EGType::IntArray;
const EGTypeUIntArray       EGType::UIntArray;
const EGTypeLongArray       EGType::LongArray;
const EGTypeULongArray      EGType::ULongArray;
const EGTypeLongLongArray   EGType::LongLongArray;
const EGTypeULongLongArray  EGType::ULongLongArray;
const EGTypeFloatArray      EGType::FloatArray;
const EGTypeDoubleArray     EGType::DoubleArray;


/* EGType */

EGstring EGType::jsonStringEncode(EGstring input) const
{
    std::stringstream ss;
    for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
        switch (*iter) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '/': ss << "\\/"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << *iter; break;
        }
    }
    return ss.str();
}


/* EGValue */

EGint EGValue::nullZero = 0;
EGValue EGValue::nullValue(EGType::Int, &nullZero);


/* EGClass */

static const char class_class_name[] = "EGClass";

EGClass::EGClass(std::string name, const std::type_info *classInfo) : name(name), classInfo(classInfo), baseClazz(NULL)
{
    if (*classInfo != typeid(EGObject)) {
        extends<EGObject>();
    }
}

void EGClass::addProperty(EGPropertyPtr prop)
{
    const EGstring &name = prop->name;
    if (properties.find(name) != properties.end() ||
        messages.find(name) != messages.end() ||
        emitters.find(name) != emitters.end())
    {
        EGError("%s:%s name not unique: %s::%s", class_class_name, __func__, this->name.c_str(), name.c_str());
        return;
    }
    properties.insert(std::pair<std::string,EGPropertyPtr>(name, prop));
    lastprop = prop;
}

void EGClass::addMessage(EGMessagePtr message)
{
    const EGstring &name = message->getFunction()->getName();
    if (properties.find(name) != properties.end() ||
        messages.find(name) != messages.end() ||
        emitters.find(name) != emitters.end())
    {
        EGError("%s:%s name not unique: %s::%s", class_class_name, __func__, this->name.c_str(), name.c_str());
        return;
    }
    messages.insert(std::pair<std::string,EGMessagePtr>(name, message));
}

void EGClass::addEmitter(EGEmitterPtr emitter)
{
    const EGstring &name = emitter->getFunction()->getName();
    if (properties.find(name) != properties.end() ||
        messages.find(name) != messages.end() ||
        emitters.find(name) != emitters.end())
    {
        EGError("%s:%s name not unique: %s::%s", class_class_name, __func__, this->name.c_str(), name.c_str());
        return;
    }
    emitters.insert(std::pair<std::string,EGEmitterPtr>(name, emitter));
}

EGClassTypeInfoMap& EGClass::getClassTypeInfoMap()
{
    static EGClassTypeInfoMap classTypeInfoMap;
    return classTypeInfoMap;
}

const EGstring& EGClass::getName() const
{
    return name;
}

const EGPropertyMap& EGClass::getProperties() const
{
    return properties;
}

const EGMessageMap& EGClass::getMessages() const
{
    return messages;
}

const EGEmitterMap& EGClass::getEmitters() const
{
    return emitters;
}

EGstring EGClass::toString() const
{
    std::stringstream ss;
    ss << "{ \"classname\": \"" << name << "\", \"properties\": [ ";
    for (EGPropertyMap::const_iterator pi = properties.begin(); pi != properties.end(); pi++) {
        const EGPropertyPtr &prop = (*pi).second;
        if (pi != properties.begin()) ss << ", ";
        ss << prop->toString();
    }
    ss << " ] }";
    return ss.str();
}

EGClassPtr EGClass::getClassByType(const std::type_info *classInfo)
{
    EGClassTypeInfoMap &map = getClassTypeInfoMap();
    EGClassTypeInfoMap::iterator ci = map.find(classInfo);
    if (ci != map.end()) {
        return (*ci).second;
    } else {
        return EGClassPtr();
    }
}


/* EGConstraintImpl */

EGConstraintImpl::~EGConstraintImpl() {}


/* EGObject */

static const char object_class_name[] = "EGObject";

const EGbool EGObject::debug = false;

EGObject::~EGObject()
{
    destroyed(this);
}

const EGClass* EGObject::classFactory()
{
    static const EGClass *clazz = EGClass::clazz<EGObject>()
        ->emitter("destroyed",      &EGObject::destroyed)
        ->message("disconnect",     &EGObject::disconnect);
    return clazz;
}

const EGClass* EGObject::getClass() const { return classFactory(); }

void EGObject::destroyed(EGObject *object) { emit(&EGObject::destroyed, this); }

EGstring EGObject::toString() const
{
    const EGPropertyMap &properties = getClass()->getProperties();
    std::stringstream ss;
    ss << "{ ";
    for (EGPropertyMap::const_iterator pi = properties.begin(); pi != properties.end(); pi++) {
        const EGPropertyPtr &prop = (*pi).second;
        if (pi != properties.begin()) ss << ", ";
        ss << "\"" << prop->name << "\": " << prop->getStringJSON((void*)this);
    }
    ss << " }";
    return ss.str();
}

EGstring EGObject::getString(const EGPropertyPtr &prop) const
{
    return prop->getString((void*)this);
}

void EGObject::setString(const EGPropertyPtr &prop, EGstring &val)
{
    if (prop->access == EGAccessReadWrite) {
        if (&prop->type == &EGType::String || prop->constraints.size() == 0) {
            prop->setString((void*)this, val);
        } else {
            EGubyte *buf = new EGubyte[prop->type.size(prop->nelem)];
            prop->type.fromString((void*)buf, val, prop->nelem);
            EGValue v = prop->type.toValue(buf, prop->nelem);
            delete [] buf;
            for (EGConstraintList::iterator ci = prop->constraints.begin(); ci != prop->constraints.end(); ci++) {
                const EGConstraintPtr &constraint = *ci;
                if (!constraint->evaluate(v)) {
                    if (debug) {
                        EGDebug("%s:%s ignoring, violated constraint: %s %s\n", getClass()->getName().c_str(), __func__, prop->name.c_str(), constraint->toString().c_str());
                    }
                    return;
                }
            }
            prop->setValue((void*)this, v);
        }
    } else {
        if (debug) {
            EGDebug("%s:%s ignoring, readonly property: %s\n", getClass()->getName().c_str(), __func__, prop->name.c_str());
        }
    }
}

EGValue EGObject::getValue(const EGPropertyPtr &prop) const
{
    return prop->getValue((void*)this);
}

void EGObject::setValue(const EGPropertyPtr &prop, EGValue &val)
{
    if (prop->access == EGAccessReadWrite) {
        for (EGConstraintList::iterator ci = prop->constraints.begin(); ci != prop->constraints.end(); ci++) {
            const EGConstraintPtr &constraint = *ci;
            if (!constraint->evaluate(val)) {
                if (debug) {
                    EGDebug("%s:%s ignoring, violated constraint: %s %s\n", getClass()->getName().c_str(), __func__, prop->name.c_str(), constraint->toString().c_str());
                }
                return;
            }
        }
        prop->setValue((void*)this, val);
    } else {
        if (debug) {
            EGDebug("%s:%s ignoring, readonly property: %s\n", getClass()->getName().c_str(), __func__, prop->name.c_str());
        }
    }
}

std::vector<EGstring> EGObject::keys() const
{
    std::vector<EGstring> vec;
    const EGPropertyMap &properties = getClass()->getProperties();
    for (EGPropertyMap::const_iterator pi = properties.begin(); pi != properties.end(); pi++) {
        vec.push_back((*pi).first);
    }
    return vec;
}

EGstring EGObject::getString(EGstring prop) const
{
    const EGPropertyMap &properties = getClass()->getProperties();
    EGPropertyMap::const_iterator pi = properties.find(prop);
    if (pi != properties.end()) {
        return getString((*pi).second);
    } else {
        return "";
    }
}

void EGObject::setString(EGstring prop, EGstring val)
{
    const EGPropertyMap &properties = getClass()->getProperties();
    EGPropertyMap::const_iterator pi = properties.find(prop);
    if (pi != properties.end()) {
        setString((*pi).second, val);
    }
}

EGValue EGObject::getValue(EGstring prop) const
{
    const EGPropertyMap &properties = getClass()->getProperties();
    EGPropertyMap::const_iterator pi = properties.find(prop);
    if (pi != properties.end()) {
        return getValue((*pi).second);
    } else {
        return EGValue::nullValue;
    }
}

void EGObject::setValue(EGstring prop, EGValue val)
{
    const EGPropertyMap &properties = getClass()->getProperties();
    EGPropertyMap::const_iterator pi = properties.find(prop);
    if (pi != properties.end()) {
        setValue((*pi).second, val);
    }
}

EGbool EGObject::connect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                         const EGFunctionPtr &func)
{
    if (!sourceObject || !emitter || !func) return false;
    
    if (sourceObject->getClass() != emitter->getFunction()->getClass()) {
        EGError("%s:%s source object class does not match emitter class: %s\n",
                object_class_name, __func__, emitter->getFunction()->getName().c_str());
        return false;
    }
    
    EGEmitterConnection conn(emitter, NULL, func);
    EGEmitterConnectionList::iterator ci = std::find(sourceObject->connections.begin(), sourceObject->connections.end(), conn);
    if (ci != sourceObject->connections.end()) {
        EGError("%s:%s duplicate connection from emitter: %s\n", object_class_name, __func__, emitter->getFunction()->getName().c_str());
        return false;
    }
    
    if (conn.emitter->getFunction()->getTypeSignature() != conn.destFunction->getTypeSignature()) {
        EGError("%s:%s type mismatch: emitter=%s message=%s\n",
                object_class_name, __func__, emitter->getFunction()->getName().c_str(), func->getName().c_str());
        return false;
    }
    
    // add emitter connection
    sourceObject->connections.push_back(conn);
    
    return true;
}

EGbool EGObject::connect(EGObject *sourceObject, EGstring emitterName,
                         const EGFunctionPtr &func)
{
    if (!sourceObject) return false;
    
    const EGClass *sourceClass = sourceObject->getClass();
    const EGEmitterMap &emitters = sourceClass->getEmitters();
    EGEmitterMap::const_iterator ei = emitters.find(emitterName);
    
    if (ei == emitters.end()) {
        EGError("%s:%s unknown emitter: %s\n", object_class_name, __func__, emitterName.c_str());
        return false;
    } else {
        const EGEmitterPtr &emitter = (*ei).second;
        return connect(sourceObject, emitter, func);
    }
}

EGbool EGObject::connect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                         EGObject *destObject, const EGPropertyPtr &property)
{
    if (!sourceObject || !emitter || !destObject || !property) return false;
    
    if (sourceObject->getClass() != emitter->getFunction()->getClass()) {
        EGError("%s:%s source object class does not match emitter class: %s\n",
                object_class_name, __func__, emitter->getFunction()->getName().c_str());
        return false;
    }
    
    if (destObject->getClass() != property->getClass()) {
        EGError("%s:%s dest object class does not match property class: %s\n",
                object_class_name, __func__, emitter->getFunction()->getName().c_str());
        return false;
    }

    EGEmitterConnection conn(emitter, destObject, property);
    EGEmitterConnectionList::iterator ci = std::find(sourceObject->connections.begin(), sourceObject->connections.end(), conn);
    if (ci != sourceObject->connections.end()) {
        EGError("%s:%s duplicate connection from emitter: %s\n",
                object_class_name, __func__, emitter->getFunction()->getName().c_str());
        return false;
    }
    
    const EGFunctionPtr &emitterFunction = conn.emitter->getFunction();
    std::vector<const EGType*> emitterArgTypes = emitterFunction->getArgumentTypes();
    std::vector<size_t> emitterArgElements = emitterFunction->getArgumentElements();
    if (emitterArgTypes.size() != 1 || emitterArgTypes.at(0) != &property->type) {
        EGError("%s:%s type mismatch: emitter=%s property=%s\n",
                object_class_name, __func__, emitter->getFunction()->getName().c_str(), property->name.c_str());
        return false;
    }
    
    // add emitter connection
    sourceObject->connections.push_back(conn);
    
    // add connections from destroyed to disconnect
    // both forward and reverse connections are required as dest object becomes an emitter
    // and needs to receive disconnect from the source object if it is destroyed
    sourceObject->connectDestroyedDisconnect(destObject);
    destObject->connectDestroyedDisconnect(sourceObject);
    
    return true;
}

EGbool EGObject::connect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                         EGObject *destObject, const EGMessagePtr &message)
{
    if (!sourceObject || !emitter || !destObject || !message) return false;
    
    if (sourceObject->getClass() != emitter->getFunction()->getClass()) {
        EGError("%s:%s source object class does not match emitter class: %s\n",
                object_class_name, __func__, emitter->getFunction()->getName().c_str());
        return false;
    }

    if (destObject->getClass() != message->getFunction()->getClass()) {
        EGError("%s:%s dest object class does not match message class: %s\n",
                object_class_name, __func__, emitter->getFunction()->getName().c_str());
        return false;
    }

    EGEmitterConnection conn(emitter, destObject, message->getFunction());
    EGEmitterConnectionList::iterator ci = std::find(sourceObject->connections.begin(), sourceObject->connections.end(), conn);
    if (ci != sourceObject->connections.end()) {
        EGError("%s:%s duplicate connection from emitter: %s\n", object_class_name, __func__, emitter->getFunction()->getName().c_str());
        return false;
    }
    
    if (conn.emitter->getFunction()->getTypeSignature() != conn.destFunction->getTypeSignature()) {
        EGError("%s:%s type mismatch: emitter=%s message=%s\n",
                object_class_name, __func__, emitter->getFunction()->getName().c_str(), message->getFunction()->getName().c_str());
        return false;
    }
    
    // add emitter connection
    sourceObject->connections.push_back(conn);

    // add connections from destroyed to disconnect
    // both forward and reverse connections are required as dest object becomes an emitter
    // and needs to receive disconnect from the source object if it is destroyed
    sourceObject->connectDestroyedDisconnect(destObject);
    destObject->connectDestroyedDisconnect(sourceObject);

    return true;
}

EGbool EGObject::connect(EGObject *sourceObject, EGstring emitterName,
                         EGObject *destObject, EGstring targetName)
{
    if (!sourceObject || !destObject) return false;
    
    const EGClass *sourceClass = sourceObject->getClass();
    const EGClass *destClass = destObject->getClass();
    
    const EGEmitterMap &emitters = sourceClass->getEmitters();
    const EGMessageMap &messages = destClass->getMessages();
    const EGPropertyMap &properties = destClass->getProperties();
    
    EGEmitterMap::const_iterator ei = emitters.find(emitterName);
    EGMessageMap::const_iterator mi = messages.find(targetName);
    EGPropertyMap::const_iterator pi = properties.find(targetName);
    
    if (ei == emitters.end()) {
        EGError("%s:%s unknown emitter: %s\n", object_class_name, __func__, emitterName.c_str());
        return false;
    } else if (pi != properties.end()) {
        const EGEmitterPtr &emitter = (*ei).second;
        const EGPropertyPtr &property = (*pi).second;
        return EGObject::connect(sourceObject, emitter, destObject, property);
    } else if (mi != messages.end()) {
        const EGEmitterPtr &emitter = (*ei).second;
        const EGMessagePtr &message = (*mi).second;
        return EGObject::connect(sourceObject, emitter, destObject, message);
    } else {
        EGError("%s:%s unknown property or message: %s\n", object_class_name, __func__, targetName.c_str());
        return false;
    }
}

EGbool EGObject::disconnect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                            const EGFunctionPtr &func)
{
    if (!sourceObject || !emitter || !func) return false;
    
    EGEmitterConnection conn(emitter, NULL, func);
    EGEmitterConnectionList::iterator ci = std::find(sourceObject->connections.begin(), sourceObject->connections.end(), conn);
    if (ci != sourceObject->connections.end()) {
        sourceObject->connections.erase(ci);
        return true;
    }
    return false;
}

EGbool EGObject::disconnect(EGObject *sourceObject, EGstring emitterName,
                            const EGFunctionPtr &func)
{
    if (!sourceObject) return false;
    
    const EGClass *sourceClass = sourceObject->getClass();
    const EGEmitterMap &emitters = sourceClass->getEmitters();
    EGEmitterMap::const_iterator ei = emitters.find(emitterName);

    if (ei == emitters.end()) {
        EGError("%s:%s unknown emitter: %s\n", object_class_name, __func__, emitterName.c_str());
        return false;
    } else {
        const EGEmitterPtr &emitter = (*ei).second;
        return disconnect(sourceObject, emitter, func);
    }
}

EGbool EGObject::disconnect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                            EGObject *destObject, const EGPropertyPtr &property)
{
    if (!sourceObject || !emitter || !destObject || !property) return false;
    
    EGEmitterConnection conn(emitter, destObject, property);
    EGEmitterConnectionList::iterator ci = std::find(sourceObject->connections.begin(), sourceObject->connections.end(), conn);
    if (ci != sourceObject->connections.end()) {
        sourceObject->connections.erase(ci);
        return true;
    }
    return false;
}

EGbool EGObject::disconnect(EGObject *sourceObject, const EGEmitterPtr &emitter,
                            EGObject *destObject, const EGMessagePtr &message)
{
    if (!sourceObject || !emitter || !destObject || !message) return false;
    
    EGEmitterConnection conn(emitter, destObject, message->getFunction());
    EGEmitterConnectionList::iterator ci = std::find(sourceObject->connections.begin(), sourceObject->connections.end(), conn);
    if (ci != sourceObject->connections.end()) {
        sourceObject->connections.erase(ci);
        return true;
    }
    return false;
}

EGbool EGObject::disconnect(EGObject *sourceObject, EGstring emitterName,
                            EGObject *destObject, EGstring targetName)
{
    if (!sourceObject || !destObject) return false;
    
    const EGClass *sourceClass = sourceObject->getClass();
    const EGClass *destClass = destObject->getClass();
    
    const EGEmitterMap &emitters = sourceClass->getEmitters();
    const EGMessageMap &messages = destClass->getMessages();
    const EGPropertyMap &properties = destClass->getProperties();
    
    EGEmitterMap::const_iterator ei = emitters.find(emitterName);
    EGMessageMap::const_iterator mi = messages.find(targetName);
    EGPropertyMap::const_iterator pi = properties.find(targetName);
    
    if (ei == emitters.end()) {
        EGError("%s:%s unknown emitter: %s\n", object_class_name, __func__, emitterName.c_str());
        return false;
    } else if (pi != properties.end()) {
        const EGEmitterPtr &emitter = (*ei).second;
        const EGPropertyPtr &property = (*pi).second;
        return EGObject::disconnect(sourceObject, emitter, destObject, property);
    } else if (mi != messages.end()) {
        const EGEmitterPtr &emitter = (*ei).second;
        const EGMessagePtr &message = (*mi).second;
        return EGObject::disconnect(sourceObject, emitter, destObject, message);
    } else {
        EGError("%s:%s unknown property or message: %s\n", object_class_name, __func__, targetName.c_str());
        return false;
    }
}

void EGObject::disconnect(EGObject* destObject)
{
    if (!destObject) return;
    for(EGEmitterConnectionList::iterator ci = connections.begin(); ci != connections.end(); ) {
        if (ci->destObject == destObject) {
            ci = connections.erase(ci);
        } else {
            ci++;
        }
    }
}

void EGObject::connectDestroyedDisconnect(EGObject* destObject)
{
    const EGClass *sourceClass = this->getClass();
    const EGClass *destClass = destObject->getClass();
    const EGEmitterMap &destEmitters = destClass->getEmitters();
    const EGMessageMap &sourceMessages = sourceClass->getMessages();
    EGEmitterMap::const_iterator ei = destEmitters.find("destroyed");
    EGMessageMap::const_iterator mi = sourceMessages.find("disconnect");
    if (ei != destEmitters.end() && mi != sourceMessages.end()) {
        const EGEmitterPtr &destroyedEmitter = (*ei).second;
        const EGMessagePtr &disconnectMessage = (*mi).second;
        EGEmitterConnection disconn(destroyedEmitter, destObject, disconnectMessage->getFunction());
        EGEmitterConnectionList::iterator ci = std::find(destObject->connections.begin(), destObject->connections.end(), disconn);
        if (ci == destObject->connections.end()) {
            destObject->connections.push_back(disconn);
        }
    }
}
