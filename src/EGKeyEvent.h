// See LICENSE for license details.

#pragma once

/* EGKeyEvent */

class EGKeyEvent : public EGEvent {
public:
    static EGEventQualifier PRESSED;
    static EGEventQualifier RELEASED;
    
    static const int KEYCODE_F1        = 112;
    static const int KEYCODE_F2        = 113;
    static const int KEYCODE_F3        = 114;
    static const int KEYCODE_F4        = 115;
    static const int KEYCODE_F5        = 116;
    static const int KEYCODE_F6        = 117;
    static const int KEYCODE_F7        = 118;
    static const int KEYCODE_F8        = 119;
    static const int KEYCODE_F9        = 120;
    static const int KEYCODE_F10       = 121;
    static const int KEYCODE_F11       = 122;
    static const int KEYCODE_F12       = 123;
    static const int KEYCODE_LEFT      = 37;
    static const int KEYCODE_UP        = 38;
    static const int KEYCODE_RIGHT     = 39;
    static const int KEYCODE_DOWN      = 40;
    static const int KEYCODE_PAGE_UP   = 33;
    static const int KEYCODE_PAGE_DOWN = 34;
    static const int KEYCODE_HOME      = 36;
    static const int KEYCODE_END       = 35;
    static const int KEYCODE_INSERT    = 45;
    
    static const int MODIFIER_NONE     = 0x0;
    static const int MODIFIER_SHIFT    = 0x1;
    static const int MODIFIER_CTRL     = 0x2;
    static const int MODIFIER_ALT      = 0x4;
    
    EGint x, y;
    EGint charcode, keycode, modifiers;
    
    EGKeyEvent(EGEventQualifier q, EGint x, EGint y, EGint charcode, EGint keycode, EGint modifiers = 0);
    
    std::string toString();
};
