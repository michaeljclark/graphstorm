/*
 *  EGTextMac.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGTextMac_H
#define EGTextMac_H

#if __APPLE__

#include <CoreText/CoreText.h>
#include <CoreText/CTFontCollection.h>


/* EGFontMac */

class EGFontMac : public EGFont
{
protected:
    CTFontDescriptorRef ctFontDesc;
    
public:
    EGFontMac(EGFontRec fontRec, CTFontDescriptorRef ctFontDesc);
    virtual ~EGFontMac();

    CTFontRef getCTFont(EGfloat fontSize);
};


/* EGTextContextMac */

class EGTextContextMac : public EGTextContext
{
public:    
    EGTextContextMac();
    ~EGTextContextMac();
    
    void initFonts();
};


/* EGTextMac */

class EGTextMac : public EGText
{
private:    
    CGContextRef cgCtx;
    CTFontRef ctFont;
    CFDictionaryRef atAttributes;
    CGFloat ctwidth, ctascent, ctdescent, ctleading;
    
public:
    EGTextMac();
    ~EGTextMac();
        
    void createContext(EGuint width, EGuint height);
    void deleteContext();
    void createFontContext();
    void deleteFontContext();
    void getTextSize(EGuint &width, EGuint &height);
    void calcGlyphRects();
    void renderText();

    static EGbool init();
};

#endif

#endif
