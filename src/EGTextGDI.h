// See LICENSE for license details.

#pragma once

#if _WIN32 && !USE_DIRECT_WRITE

#include <windows.h>
#include <gdiplus.h>
#if USE_UNISCRIBE
#include <Usp10.h>
#endif


/* EGFontGDI */

class EGFontGDI : public EGFont
{
protected:
    LOGFONT lfTemplate;
	HFONT hfont;
    
public:
    EGFontGDI(EGFontRec fontRec, LOGFONT lfTemplate);
    virtual ~EGFontGDI();

    HFONT getHFont(HDC hdc, EGfloat fontSize);
};


/* EGTextContextGDI */

typedef std::map<EGFontCacheKey,HFONT> EGFontCacheMap;

class EGTextContextGDI : public EGTextContext
{
public:
    HDC hdc;
    EGFontCacheMap fontCache;
    
    EGTextContextGDI();
    ~EGTextContextGDI();
    
	int enumFonts(const LOGFONT *lf, const TEXTMETRIC *tm, DWORD FontType);
	int enumFamilies(const LOGFONT *lf, const TEXTMETRIC *tm, DWORD FontType);

    void initFonts();
    void initOneFont(int fontrsrcid);
    void initOneFont(EGResourcePtr fontRsrc);
};

/* EGTextGDI */

class EGTextGDI: public EGText
{
private:
    HFONT hfont;
    BITMAPINFO bmi;
    HBITMAP hbitmap;
	TEXTMETRIC tm;
    char *pixData;

#if USE_UNISCRIBE
	struct GlyphRun {
		std::vector<WORD> glyphs;
		std::vector<SCRIPT_VISATTR> visattr;
		std::vector<WORD> logs;
		std::vector<int> advances;
		std::vector<GOFFSET> offsets;
		ABC abc;
	};
	struct GlyphLayout {
		int generatedItems;
		std::vector<SCRIPT_ITEM> items;
		std::vector<int> visualToLogical;
		std::vector<int> logicalToVisual;
		std::vector<BYTE> directions;
		std::vector<GlyphRun> runs;

		void clear() {
			items.clear();
			visualToLogical.clear();
			logicalToVisual.clear();
			directions.clear();
			runs.clear();
		}

		void resize() {
			visualToLogical.resize(generatedItems);
			logicalToVisual.resize(generatedItems);
			directions.resize(generatedItems);
			runs.resize(generatedItems);
		}
	};
	GlyphLayout layout;
#endif
    
public:
    EGTextGDI();
    ~EGTextGDI();
    
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
