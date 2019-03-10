// See LICENSE for license details.

#pragma once

#if defined(__unix__) || defined(__linux__) || defined(__native_client__)

#if EMSCRIPTEN
#include <libc/locale.h>
#endif

#include <cairo.h>
#include <cairo-ft.h>

#if USE_HARFBUZZ
#include <hb.h>
#include <hb-ft.h>
#endif

#if USE_FONTCONFIG
#include <fontconfig.h>
#include <fcfreetype.h>
#endif


/* EGFontCairo */

class EGFontCairo : public EGFont
{
protected:
    EGResourcePtr fontRsrc;
    FT_Face ft_face;
    cairo_font_face_t *cr_face;
    
public:
    EGFontCairo(EGFontRec fontRec, EGResourcePtr fontRsrc, FT_Face ft_face);
    virtual ~EGFontCairo();
    
    cairo_scaled_font_t* getScaledFont(EGfloat size);
};


/* EGTextContextCairo */

class EGTextContextCairo : public EGTextContext
{
public:
    FT_Library ftLib;
    
    EGTextContextCairo();
    ~EGTextContextCairo();
    
    void initFonts();
    void initOneFont(EGResourcePtr fontRsrc);
};


/* EGTextCairo */

class EGTextCairo: public EGText
{
private:
    cairo_t *cr;
    cairo_surface_t *surface;
    cairo_scaled_font_t *scaled_font;
    
public:
    EGTextCairo();
    ~EGTextCairo();
    
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
