/*
 *  EGTextCairo.cc
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
#include "EGText.h"
#include "EGTextCairo.h"


/*
 * EGTextCairo - UNIX/Linux cairo implementation
 */

#if defined(__unix__) || defined(__linux__) || defined(__native_client__)

static const char class_name[] = "EGTextCairo";


/* EGFontCairo */

EGFontCairo::EGFontCairo(EGFontRec fontRec, EGResourcePtr fontRsrc, FT_Face ft_face) : EGFont(fontRec), fontRsrc(fontRsrc), ft_face(ft_face), cr_face(NULL)
{
    if ((cr_face = cairo_ft_font_face_create_for_ft_face(ft_face, 0 /*FT_LOAD_FORCE_AUTOHINT*/)) == NULL) {
        EGstring psName = FT_Get_Postscript_Name(ft_face);
        EGError("%s:%s can't create cairo face for font %s\n", class_name, __func__, psName.c_str());
    } else {
        cairo_font_face_set_user_data (cr_face, NULL, ft_face, (cairo_destroy_func_t) FT_Done_Face);
    }
}

EGFontCairo::~EGFontCairo()
{
    if (cr_face) {
        cairo_font_face_destroy(cr_face);
    }
}

cairo_scaled_font_t* EGFontCairo::getScaledFont(EGfloat fontSize)
{
    if (!cr_face) return NULL;
    cairo_matrix_t ctm, font_matrix;
    ctm.xy = ctm.yx = 0;
    ctm.xx = ctm.yy = 1;
    font_matrix.xy = font_matrix.yx = 0;
    font_matrix.xx = font_matrix.yy = fontSize;
    cairo_font_options_t *font_options = cairo_font_options_create();
    cairo_font_options_set_antialias(font_options, CAIRO_ANTIALIAS_GRAY);
    cairo_font_options_set_hint_metrics(font_options, CAIRO_HINT_METRICS_OFF); // CAIRO_HINT_METRICS_ON
    cairo_font_options_set_hint_style(font_options, CAIRO_HINT_STYLE_NONE); // CAIRO_HINT_STYLE_SLIGHT
    cairo_scaled_font_t *scaled_font = cairo_scaled_font_create(cr_face, &font_matrix, &ctm, font_options);
    return scaled_font;
}


/* EGTextContextCairo */

EGTextContextCairo::EGTextContextCairo() : ftLib(NULL)
{
    if (ftLib == NULL && FT_Init_FreeType(&ftLib)) {
        EGError("%s:%s can't initialize freetype\n", class_name, __func__);
        return;
    }
    initFonts();
}

EGTextContextCairo::~EGTextContextCairo()
{
}

void EGTextContextCairo::initOneFont(EGResourcePtr fontRsrc)
{
    FT_Face ft_face;
    if (FT_New_Memory_Face(ftLib, (const FT_Byte*)fontRsrc->getBuffer(), fontRsrc->getLength(), 0, &ft_face)) {
        EGError("%s:%s can't load memory font\n", class_name, __func__);
        return;
    }
    EGstring psName = FT_Get_Postscript_Name(ft_face);
    EGFontRec fontRec = EGFont::createFontRecord(psName, ft_face->family_name, ft_face->style_name);
    EGFontPtr font(new EGFontCairo(fontRec, fontRsrc, ft_face));
    EGFont::addFont(font);
}

void EGTextContextCairo::initFonts()
{
    char buf[256];
    EGResourcePtr rsrc = EGResource::getResource("Resources/fonts.bundle/_index.txt");
    while (rsrc->readLine(buf, sizeof(buf))) {
        EGstring filename(buf);
        if (filename.length() == 0 || filename[0] == '#') continue;
        EGResourcePtr fontRsrc = EGResource::getResource("Resources/fonts.bundle/" + filename);
        initOneFont(fontRsrc);
    }
    rsrc->close();
    
#if USE_FONTCONFIG
    FcPattern *pat;
    FcFontSet *fs;
    FcObjectSet *os;
    FcChar8 *family, *style, *file;
    int weight, slant, width;
    FcConfig *config;
    FcBool result;
    FT_Face font_face;
    int i;
    
    result = FcInit();
    config = FcConfigGetCurrent();
    FcConfigSetRescanInterval(config, 0);
    
    pat = FcPatternBuild(NULL, FC_OUTLINE, FcTypeBool, FcTrue, FC_SCALABLE, FcTypeBool, FcTrue, NULL);
    os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, FC_WEIGHT, FC_SLANT, FC_WIDTH, FC_LANG, FC_FILE, NULL);
    fs = FcFontList(config, pat, os);
    for (i=0; fs && i < fs->nfont; i++) {
        FcPattern *font = fs->fonts[i];
        if (FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
            FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch &&
            FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
            FcPatternGetInteger(font, FC_WEIGHT, 0, &weight) == FcResultMatch &&
            FcPatternGetInteger(font, FC_SLANT, 0, &slant) == FcResultMatch &&
            FcPatternGetInteger(font, FC_WIDTH, 0, &width) == FcResultMatch)
        {
            printf("family=%-25s style=%-15s weight=%-3d slant=%-3d width=%-3d filename=%s\n",
                   family, style, weight, slant, width, file);
        }
        if (FcPatternGetFTFace (font, FC_FT_FACE, 0, &font_face) == FcResultMatch) {
            printf("FT_Face=%s\n", FT_Get_Postscript_Name(font_face));
        }
    }
    if (fs) FcFontSetDestroy(fs);
#endif
}


/* EGTextCairo */

EGTextCairo::EGTextCairo() : cr(NULL), surface(NULL), scaled_font(NULL)
{
    if (!ctx) {
        ctx = EGTextContextPtr(new EGTextContextCairo());
    }
}

EGTextCairo::~EGTextCairo()
{
    deleteFontContext();
    deleteContext();
}

void EGTextCairo::createContext(EGuint width, EGuint height)
{
    texWidth = width;
    texHeight = height;
    texData = calloc(texWidth, texHeight);
    surface = cairo_image_surface_create_for_data((unsigned char*) texData, CAIRO_FORMAT_A8, texWidth, texHeight, texWidth);
    cr = cairo_create(surface);
    cairo_scale(cr, 1.0, -1.0);
    
    EGFontPtr font = EGFont::findFont(fontFace, (EGFontStyle)fontStyle);
    if (font) {
        EGFontCairo *cairofont = static_cast<EGFontCairo*>(font.get());
        scaled_font = cairofont->getScaledFont(fontSize);
        if (scaled_font) {
            cairo_set_scaled_font(cr, scaled_font);
        }
    }
}

void EGTextCairo::deleteContext()
{
    if (scaled_font) {
        cairo_scaled_font_destroy(scaled_font);
        scaled_font = NULL;
    }
    if (surface) {
        cairo_surface_destroy(surface);
        surface = NULL;
    }
    if (cr) {
        cairo_destroy(cr);
        cr = NULL;
    }
    if (texData) {
        free(texData);
        texData = 0;
        texWidth = texHeight = 0;
    }
    rendered = false;
}

void EGTextCairo::createFontContext()
{
    if (texWidth < defaultTextureWidth || texHeight < defaultTextureHeight) {
        deleteContext();
        createContext(wp2, hp2);
    }    
}

void EGTextCairo::deleteFontContext()
{
    deleteContext();
    glyphRects.clear();
    updated = rendered = sized = false;
}

void EGTextCairo::getTextSize(EGuint &width, EGuint &height)
{    
#if USE_HARFBUZZ
    FT_Face ft_face = cairo_ft_scaled_font_lock_face(scaled_font);
    hb_font_t *hb_font = hb_ft_font_create(ft_face, NULL);

    hb_buffer_t *buf = hb_buffer_create();
    hb_buffer_set_unicode_funcs(buf, hb_unicode_funcs_get_default());
    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buf, hb_language_from_string("en", 2));
    
    hb_buffer_add_utf8(buf, textStr.c_str(), textStr.length(), 0, textStr.length());
    hb_shape(hb_font, buf, NULL, 0);
    
    // calculate total width
    EGuint glyph_count;
    EGuint total_width = 0;
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
    for (size_t i = 0; i < glyph_count; i++) {
        total_width += glyph_pos[i].x_advance;
    }
    
    width = (total_width + 63) / 64;
    if (maxWidth > 0) width = (std::min)(width, maxWidth);
    height = fontSize + fontSize / 3;

    // calculate individual glyph rects
    EGfloat dx = (flags & EGTextHAlignCenter) ? - (EGfloat)width * 0.5f : flags & EGTextHAlignRight ? -(EGfloat)width : 0;
    EGfloat dy = (flags & EGTextVAlignCenter) ? - (EGfloat)height * 0.5f : flags & EGTextVAlignTop ? 0 : -(EGfloat)height;
    glyphRects.clear();
    for (size_t i = 0; i < glyph_count; i++) {
        EGint cx = (EGint)floorf(dx);
        EGint cy = (EGint)floorf(dy);
        EGint cw = (EGint)ceilf(glyph_pos[i].x_advance / 64.0f);
        EGint ch = (EGint)height;
        if (maxWidth > 0 && (EGuint)(cx + cw) > maxWidth) break;
        glyphRects.push_back(EGRect(cx, cy, cw, ch));
        dx += glyph_pos[i].x_advance / 64.0f;
    }

    hb_buffer_destroy(buf);
    hb_font_destroy(hb_font);
    cairo_ft_scaled_font_unlock_face(scaled_font);
#else
    cairo_font_extents_t fontExtents;
    cairo_text_extents_t textExtents;
    
    cairo_font_extents(cr, &fontExtents);
    cairo_text_extents(cr, textStr.c_str(), &textExtents);
    
    width = textExtents.width + 2;
    if (maxWidth > 0) width = (std::min)(width, maxWidth);
    height = fontExtents.height;
#endif
}

void EGTextCairo::calcGlyphRects()
{
    //
}

void EGTextCairo::renderText()
{
    if (rendered) return;
    
    if (texWidth < wp2 || texHeight < hp2) {
        deleteContext();
        createContext(wp2, hp2);
    }
    
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    const EGfloat descent = fontSize * 0.225f;
    EGfloat dx = flags & EGTextHAlignCenter ? (texWidth - w) * 0.5f : flags & EGTextHAlignRight ? (texWidth - w) : 0;
    EGfloat dy = flags & EGTextVAlignCenter ? -(texHeight - fontSize + descent) * 0.5f : flags & EGTextVAlignTop ? -(texHeight - fontSize + descent) : -descent;
    cairo_move_to(cr, dx, dy);
    
#if USE_HARFBUZZ
    FT_Face ft_face = cairo_ft_scaled_font_lock_face(scaled_font);
    hb_font_t *hb_font = hb_ft_font_create(ft_face, NULL);
    
    hb_buffer_t *buf = hb_buffer_create();
    hb_buffer_set_unicode_funcs(buf, hb_unicode_funcs_get_default());
    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buf, hb_language_from_string("en", 2));
    
    /* Layout the text */
    hb_buffer_add_utf8(buf, textStr.c_str(), textStr.length(), 0, textStr.length());
    hb_shape(hb_font, buf, NULL, 0);
    
    EGuint glyph_count;
    EGfloat x = dx, y = dy;
    hb_glyph_info_t     *glyph_info   = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t *glyph_pos    = hb_buffer_get_glyph_positions(buf, &glyph_count);
    cairo_glyph_t       *cairo_glyphs = new cairo_glyph_t[glyph_count];
    EGfloat totalWidth = 0;
    for (size_t i=0; i < glyph_count; ++i) {
        if (maxWidth > 0 && totalWidth + glyph_pos[i].x_advance / 64.0f > maxWidth) {
            glyph_count = i;
            break;
        }
        cairo_glyphs[i].index = glyph_info[i].codepoint;
        cairo_glyphs[i].x = x + glyph_pos[i].x_offset / 64.0f;
        cairo_glyphs[i].y = y - glyph_pos[i].y_offset / 64.0f;
        x += glyph_pos[i].x_advance / 64.0f;
        y -= glyph_pos[i].y_advance / 64.0f;
        totalWidth += glyph_pos[i].x_advance / 64.0f;
    }
    cairo_show_glyphs(cr, cairo_glyphs, glyph_count);
    
    delete [] cairo_glyphs;
    hb_font_destroy(hb_font);
    hb_buffer_destroy(buf);
    cairo_ft_scaled_font_unlock_face(scaled_font);
#else
    cairo_show_text(cr, textStr.c_str());
#endif
    
    rendered = true;
}

EGbool EGTextCairo::init()
{
    if (!ctx) {
        ctx = EGTextContextPtr(new EGTextContextCairo());
    }
    return true;
}

#endif

