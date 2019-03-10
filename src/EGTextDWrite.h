// See LICENSE for license details.

#pragma once

#if _WIN32 && USE_DIRECT_WRITE

#include <windows.h>
#include <gdiplus.h>
#include <Wincodec.h>
#include <d2d1.h>
#include <dwrite.h>


/* EGFontDWrite */

class EGFontDWrite : public EGFont
{
protected:
    IDWriteFont *font;
    
public:
    EGFontDWrite(EGFontRec fontRec, IDWriteFont *font);
    virtual ~EGFontDWrite();

	IDWriteTextFormat* getTextFormat(IDWriteFactory* pDWriteFactory, EGfloat fontSize);
};


/* EGTextContextDWrite */

typedef std::map<EGFontCacheKey,IDWriteTextFormat*> EGFontCacheMap;

class EGTextContextDWrite : public EGTextContext
{
public:
    IWICImagingFactory* pWICFactory;
    ID2D1Factory* pD2DFactory;
    IDWriteFactory* pDWriteFactory;
    EGFontCacheMap fontCache;
    
    EGTextContextDWrite();
    ~EGTextContextDWrite();
    
    IDWriteTextFormat* getFont(std::string fontFace, EGfloat fontSize, EGenum fontStyle);

    void initFonts();
    void initOneFont(int fontrsrcid);
    void initOneFont(EGResourcePtr fontRsrc);
};

/* EGTextDWrite */

class EGTextDWrite: public EGText
{
private:    
    IWICBitmap *pBitmap;
    ID2D1RenderTarget* pRT;
    ID2D1SolidColorBrush* pBlackBrush;
    IDWriteTextFormat* pTextFormat;
    
public:
    EGTextDWrite();
    ~EGTextDWrite();
    
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
