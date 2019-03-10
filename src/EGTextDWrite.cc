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
#include "EGTextDWrite.h"


/*
 * EGTextDWrite - Windows DirectWrite implementation
 */

#if _WIN32 && USE_DIRECT_WRITE

static const char class_name[] = "EGTextDWrite";

static wchar_t* utf8ToWide(const char *str, int *outlen = NULL)
{
    int buflen = MultiByteToWideChar(CP_UTF8, 0, (char*)str, -1, NULL, 0);
    wchar_t *wstr = new wchar_t[buflen+1];
    wstr[0] = '\0';
    int len = MultiByteToWideChar(CP_UTF8, 0, (char*)str, -1, wstr, buflen+1);
	if (outlen) *outlen = len > 0 ? len - 1 : 0;
    return wstr;
}


/* EGFontDWrite */

EGFontDWrite::EGFontDWrite(EGFontRec fontRec, IDWriteFont *font) : EGFont(fontRec), font(font)
{
	font->AddRef();
}

EGFontDWrite::~EGFontDWrite()
{
	font->Release();
}

IDWriteTextFormat* EGFontDWrite::getTextFormat(IDWriteFactory* pDWriteFactory, EGfloat fontSize)
{
	wchar_t *fontNameW = utf8ToWide(fontRec.fontFamily.c_str());
	IDWriteTextFormat *pTextFormat = NULL;
    HRESULT hr = pDWriteFactory->CreateTextFormat(
                                                  fontNameW,                  // Font family name.
                                                  NULL,                       // Font collection (NULL sets it to use the system font collection).
												  (DWRITE_FONT_WEIGHT)EGFont::weightTable[fontRec.fontWeight],
                                                  (DWRITE_FONT_STYLE)EGFont::slantTable[fontRec.fontSlant],
                                                  (DWRITE_FONT_STRETCH)EGFont::stretchTable[fontRec.fontSlant],
                                                  fontSize,
                                                  L"en-us",
                                                  &pTextFormat);
	if (FAILED(hr)) {
        EGError("%s:%s unable to create IDWriteTextFormat for font: %s: errcode=%d\n", class_name, __func__, fontRec.fontFamily.c_str(), GetLastError());
	}
	delete [] fontNameW;
	return pTextFormat;
}


/* EGTextContextDWrite */

EGTextContextDWrite::EGTextContextDWrite() : pWICFactory(NULL), pD2DFactory(NULL), pDWriteFactory(NULL)
{
    HRESULT hr;
    
    // Initialize COM (required if we are used on a different thread)
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        EGError("%s:%s CoInitializeEx failed: errcode=%d\n", class_name, __func__, GetLastError());
        return;
    }
    
    // Create WIC factor
    hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWICFactory));
    if (FAILED(hr)) {
        EGError("%s:%s unable to create WICImagingFactory: errcode=%d\n", class_name, __func__, GetLastError());
        return;
    }
    // Create Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pD2DFactory);
    if (FAILED(hr)) {
        EGError("%s:%s unable to create D2D1Factory: errcode=%d\n", class_name, __func__, GetLastError());
        return;
    }
    // Create DirectWrite factory.
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
    if (FAILED(hr)) {
        EGError("%s:%s unable to create DWriteFactory: errcode=%d\n", class_name, __func__, GetLastError());
    }

    initFonts();
}

EGTextContextDWrite::~EGTextContextDWrite()
{
    // release text format handles
    for (EGFontCacheMap::iterator fci = fontCache.begin(); fci != fontCache.end(); fci++) {
        (*fci).second->Release();
    }
#if 0
    // release D2D1Factory
    if (pD2DFactory) {
        EGDebug("%s:%s releasing D2D1Factory\n", class_name, __func__);
        pD2DFactory->Release();
    }
    // release DWriteFactory
    if (pDWriteFactory) {
        EGDebug("%s:%s releasing DWriteFactory\n", class_name, __func__);
        pD2DFactory->Release();
    }
#endif
}

IDWriteTextFormat* EGTextContextDWrite::getFont(std::string fontFace, EGfloat fontSize, EGenum fontStyle)
{
    IDWriteTextFormat* pTextFormat;
    
    // Lookup cache
    EGFontCacheKey key = { fontFace, fontSize, fontStyle };
    EGFontCacheMap::iterator fci = fontCache.find(key);
    if (fci != fontCache.end()) {
        return (*fci).second;
    }
    
    wchar_t *fontNameW = utf8ToWide(fontFace.c_str());
    HRESULT hr = pDWriteFactory->CreateTextFormat(
                                                  fontNameW,                  // Font family name.
                                                  NULL,                       // Font collection (NULL sets it to use the system font collection).
                                                  (fontStyle == EGFontStyleBold || fontStyle == EGFontStyleBoldItalic) ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
                                                  (fontStyle == EGFontStyleItalic || fontStyle == EGFontStyleBoldItalic) ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
                                                  DWRITE_FONT_STRETCH_NORMAL,
                                                  fontSize,
                                                  L"en-us",
                                                  &pTextFormat);
    
    if (FAILED(hr)) {
        EGError("%s:%s unable to create IDWriteTextFormat for font: %s: errcode=%d\n", class_name, __func__, fontFace.c_str(), GetLastError());
    }
    
    delete [] fontNameW;
    
    // Add to cache
    if (pTextFormat) {
        fontCache.insert(std::pair<EGFontCacheKey,IDWriteTextFormat*>(key, pTextFormat));
    }
    
    return pTextFormat;
}

void EGTextContextDWrite::initOneFont(int fontrsrcid)
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

void EGTextContextDWrite::initOneFont(EGResourcePtr fontRsrc)
{
    DWORD fontcount = 0;
    HANDLE fonth = AddFontMemResourceEx((PVOID)fontRsrc->getBuffer(), fontRsrc->getLength(), 0, &fontcount);
    if (fontcount == 0) {
		EGError("%s:%s couldn't init font: resource %d could not be added\n", class_name, __func__, fontRsrc->getBasename().c_str());
    }
}

static EGFontWeight convertFontWeight(DWRITE_FONT_WEIGHT dWeight)
{
	struct FontWeightMap { EGenum dWeight; EGFontWeight fWeight; };

	static FontWeightMap fontWeightMap[] = {
		{ DWRITE_FONT_WEIGHT_THIN,				EGFontWeightThin },
		{ DWRITE_FONT_WEIGHT_EXTRA_LIGHT,		EGFontWeightExtraLight }, 
		{ DWRITE_FONT_WEIGHT_LIGHT, 			EGFontWeightLight },
		{ DWRITE_FONT_WEIGHT_SEMI_LIGHT, 		EGFontWeightSemiLight },
		{ DWRITE_FONT_WEIGHT_REGULAR, 			EGFontWeightRegular },
		{ DWRITE_FONT_WEIGHT_MEDIUM, 			EGFontWeightMedium },
		{ DWRITE_FONT_WEIGHT_SEMI_BOLD, 		EGFontWeightSemiBold },
		{ DWRITE_FONT_WEIGHT_BOLD,				EGFontWeightBold },
		{ DWRITE_FONT_WEIGHT_EXTRA_BOLD, 		EGFontWeightExtraBold },
		{ DWRITE_FONT_WEIGHT_BLACK, 			EGFontWeightBlack },
		{ DWRITE_FONT_WEIGHT_EXTRA_BLACK,		EGFontWeightExtraBlack },
		{ -1,									EGFontWeightAny }
	};

	EGFontWeight fontWeight = EGFontWeightNormal;
	FontWeightMap *ent = fontWeightMap;
	while (ent->dWeight != -1) {
		if (ent->dWeight == dWeight) {
			fontWeight = ent->fWeight;
			break;
		}
		ent++;
	}
	return fontWeight;
}

static EGFontSlant convertFontSlant(DWRITE_FONT_STYLE dSlant)
{
	struct FontSlantMap { EGenum dSlant; EGFontSlant fSlant; };

	static FontSlantMap fontSlantMap[] = {
		{ DWRITE_FONT_STYLE_NORMAL,				EGFontSlantNone },
		{ DWRITE_FONT_STYLE_OBLIQUE,			EGFontSlantOblique },
		{ DWRITE_FONT_STYLE_ITALIC,				EGFontSlantItalic },
		{ -1,									EGFontSlantAny }
	};

	EGFontSlant fontSlant = EGFontSlantNone;
	FontSlantMap *ent = fontSlantMap;
	while (ent->dSlant != -1) {
		if (ent->dSlant == dSlant) {
			fontSlant = ent->fSlant;
			break;
		}
		ent++;
	}
	return fontSlant;
}

static EGFontStretch convertFontStretch(DWRITE_FONT_STRETCH dStretch)
{
	struct FontStretchMap { EGenum dStretch; EGFontStretch fStretch; };

	static FontStretchMap fontStretchMap[] = {
		{ DWRITE_FONT_STRETCH_ULTRA_CONDENSED,	EGFontStretchUltraCondensed },
		{ DWRITE_FONT_STRETCH_EXTRA_CONDENSED,	EGFontStretchExtraCondensed },
		{ DWRITE_FONT_STRETCH_CONDENSED,		EGFontStretchCondensed },
		{ DWRITE_FONT_STRETCH_SEMI_CONDENSED,	EGFontStretchSemiCondensed },
		{ DWRITE_FONT_STRETCH_NORMAL,			EGFontStretchMedium },
		{ DWRITE_FONT_STRETCH_SEMI_EXPANDED,	EGFontStretchSemiExpanded },
		{ DWRITE_FONT_STRETCH_EXPANDED,			EGFontStretchExpanded },
		{ DWRITE_FONT_STRETCH_EXTRA_EXPANDED,	EGFontStretchExtraExpanded },
		{ DWRITE_FONT_STRETCH_ULTRA_EXPANDED,	EGFontStretchUltraExpanded },
		{ -1,									EGFontStretchAny }
	};

	EGFontStretch fontStretch = EGFontStretchMedium;
	FontStretchMap *ent = fontStretchMap;
	while (ent->dStretch != -1) {
		if (ent->dStretch == dStretch) {
			fontStretch = ent->fStretch;
			break;
		}
		ent++;
	}
	return fontStretch;
}

static EGstring convertDWriteLocalizedStringtoEGstring(wchar_t *localeName, IDWriteLocalizedStrings *localString)
{
	HRESULT hr;
	EGstring str;
	UINT32 index = 0, length = 0;
	BOOL exists = false;

	// find localized index
	hr = localString->FindLocaleName(localeName, &index, &exists);
	if (SUCCEEDED(hr) && !exists) // if no match retry with US English
	{
		hr = localString->FindLocaleName(L"en-us", &index, &exists);
		if (SUCCEEDED(hr) && !exists) index = 0; // default to first string
	}

	// convert string to utf-8
	hr = localString->GetStringLength(index, &length);
	if (SUCCEEDED(hr)) {
		wchar_t* bufw = new wchar_t[length+1];
		char* buf = new char[(length * 2) + 1];
		hr = localString->GetString(index, bufw, length+1);
		if (SUCCEEDED(hr)) {
			int bufLen = WideCharToMultiByte(CP_UTF8, 0, bufw, length, buf, length * 2, NULL, NULL);
			buf[bufLen] = '\0';
			str = buf;
			delete [] bufw;
			delete [] buf;
		}
	}

	return str;
}

void EGTextContextDWrite::initFonts()
{
	HRESULT hr;

	// Get locale
	wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
	hr = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
	if (FAILED(hr)) {
        EGError("%s:%s error getting locale name\n", class_name, __func__);
    }

	// Get list of installed fonts
	IDWriteFontCollection *fontCollection;
	hr = pDWriteFactory->GetSystemFontCollection(&fontCollection, FALSE);
	if (FAILED(hr)) {
        EGError("%s:%s error getting system font collection\n", class_name, __func__);
		return;
	}

	// Iterate through font familes
	int familyCount = fontCollection->GetFontFamilyCount();
	for (EGint familyIdx = 0; familyIdx < familyCount; familyIdx++)
	{
		// Get next font family
		IDWriteFontFamily *fontFamily;
		hr = fontCollection->GetFontFamily(familyIdx, &fontFamily);
		if (FAILED(hr)) {
			EGError("%s:%s GetFontFamily failed: IDWriteFontFamily %d\n", class_name, __func__, familyIdx);
			continue;
		}

		// Get localized font family name
		IDWriteLocalizedStrings *fontFamilyNames;
		hr = fontFamily->GetFamilyNames(&fontFamilyNames);
		if (FAILED(hr)) {
			EGError("%s:%s GetFamilyNames failed: IDWriteFontFamily %d\n", class_name, __func__, familyIdx);
			continue;
		}
		EGstring fontFamilyName = convertDWriteLocalizedStringtoEGstring(localeName, fontFamilyNames);

		// Iterate through font styles for this font family
		EGuint fontCount = fontFamily->GetFontCount();
		for (EGuint fontIdx = 0; fontIdx < fontCount; fontIdx++) {
			// Get font
			IDWriteFont *font;
			HRESULT hr = fontFamily->GetFont(fontIdx, &font);
			if (FAILED(hr)) {
				EGError("%s:%s GetFont failed: IDWriteFont %d, IDWriteFontFamily %d\n", class_name, __func__, fontIdx, familyIdx);
				continue;
			}

			// Get localized facename
			IDWriteLocalizedStrings *fontFaceNames;
			hr = font->GetFaceNames(&fontFaceNames);
			if (FAILED(hr)) {
				EGError("%s:%s GetFaceNames failed: IDWriteFont %d, IDWriteFontFamily %d\n", class_name, __func__, fontIdx, familyIdx);
				continue;
			}

			EGstring fontStyle = convertDWriteLocalizedStringtoEGstring(localeName, fontFaceNames);
			EGFontWeight fontWeight = convertFontWeight(font->GetWeight());
			EGFontSlant fontSlant = convertFontSlant(font->GetStyle());
			EGFontStretch fontStretch = convertFontStretch(font->GetStretch());
			EGstring fontName = fontFamilyName + (fontStyle.length() > 0 ? " " : "") + fontStyle;
			EGFontRec fontRec(fontFamilyName, fontName, fontWeight, fontSlant, fontStretch);
			EGFont::addFont(EGFontPtr(new EGFontDWrite(fontRec, font)));

			fontFaceNames->Release();
			font->Release();
		}
	}
	// Release font collection
	fontCollection->Release();
}


/* EGTextDWrite */

EGTextDWrite::EGTextDWrite() : pBitmap(NULL), pRT(NULL), pBlackBrush(NULL), pTextFormat(NULL)
{
    if (!ctx) {
        ctx = EGTextContextPtr(new EGTextContextDWrite());
    }
}

EGTextDWrite::~EGTextDWrite()
{
    deleteFontContext();
    deleteContext();
}

void EGTextDWrite::createContext(EGuint width, EGuint height)
{
    texWidth = width;
    texHeight = height;
    texData = calloc(texWidth * texHeight, 1);
    
    HRESULT hr;
    
    // Create IWIC bitmap
    hr = static_cast<EGTextContextDWrite*>(ctx.get())->pWICFactory->CreateBitmap(texWidth, texHeight, GUID_WICPixelFormat8bppAlpha, WICBitmapCacheOnDemand, &pBitmap);
    if (FAILED(hr)) {
        EGError("%s:%s unable to create IWICBitmap\n", class_name, __func__);
        return;
    }
    
    // Create Direct2D render target
    D2D1_RENDER_TARGET_PROPERTIES targetProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat( DXGI_FORMAT_A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED ) );
    hr = static_cast<EGTextContextDWrite*>(ctx.get())->pD2DFactory->CreateWicBitmapRenderTarget(pBitmap, &targetProps, &pRT);
    if (FAILED(hr)) {
        EGError("%s:%s unable to create ID2D1RenderTarget\n", class_name, __func__);
        return;
    }
    pRT->SetTransform(D2D1::Matrix3x2F::Scale(1.0, -1.0) * D2D1::Matrix3x2F::Translation(0, (EGfloat)texHeight));
    
    // Create a black brush.
    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBlackBrush);
    if (FAILED(hr)) {
        EGError("%s:%s unable to create black ID2D1SolidColorBrush\n", class_name, __func__);
        return;
    }
}

void EGTextDWrite::deleteContext()
{
    if (pBlackBrush) {
        pBlackBrush->Release();
        pBlackBrush = NULL;
    }
    if (pRT) {
        pRT->Release();
        pRT = NULL;
    };
    if (pBitmap) {
        pBitmap->Release();
        pBitmap = NULL;
    }
    if (texData) {
        free(texData);
        texData = 0;
        texWidth = texHeight = 0;
    }
    rendered = false;
}

void EGTextDWrite::createFontContext()
{
    if (pTextFormat) return;
	EGFontPtr font = EGFont::findFont(fontFace, (EGFontStyle)fontStyle);
    if (font) {
        EGFontDWrite *dwritefont = static_cast<EGFontDWrite*>(font.get());
		pTextFormat = dwritefont->getTextFormat(static_cast<EGTextContextDWrite*>(ctx.get())->pDWriteFactory, fontSize);
    }
}

void EGTextDWrite::deleteFontContext()
{
    pTextFormat = NULL;
    updated = rendered = sized = false;
}

void EGTextDWrite::getTextSize(EGuint &width, EGuint &height)
{
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = texWidth;
    rect.bottom = texHeight;
    
    IDWriteTextLayout *pTextLayout = NULL;
	int wstrLen;
    wchar_t *wstr = utf8ToWide(textStr.c_str(), &wstrLen);
    HRESULT hr = static_cast<EGTextContextDWrite*>(ctx.get())->pDWriteFactory->CreateTextLayout(wstr, wstrLen, pTextFormat, 2048, 2048, &pTextLayout);
    if (SUCCEEDED(hr)) {
        DWRITE_TEXT_METRICS metrics;
        pTextLayout->GetMetrics(&metrics);
        width = (EGuint)ceilf(metrics.width);
        height = (EGuint)ceilf(metrics.height);
    } else {
        width = height = 0;
        EGError("%s:%s unable to create IDWriteTextFormat\n", class_name, __func__);
    }
    delete [] wstr;
}

void EGTextDWrite::calcGlyphRects()
{
    if (glyphRects.size() > 0) {
        return;
    }
    
    // todo
}

void EGTextDWrite::renderText()
{
    if (rendered) return;
    
    if (texWidth < wp2 || texHeight < hp2) {
        deleteContext();
        createContext(wp2, hp2);
    }
    
    // begin drawing
    pRT->BeginDraw();
    
    // clear render target
    pRT->Clear(0);
    
    // draw text
	int wstrLen;
    wchar_t *wstr = utf8ToWide(textStr.c_str(), &wstrLen);
    EGfloat dx = flags & EGTextHAlignCenter ? (texWidth - w) * 0.5f : flags & EGTextHAlignRight ? texWidth - w : 0;
    EGfloat dy = flags & EGTextVAlignCenter ? (texHeight - fontSize) * 0.5f : flags & EGTextVAlignTop ? 0 : texHeight - fontSize;
    D2D1_RECT_F layoutRect = D2D1::RectF(dx, dy, (EGfloat)texWidth, (EGfloat)texHeight);
    pRT->DrawText(wstr, wstrLen, pTextFormat, layoutRect, pBlackBrush);
    delete [] wstr;
    
    // end drawing
    pRT->EndDraw();
    
    // copy pixels into texture buffer
    WICRect rect = { 0, 0, texWidth, texHeight };
    HRESULT hr = pBitmap->CopyPixels(&rect, texWidth, texWidth * texHeight, (BYTE*)texData);
    
    rendered = true;
}

EGbool EGTextDWrite::init()
{
    if (!ctx) {
        ctx = EGTextContextPtr(new EGTextContextDWrite());
    }
    return true;
}

#endif
