// See LICENSE for license details.

#include "EG.h"
#include "EGEvent.h"


/* EGEventInfoRef */

EGEventInfoRef::EGEventInfoRef() : ei(EGEventManager::eilist.end()) {}


/* EGEventManager */

/* list of registered event handlers */
EGEventInfoList EGEventManager::eilist;

/* Remove event listener */
EGEventInfoRef EGEventManager::disconnect(EGEventInfoRef eiref)
{
    return eilist.erase(eiref.ei);
}

/* Post an event */
EGbool EGEventManager::postEvent(EGEvent* evt)
{
    // std::cout << evt->toString() << std::endl;
    EGEventInfoList::iterator eiref;
    const char *evtType = typeid(*evt).name();
    EGEventInfoList matches;
    for (eiref = eilist.begin(); eiref != eilist.end(); eiref++) {
        if (strcmp(eiref->eventType, evtType) == 0 &&
            eiref->q == evt->q) {
            matches.push_back(*eiref);
        }
    }
    for (eiref = matches.begin(); eiref != matches.end(); eiref++) {
        if ((*eiref).cb->call(evt)) return true;
    }
    return false;
}


/* EGEvent */

EGEvent::EGEvent(EGEventQualifier q) : q(q) {}

EGEvent::~EGEvent() {}
