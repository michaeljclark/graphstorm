// See LICENSE for license details.

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
#include "EGTextGDI.h"


/*
 * EGTextGDI - Windows GDI implementation
 */

#if _WIN32 && !USE_DIRECT_WRITE

static const char class_name[] = "EGTextGDI";

static wchar_t* utf8ToWide(const char *str, int *outlen = NULL)
{
    int buflen = MultiByteToWideChar(CP_UTF8, 0, (char*)str, -1, NULL, 0);
    wchar_t *wstr = new wchar_t[buflen+1];
    wstr[0] = '\0';
    int len = MultiByteToWideChar(CP_UTF8, 0, (char*)str, -1, wstr, buflen+1);
	if (outlen) *outlen = len > 0 ? len - 1 : 0;
    return wstr;
}


/* EGFontGDI */

EGFontGDI::EGFontGDI(EGFontRec fontRec, LOGFONT lfTemplate) : EGFont(fontRec), lfTemplate(lfTemplate)
{

}

EGFontGDI::~EGFontGDI()
{

}

HFONT EGFontGDI::getHFont(HDC hdc, EGfloat fontSize)
{
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = (long)(-MulDiv((EGint)fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 100));
    lf.lfWidth = 0;
    lf.lfItalic = lfTemplate.lfItalic;
    lf.lfQuality = 6; //CLEARTYPE_NATURAL_QUALITY
    lf.lfWeight = lfTemplate.lfWeight;
    memcpy(lf.lfFaceName, lfTemplate.lfFaceName, sizeof(lf.lfFaceName));

	HFONT hfont;
	if ((hfont = CreateFontIndirect(&lf)) == NULL) {
        EGDebug("%s:%s CreateFontIndirect failed: errcode=%d\n", class_name, __func__, GetLastError());
    }
	return hfont;
}


/* EGTextContextGDI */

EGTextContextGDI::EGTextContextGDI() : hdc(NULL)
{
    // Create drawing context
    if ((hdc = CreateCompatibleDC(NULL)) == NULL) {
        EGDebug("%s:%s failed to create device context: errcode=%d\n", class_name, __func__, GetLastError());
    }
    initFonts();
}

EGTextContextGDI::~EGTextContextGDI()
{
    // delete font handles
    for (EGFontCacheMap::iterator fci = fontCache.begin(); fci != fontCache.end(); fci++) {
        DeleteObject((*fci).second);
    }
    // delete device context
    if (hdc) {
        DeleteDC(hdc);
    }
}

void EGTextContextGDI::initOneFont(int fontrsrcid)
{
    DWORD   fontcount = 0;
    HRSRC   fontrsrc = FindResource(NULL, MAKEINTRESOURCE(fontrsrcid), RT_FONT);
    if (fontrsrc) {
        DWORD   fontlen   = SizeofResource(NULL, fontrsrc);
        HGLOBAL fontrsrch  = LoadResource(NULL, fontrsrc);
        HANDLE  fonth = AddFontMemResourceEx(fontrsrch, fontlen, 0, &fontcount);
        if (fontcount == 0) {
            EGError("%s:%s couldn't init font: resource %d could not be added\n", class_name, __func__, fontrsrcid);
        }
    } else {
        EGError("%s:%s couldn't init font: resource %d not found\n", class_name, __func__,fontrsrcid);
    }
}

void EGTextContextGDI::initOneFont(EGResourcePtr fontRsrc)
{
    DWORD fontcount = 0;
    HANDLE fonth = AddFontMemResourceEx((PVOID)fontRsrc->getBuffer(), fontRsrc->getLength(), 0, &fontcount);
    if (fontcount == 0) {
		EGError("%s:%s couldn't init font: resource %d could not be added\n", class_name, __func__, fontRsrc->getBasename().c_str());
    }
}

static EGFontWeight convertFontWeight(int weight)
{
	struct FontWeightMap { EGenum weight; EGFontWeight fWeight; };

	static FontWeightMap fontWeightMap[] = {
		{ 100,	EGFontWeightThin },
		{ 200,	EGFontWeightExtraLight }, 
		{ 300,	EGFontWeightLight },
		{ 350,	EGFontWeightSemiLight },
		{ 400,	EGFontWeightRegular },
		{ 500,	EGFontWeightMedium },
		{ 600,	EGFontWeightSemiBold },
		{ 700,	EGFontWeightBold },
		{ 800,	EGFontWeightExtraBold },
		{ 900,	EGFontWeightBlack },
		{ 950,	EGFontWeightExtraBlack },
		{ -1,	EGFontWeightAny }
	};

	EGFontWeight fontWeight = EGFontWeightNormal;
	FontWeightMap *ent = fontWeightMap;
	while (ent->weight != -1) {
		if (ent->weight == weight) {
			fontWeight = ent->fWeight;
			break;
		}
		ent++;
	}
	return fontWeight;
}

int CALLBACK enumFontsCallback(const LOGFONT *lf, const TEXTMETRIC *tm, DWORD FontType, LPARAM lParam)
{
	return ((EGTextContextGDI*)lParam)->enumFonts(lf, tm, FontType);
}

int EGTextContextGDI::enumFonts(const LOGFONT *lf, const TEXTMETRIC *tm, DWORD FontType)
{
	if (FontType != TRUETYPE_FONTTYPE || lf->lfFaceName[0] == '@') return 1;

	EGFontWeight fontWeight = convertFontWeight(lf->lfWeight);
	EGFontSlant fontSlant = lf->lfItalic ? EGFontSlantItalic : EGFontSlantNone;
	EGFontStretch fontStretch = EGFontStretchMedium;
	EGFontSpacing fontSpacing = (lf->lfPitchAndFamily & FIXED_PITCH) ? EGFontSpacingMonospaced : EGFontSpacingNormal;
	EGstring familyName = lf->lfFaceName;
	EGstring fontName = EGFont::synthesizeFontName(familyName, fontWeight, fontSlant, fontStretch);
	EGFontRec fontRec(familyName, fontName, fontWeight, fontSlant, fontStretch, fontSpacing);
	EGFont::addFont(EGFontPtr(new EGFontGDI(fontRec, *lf)));

	return 1;
}

int CALLBACK enumFamiliesCallback(const LOGFONT *lf, const TEXTMETRIC *tm, DWORD FontType, LPARAM lParam)
{
	return ((EGTextContextGDI*)lParam)->enumFamilies(lf, tm, FontType);
}

int EGTextContextGDI::enumFamilies(const LOGFONT *lf, const TEXTMETRIC *tm, DWORD FontType)
{
	if (FontType != TRUETYPE_FONTTYPE) return 1;

	LOGFONT facelf;
    memset(&facelf, 0, sizeof(LOGFONT));
	facelf.lfCharSet = ANSI_CHARSET;
	memcpy(facelf.lfFaceName, lf->lfFaceName, sizeof(facelf.lfFaceName));
	facelf.lfPitchAndFamily = 0;
	facelf.lfQuality = 6;
	EnumFontFamiliesEx(hdc, &facelf, &enumFontsCallback, (LPARAM)this, 0);

	return 1;
}

void EGTextContextGDI::initFonts()
{
	LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfFaceName[0] = '\0';
	lf.lfPitchAndFamily = 0;
	lf.lfQuality = 6;
	EnumFontFamiliesEx(hdc, &lf, &enumFamiliesCallback, (LPARAM)this, 0);
}


/* EGTextGDI */

EGTextGDI::EGTextGDI() : bmi(), hfont(NULL)
{
    if (!ctx) {
        ctx = EGTextContextPtr(new EGTextContextGDI());
    }
}

EGTextGDI::~EGTextGDI()
{
    deleteFontContext();
    deleteContext();
}

void EGTextGDI::createContext(EGuint width, EGuint height)
{
    texWidth = width;
    texHeight = height;
    texData = calloc(texWidth * texHeight, 1);
    
    // Create bitmap info
    memset(&bmi, 0, sizeof(BITMAPINFOHEADER));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = texWidth;
    bmi.bmiHeader.biHeight = texHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    // create bitmap
	if ((hbitmap = CreateDIBSection(static_cast<EGTextContextGDI*>(ctx.get())->hdc, &bmi, DIB_RGB_COLORS, (LPVOID*)&pixData, NULL, 0)) == NULL) {
        EGDebug("%s:%s failed to create %ux%u bitmap: errcode=%d\n", class_name, __func__, texWidth, texHeight, GetLastError());
        pixData = NULL;
    }
}

void EGTextGDI::deleteContext()
{
    if (hbitmap) {
        DeleteObject(hbitmap);
        hbitmap = NULL;
    }
    if (texData) {
        free(texData);
        texData = 0;
        texWidth = texHeight = 0;
    }
    rendered = false;
}

void EGTextGDI::createFontContext()
{
    if (hfont) return;
	HDC hdc = static_cast<EGTextContextGDI*>(ctx.get())->hdc;
	EGFontPtr font = EGFont::findFont(fontFace, (EGFontStyle)fontStyle);
    if (font) {
        EGFontGDI *gdifont = static_cast<EGFontGDI*>(font.get());
        hfont = gdifont->getHFont(hdc, fontSize);
    }
}

void EGTextGDI::deleteFontContext()
{
    hfont = NULL;
    updated = rendered = sized = false;
#if USE_UNISCRIBE
	layout.clear();
	glyphRects.clear();
#endif
}

void EGTextGDI::getTextSize(EGuint &width, EGuint &height)
{
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = texWidth;
    rect.bottom = texHeight;
	HDC hdc = static_cast<EGTextContextGDI*>(ctx.get())->hdc;

	SelectObject(hdc, hfont);

    if (!GetTextMetrics(hdc, &tm)) {
        EGDebug("%s:%s GetTextMetrics failed: errcode=%d\n", class_name, __func__, GetLastError());
        width = height = 0;
        return;
    }
    
	int wstrLen;
    wchar_t *wstr = utf8ToWide(textStr.c_str(), &wstrLen);

#if USE_UNISCRIBE
	HRESULT hr;

	// Call ScriptItemize to break unicode string into individually shapeable items
	SCRIPT_CONTROL control;
	SCRIPT_STATE state;
	ZeroMemory(&control, sizeof(SCRIPT_CONTROL));
	ZeroMemory(&state, sizeof(SCRIPT_STATE));
	state.uBidiLevel = 0; // left to right

	int max_items = 16;
	layout.clear();
	while (true) {
	    layout.items.resize(max_items);
		HRESULT hr = ScriptItemize(wstr, wstrLen, max_items - 1, &control,
                               &state, &(layout.items)[0], &layout.generatedItems);
	    if (SUCCEEDED(hr)) {
			break;
		} else if (hr != E_OUTOFMEMORY) {
			EGDebug("%s:%s ScriptItemize failed: errcode=%d\n", class_name, __func__, GetLastError());
			width = height = 0;
			delete [] wstr;
			return;
		}
	    max_items *= 2;
	}

	// resize layout for number of runs
	layout.resize();

	// Call ScriptLayout  to converts an array of run embedding levels to a
	// map of visual-to-logical position and/or logical-to-visual position.
	for (size_t i = 0; i < layout.generatedItems; i++) {
		layout.directions[i] = layout.items[i].a.s.uBidiLevel;
	}
	hr = ScriptLayout(layout.generatedItems, &layout.directions[0], &layout.visualToLogical[0], &layout.logicalToVisual[0]);
	if (FAILED(hr)) {
		EGDebug("%s:%s ScriptLayout failed: errcode=%d\n", class_name, __func__, GetLastError());
		width = height = 0;
		delete [] wstr;
		return;
	}
	
	// Call ScriptShape to generates glyphs and visual attributes for each Unicode run
	// Call ScriptPlace to generate glyph advance width and offset information
	SCRIPT_CACHE cache = NULL;
	int totalWidth = 0;
	for (size_t i = 0; i < layout.generatedItems; i++) {
		GlyphRun &run = layout.runs[i];
		wchar_t* inputText = &wstr[layout.items[i].iCharPos];
		int inputLength = (i == layout.generatedItems - 1 ? wstrLen : layout.items[i+1].iCharPos) - layout.items[i].iCharPos;
		int initialSize = inputLength * 3 / 2 + 16;
		run.glyphs.resize(initialSize);
		run.visattr.resize(initialSize);
		run.logs.resize(inputLength);
		while (true) {
			int glyphsUsed;
			hr = ScriptShape(hdc, &cache, inputText, inputLength, run.glyphs.size(), &layout.items[i].a,
							 &(run.glyphs)[0], &(run.logs)[0], &(run.visattr)[0], &glyphsUsed);

			if (SUCCEEDED(hr)) {
				run.glyphs.resize(glyphsUsed);
				run.advances.resize(glyphsUsed); // used by ScriptPlace
				run.offsets.resize(glyphsUsed); // used by ScriptPlace
				break;
			} else if (hr == E_OUTOFMEMORY) {
				run.glyphs.resize(run.glyphs.size() * 2);
				run.visattr.resize(run.glyphs.size() * 2); 
		    } else {
				// handle USP_E_SCRIPT_NOT_IN_FONT
				EGDebug("%s:%s ScriptShape failed: errcode=%d\n", class_name, __func__, GetLastError());
				width = height = 0;
				ScriptFreeCache(&cache);
				delete [] wstr;
				return;
			}
		}
		hr = ScriptPlace(hdc, &cache,
                         &run.glyphs[0], run.glyphs.size(), &run.visattr[0],	// from ScriptShape
                         &layout.items[i].a,									// from ScriptItemize
                         &run.advances[0],										// Output: glyph advances
                         &run.offsets[0],										// Output: glyph offsets
                         &run.abc);												// Output: size of run
		if (FAILED(hr)) {
			EGDebug("%s:%s ScriptPlace failed: errcode=%d\n", class_name, __func__, GetLastError());
			width = height = 0;
			ScriptFreeCache(&cache);
			delete [] wstr;
			return;
		}

		totalWidth += run.abc.abcB;
	}

	width = totalWidth;
    height = fontSize + fontSize / 3;
	
	// calculate individual glyph rects
	EGfloat dx = (flags & EGTextHAlignCenter) ? -(EGfloat)width * 0.5f : flags & EGTextHAlignRight ? -(EGfloat)width : 0;
	EGfloat dy = (flags & EGTextVAlignCenter) ? -(EGfloat)height * 0.5f : flags & EGTextVAlignTop ? 0 : -(EGfloat)height;
	glyphRects.clear();
	for (size_t i = 0; i < layout.generatedItems; i++) {
		GlyphRun &run = layout.runs[i];
		for (EGint i = 0; i < run.glyphs.size(); i++) {
			EGint cx = (EGint)floorf(dx);
			EGint cy = (EGint)floorf(dy);
			EGint cw = (EGint)ceilf(run.advances[i]);
			EGint ch = (EGint)height;
			if (maxWidth > 0 && cx + cw > maxWidth) break;
			glyphRects.push_back(EGRect(cx, cy, cw, ch));
			dx += run.advances[i];
		}
	}

	ScriptFreeCache(&cache);
    delete [] wstr;
#else
    if (!DrawTextW(hdc, wstr, -1, &rect, DT_CALCRECT | DT_NOPREFIX)) {
        EGDebug("%s:%s DrawTextW failed: errcode=%d\n", class_name, __func__, GetLastError());
        width = height = 0;
    } else {
        width = rect.right - rect.left;
        height = tm.tmHeight;
    }
    delete [] wstr;
#endif
}

void EGTextGDI::calcGlyphRects()
{
	//
}

void EGTextGDI::renderText()
{
    if (rendered) return;
    
    if (texWidth < wp2 || texHeight < hp2) {
        deleteContext();
        createContext(wp2, hp2);
    }
    
    if (!pixData) return;

    RECT rect;
    rect.left = flags & EGTextHAlignCenter ? (texWidth - w) / 2 : flags & EGTextHAlignRight ? texWidth - w : 0;
    rect.top = flags & EGTextVAlignCenter ? (texHeight - fontSize- tm.tmDescent) / 2 : flags & EGTextVAlignTop ? 0 : texHeight - fontSize - tm.tmDescent;
    rect.right = texWidth;
    rect.bottom = texHeight;
    
    HDC hdc = static_cast<EGTextContextGDI*>(ctx.get())->hdc;
    SelectObject(hdc, hbitmap);
    SelectObject(hdc, hfont);

	FillRect(hdc, &rect,(HBRUSH)GetStockObject(BLACK_BRUSH));

	SetBkColor(hdc, 0x00000000);
    SetBkMode(hdc, OPAQUE);
    SetTextColor(hdc, 0x00FFFFFF);

#if USE_UNISCRIBE
	SCRIPT_CACHE cache = NULL;
	int xoffset = 0;
	for (size_t i = 0; i < layout.generatedItems; i++) {
		int runIdx = layout.visualToLogical[i];
		GlyphRun &run = layout.runs[runIdx];
		HRESULT hr = ScriptTextOut(hdc, &cache, rect.left + xoffset, rect.top, 0, &rect, &layout.items[runIdx].a, NULL, 0,
									&run.glyphs[0], run.glyphs.size(), &run.advances[0], NULL, &run.offsets[0]);
		if (FAILED(hr)) {
			EGDebug("%s:%s ScriptTextOut failed: errcode=%d\n", class_name, __func__, GetLastError());
		}
		xoffset += run.abc.abcA + run.abc.abcB + run.abc.abcC;
	}
	ScriptFreeCache(&cache);
#else
    SetTextAlign(hdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);

    wchar_t *wstr = utf8ToWide(textStr.c_str());
    if (!DrawTextW(hdc, wstr, -1, &rect, DT_SINGLELINE | DT_NOCLIP | DT_NOPREFIX)) {
        EGDebug("%s:%s DrawTextW failed: errcode=%d\n", class_name, __func__, GetLastError());
    }
    delete [] wstr;
#endif
    
    GdiFlush();
    
    /* copy from 24bit dib to 8 bit texture */
    char *t = (char*)texData, *p = pixData;
    for(unsigned int i=0; i < texWidth*texHeight; i++) {
        *t++ = *p;
        p += 3;
    }
    
    rendered = true;
}

EGbool EGTextGDI::init()
{
    if (!ctx) {
        ctx = EGTextContextPtr(new EGTextContextGDI());
    }
    return true;
}

#endif

