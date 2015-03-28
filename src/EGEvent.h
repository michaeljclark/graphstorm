/*
 *  EGEvent.h
 *
 *  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGEVENT_H_
#define EGEVENT_H_

#include <list>
#include <string>
#include <typeinfo>
#include <iostream>

class EGEvent;
class EGEventManager;
struct EGEventInfoRef;

typedef const char* EGEventQualifier;


/*
 * EGEvent
 *
 * Base class for all events
 */

class EGEvent
{
public:
    EGEventQualifier q;
    
    EGEvent(EGEventQualifier q);
    
    virtual ~EGEvent();
    
    virtual std::string toString() = 0;
};


/*
 * EGEventCallback
 *
 * Event callback abstract base class
 */

class EGEventCallback
{
public:
    virtual ~EGEventCallback() {}
    
    virtual EGbool call(EGEvent *evt) = 0;
};


/*
 * EGTypedEventCallback
 *
 * Templated typesafe downcasting callback wrapper
 */

template <class R, class T>
class EGTypedEventCallback : public EGEventCallback
{
public:
    EGTypedEventCallback(R *obj, EGbool (R::*cb)(T*)) : obj(obj), cb(cb) {}
    
    EGTypedEventCallback(const EGTypedEventCallback &tcv) : obj(tcv.obj), cb(tcv.cb) {}
    
    R *obj;
    EGbool (R::*cb)(T*);
    
    virtual ~EGTypedEventCallback() {}

    EGbool call(EGEvent *evt) { return (obj->*cb)(static_cast<T*>(evt)); }    
};


/*
 * EGEventInfo
 *
 * Stores event registration info
 */

typedef std::shared_ptr<EGEventCallback> EGEventCallbackPtr;

class EGEventInfo
{
public:
    const char *eventType;
    EGEventQualifier q;
    EGEventCallbackPtr cb;
    
    EGEventInfo(const char* eventType, EGEventQualifier q, EGEventCallbackPtr cb) : eventType(eventType), q(q), cb(cb) {}
    
    EGEventInfo(const EGEventInfo &e) : eventType(e.eventType), q(e.q), cb(e.cb) {}
};


/*
 * EGEventInfoRef
 *
 * Wrapper around EGEventInfoList::iterator
 */

typedef std::list<EGEventInfo> EGEventInfoList;

struct EGEventInfoRef
{
	EGEventInfoList::iterator ei;

	EGEventInfoRef();
	EGEventInfoRef(const EGEventInfoRef& o) : ei(o.ei) {}
	EGEventInfoRef(const EGEventInfoList::iterator &ei) : ei(ei) {}
	operator const EGEventInfoList::iterator& () { return ei; }
	bool operator==(const EGEventInfoRef &o) { return ei == o.ei; }
	bool operator!=(const EGEventInfoRef &o) { return ei != o.ei; }
};


/*
 * EGEventManager
 *
 * Main interface to add and remove event listeners and to post events
 */

class EGEventManager
{
private:
	friend struct EGEventInfoRef;
    static EGEventInfoList eilist;
    
public:
    
    /* Add event listener using an instance and pointer to member function
     * that recieves the event type */
    template <class T, class R>
    static EGEventInfoRef connect(EGEventQualifier q, R* obj, EGbool (R::*cb)(T*)) {
        return eilist.insert(eilist.end(), EGEventInfo(typeid(T).name(), q, EGEventCallbackPtr(new EGTypedEventCallback<R,T>(obj, cb))));
    }
    
    /* Remove event listener */
    static EGEventInfoRef disconnect(EGEventInfoRef eiref);
    
    /* Post an event */
    static EGbool postEvent(EGEvent* evt);
    
};

#endif /* EGEVENT_H_ */
