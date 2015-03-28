/*
 *  EGText.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGText_H
#define EGText_H

#include "EGBase.h"
#include "EGColor.h"
#include "EGFont.h"

class EGText;
typedef std::shared_ptr<EGText> EGTextPtr;
class EGFont;
typedef std::shared_ptr<EGFont> EGFontPtr;
class EGTextCache;
class EGTextRenderer;
class EGTextRendererES2;
class EGTextCairo;
class EGTextMac;
class EGTextGDI;
class EGTextDWrite;
class EGTextContext;
typedef std::shared_ptr<EGTextContext> EGTextContextPtr;


/* EGTextFlags */

typedef enum {
    EGTextHAlignLeft = 0x1,
    EGTextHAlignCenter = 0x2,
    EGTextHAlignRight = 0x4,
    EGTextVAlignTop = 0x8,
    EGTextVAlignCenter = 0x10,
    EGTextVAlignBase = 0x20,
    EGTextEmbossed = 0x40
} EGTextFlags;


/* EGTextStyle */

class EGTextStyle
{
public:
    std::string fontface;
    EGfloat fontsize;
    EGenum fontstyle;
    EGenum textflags;
    EGColor color;
    EGColor embosscolor;
    
    EGTextStyle(const std::string &fontface, EGfloat fontsize, EGenum fontstyle, EGenum textflags, const EGColor &color, const EGColor &embosscolor);
};


/* EGTextContext */

class EGTextContext
{
public:
    EGTextContext();
    virtual ~EGTextContext();
    
    virtual void initFonts() = 0;
};


/* EGText */

class EGText
{
protected:
    static EGTextContextPtr ctx;
    
    void *texData;
    EGuint texWidth, texHeight;
    std::string fontFace;
    EGfloat fontSize;
    EGenum fontStyle;
    EGfloat scale;
    std::string textStr;
    EGenum flags;
    EGfloat x, y;
    EGfloat angle;
    EGuint maxWidth;
    EGuint w, h;
    EGfloat color[4];
    EGfloat embosscolor[4];
    EGuint wp2, hp2;
    EGbool rendered;
    EGbool sized;
    EGbool updated;
    EGRectList glyphRects;
    
    EGTextRenderer *renderer;
    
    virtual void createContext(EGuint width, EGuint height) = 0;
    virtual void deleteContext() = 0;
    virtual void createFontContext() = 0;
    virtual void deleteFontContext() = 0;
    virtual void getTextSize(EGuint &width, EGuint &height) = 0;
    virtual void calcGlyphRects() = 0;
    virtual void renderText() = 0;

public:
    friend class EGTextRendererES2;

    static EGfloat defaultScale;
    static const EGuint defaultTextureWidth;
    static const EGuint defaultTextureHeight;
    static const EGfloat defaultFontSize;
    static const char* defaultFontFace;

    EGText();
    virtual ~EGText();
    
    const EGRectList& getGlyphRects();
    EGuint getWidth();
    EGuint getHeight();
    void calcSize();
    void setScale(EGfloat scale);
    void setFontSize(EGfloat size);
    void setFontFace(const std::string &font);
    void setFontStyle(EGenum fontstyle);
    void setText(const std::string &text);
    void setColor(EGfloat r, EGfloat g, EGfloat b, EGfloat a);
    void setColor(const EGColor &newcolor);
    void setEmbossColor(EGfloat r, EGfloat g, EGfloat b, EGfloat a);
    void setEmbossColor(const EGColor &newcolor);
    void setFlags(EGenum flags);
    void setPosition(EGfloat x, EGfloat y);
    void setRotation(EGfloat angle);
    void setMaximumWidth(EGuint maxWidth);
    EGfloat getFontSize();
    std::string& getFontFace();
    std::string& getText();
    EGenum getFlags();
    EGfloat getX();
    EGfloat getY();
    void clearBuffers();
    EGbool prepare();
    void draw();
    
    static EGbool init();
    static EGText* create();
    static EGTextPtr create(const std::string &fontface, EGfloat fontsize, EGenum fontstyle, EGenum textflags);
    static EGTextPtr create(const std::string &textstring, const EGTextStyle &textstyle);
};


/* EGTextCache */

class EGTextCache
{
private:
    std::list<EGText*> spare;
    
public:
    EGText* create()
    {
        EGText *text;
        if (!spare.empty()) {
            text = spare.front();
            spare.pop_front();
        } else {
            text = EGText::create();
        }
        return text;
    }

    EGText* create(const std::string &fontface, EGfloat fontsize, EGenum fontstyle, EGenum textflags)
    {
        EGText *text = create();
        text->setFontFace(fontface);
        text->setFontSize(fontsize);
        text->setFontStyle(fontstyle);
        text->setFlags(textflags);
        return text;
    }
    
    void release(EGText *text)
    {
        if (text) {
            spare.push_back(text);
        }
    }
};

#endif
