// See LICENSE for license details.

#include "EG.h"
#include "EGEvent.h"
#include "EGMouseEvent.h"


/* EGMouseEvent */

EGEventQualifier EGMouseEvent::PRESSED   = "button-pressed";
EGEventQualifier EGMouseEvent::RELEASED  = "button-released";
EGEventQualifier EGMouseEvent::MOTION    = "pointer-motion";

EGMouseEvent::EGMouseEvent(EGEventQualifier q, EGint x, EGint y)
    : EGEvent(q), x(x), y(y), button(-1) {}

EGMouseEvent::EGMouseEvent(EGEventQualifier q, EGint x, EGint y, EGint button)
    : EGEvent(q), x(x), y(y), button(button) {}

std::string EGMouseEvent::toString() {
    std::stringstream s;
    s << "[EGMouseEvent " << q << " button=" << button << " x=" << x << " y=" << y << "]";
    return s.str();
};
