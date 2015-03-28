/*
 *  EGText.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGGL.h"
#include "EGText.h"
#include "EGMath.h"
#include "EGResource.h"
#include "EGRenderApi.h"
#include "EGRenderApiES2.h"
#include "EGTextRenderer.h"
#include "EGTextRendererES2.h"
#include "EGTextCairo.h"
#include "EGTextMac.h"
#include "EGTextGDI.h"
#include "EGTextDWrite.h"

EGfloat EGText::defaultScale = 1.0f;
const EGuint EGText::defaultTextureWidth = 16;
const EGuint EGText::defaultTextureHeight = 16;
const EGfloat EGText::defaultFontSize = 12.0f;

#if _WIN32
const char* EGText::defaultFontFace = "Arial";
#elif EMSCRIPTEN || __native_client__ || __linux__
const char* EGText::defaultFontFace = "MgOpen Moderna";
#else
const char* EGText::defaultFontFace = "Helvetica";
#endif


/* EGTextStyle */

EGTextStyle::EGTextStyle(const std::string &fontface, EGfloat fontsize, EGenum fontstyle, EGenum textflags, const EGColor &color, const EGColor &embosscolor)
    : fontface(fontface), fontsize(fontsize), fontstyle(fontstyle), textflags(textflags), color(color), embosscolor(embosscolor)
{
}


/* EGTextContext */

EGTextContext::EGTextContext() {}
EGTextContext::~EGTextContext() {}


/* EGText */

EGTextContextPtr EGText::ctx;

EGText::EGText() :
    texData(NULL), texWidth(0), texHeight(0),
    fontFace(defaultFontFace), fontSize(defaultFontSize), fontStyle(EGFontStyleNormal), scale(defaultScale), textStr(""),
    flags(EGTextHAlignLeft | EGTextVAlignTop), x(0), y(0), angle(0), maxWidth(0), w(0), h(0), wp2(defaultTextureWidth), hp2(defaultTextureHeight),
    rendered(false), sized(false), updated(false)
{
    renderer = new EGTextRendererES2(this);
    setColor(1, 1, 1, 1);
}

EGText::~EGText()
{
    delete renderer;
}

EGfloat EGText::getFontSize() { return fontSize; }
std::string& EGText::getFontFace() { return fontFace; }
std::string& EGText::getText() { return textStr; }
EGenum EGText::getFlags() { return flags; }
EGfloat EGText::getX() { return x; }
EGfloat EGText::getY() { return y; }

const EGRectList& EGText::getGlyphRects()
{
    calcSize();
    calcGlyphRects();
    
    return glyphRects;
}

EGuint EGText::getWidth()
{
    calcSize();

    return (EGuint)(w / scale);
}

EGuint EGText::getHeight()
{
    calcSize();

    return (EGuint)(h / scale);
}

void EGText::calcSize()
{
    if (sized) return;

    createFontContext();
    getTextSize(w, h);
    wp2 = hp2 = 2;
    while (w + 4 > wp2) {
        wp2 <<= 1;
    }
    while (h > hp2) {
        hp2 <<= 1;
    }
    
    sized = true;
}

void EGText::setScale(EGfloat newscale)
{
    if (scale != newscale) {
        fontSize = fontSize / scale * newscale;
        scale = newscale;
        deleteFontContext();
    }
}

void EGText::setFontSize(EGfloat fontSize)
{
    EGfloat newFontSize = fontSize * scale;
    if (newFontSize != this->fontSize) {
        this->fontSize = newFontSize;
        deleteFontContext();
    }
}

void EGText::setFontFace(const std::string &font)
{
    if (font != fontFace) {
        fontFace = font;
        deleteFontContext();
    }
}

void EGText::setFontStyle(EGenum style)
{
    if (style != fontStyle) {
        fontStyle = style;
        deleteFontContext();
    }
}

void EGText::setText(const std::string &text)
{
    if (textStr != text) {
        textStr = text;
        updated = rendered = sized = false;
    }
}

void EGText::setColor(EGfloat r, EGfloat g, EGfloat b, EGfloat a)
{
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;
    embosscolor[0] = 1.0f - r;
    embosscolor[1] = 1.0f - g;
    embosscolor[2] = 1.0f - b;
    embosscolor[3] = a;
    updated = false;
}

void EGText::setColor(const EGColor &newcolor)
{
    color[0] = newcolor.red;
    color[1] = newcolor.green;
    color[2] = newcolor.blue;
    color[3] = newcolor.alpha;
    embosscolor[0] = 1.0f - newcolor.red;
    embosscolor[1] = 1.0f - newcolor.green;
    embosscolor[2] = 1.0f - newcolor.blue;
    embosscolor[3] = newcolor.alpha;
    updated = false;
}

void EGText::setEmbossColor(EGfloat r, EGfloat g, EGfloat b, EGfloat a)
{
    embosscolor[0] = r;
    embosscolor[1] = g;
    embosscolor[2] = b;
    embosscolor[3] = a;
    updated = false;
}

void EGText::setEmbossColor(const EGColor &newcolor)
{
    embosscolor[0] = newcolor.red;
    embosscolor[1] = newcolor.green;
    embosscolor[2] = newcolor.blue;
    embosscolor[3] = newcolor.alpha;
    updated = false;
}

void EGText::setFlags(EGenum flags)
{
    this->flags = flags;
    updated = false;
}

void EGText::setPosition(EGfloat x, EGfloat y)
{
    this->x = x;
    this->y = y;
    updated = false;
}

void EGText::setRotation(EGfloat angle)
{
    if (this->angle != angle) {
        this->angle = angle;
        updated = false;
    }
}

void EGText::setMaximumWidth(EGuint maxWidth)
{
    if (this->maxWidth != maxWidth) {
        this->maxWidth = maxWidth;
        updated = rendered = sized = false;
    }
}

void EGText::clearBuffers()
{
    deleteFontContext();
    deleteContext();
    renderer->clear();
    updated = false;
}

EGbool EGText::prepare()
{
    calcSize();
    if (!rendered) {
        renderText();
        return true;
    } else {
        return false;
    }
}

void EGText::draw()
{
    renderer->update();
    renderer->draw();
}

EGText* EGText::create()
{
#if __APPLE__
    return static_cast<EGText*>(new EGTextMac());
#elif __unix__ || __linux__ || EMSCRIPTEN || __native_client__
    return static_cast<EGText*>(new EGTextCairo());
#elif _WIN32 && USE_DIRECT_WRITE
    return static_cast<EGText*>(new EGTextDWrite());
#elif _WIN32 && !USE_DIRECT_WRITE
    return static_cast<EGText*>(new EGTextGDI());
#endif
    return NULL;
}

EGTextPtr EGText::create(const std::string &fontface, EGfloat fontsize, EGenum fontstyle, EGenum textflags)
{
    EGText *text = create();
    text->setFontFace(fontface);
    text->setFontSize(fontsize);
    text->setFontStyle(fontstyle);
    text->setFlags(textflags);
    return EGTextPtr(text);
}

EGTextPtr EGText::create(const std::string &textstring, const EGTextStyle &textstyle)
{
    EGText *text = create();
    text->setFontFace(textstyle.fontface);
    text->setFontSize(textstyle.fontsize);
    text->setFontStyle(textstyle.fontstyle);
    text->setFlags(textstyle.textflags);
    text->setColor(textstyle.color);
    text->setEmbossColor(textstyle.embosscolor);
    text->setText(textstring);
    return EGTextPtr(text);
}

EGbool EGText::init()
{
#if __APPLE__
    return EGTextMac::init();
#elif __unix__ || __linux__ || EMSCRIPTEN || __native_client__
    return EGTextCairo::init();
#elif _WIN32 && USE_DIRECT_WRITE
    return EGTextDWrite::init();
#elif _WIN32 && !USE_DIRECT_WRITE
    return EGTextGDI::init();
#endif
}
