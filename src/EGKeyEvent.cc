// See LICENSE for license details.

#include "EG.h"
#include "EGEvent.h"
#include "EGKeyEvent.h"


/* EGKeyEvent */

EGEventQualifier EGKeyEvent::PRESSED     = "key-pressed";
EGEventQualifier EGKeyEvent::RELEASED    = "key-released";

EGKeyEvent::EGKeyEvent(EGEventQualifier q, EGint x, EGint y, EGint charcode, EGint keycode, EGint modifiers)
: EGEvent(q), x(x), y(y), charcode(charcode), keycode(keycode), modifiers(modifiers) {}

std::string EGKeyEvent::toString() {
    std::stringstream s;
    s << "[EGKeyEvent " << q << " x=" << x << " y=" << y << " char=" << charcode << " keycode=" << keycode << " modifiers=" << modifiers << "]";
    return s.str();
};
