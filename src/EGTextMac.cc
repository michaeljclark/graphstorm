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
#include "EGTextMac.h"

/*
 * EGTextMac - Mac CoreText implementation
 */

#if __APPLE__

/* EGFontMac */

EGFontMac::EGFontMac(EGFontRec fontRec, CTFontDescriptorRef ctFontDesc) : EGFont(fontRec), ctFontDesc(ctFontDesc)
{
    CFRetain(ctFontDesc);
}

EGFontMac::~EGFontMac()
{
    CFRelease(ctFontDesc);
}

CTFontRef EGFontMac::getCTFont(EGfloat fontSize)
{
    return CTFontCreateWithFontDescriptor(ctFontDesc, fontSize, NULL);
}


/* EGTextContextMac */

EGTextContextMac::EGTextContextMac()
{
    initFonts();
}

EGTextContextMac::~EGTextContextMac()
{
}

static EGstring CFStringToEGstring(CFStringRef stringRef, EGbool release = true)
{
    if (!stringRef) return "";
    CFIndex length = CFStringGetLength(stringRef);
    CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
    char *cstring = new char[maxSize];
    CFStringGetCString(stringRef, cstring, maxSize, kCFStringEncodingUTF8);
    std::string str(cstring);
    delete [] cstring;
    if (release) {
        CFRelease(stringRef);
    }
    return str;
}

static CFComparisonResult fontNameSortCallback(CTFontDescriptorRef fontRefA, CTFontDescriptorRef fontRefB, void *refCon)
{
    CFStringRef fontNameA = (CFStringRef)CTFontDescriptorCopyAttribute(fontRefA, kCTFontDisplayNameAttribute);
    CFStringRef fontNameB = (CFStringRef)CTFontDescriptorCopyAttribute(fontRefB, kCTFontDisplayNameAttribute);
    CFComparisonResult result = CFStringCompare(fontNameA, fontNameB, kCFCompareCaseInsensitive);
    CFRelease(fontNameA);
    CFRelease(fontNameB);
    return result;
}

void EGTextContextMac::initFonts()
{
    CFBooleanRef value = kCFBooleanTrue;
    CFDictionaryRef dict = CFDictionaryCreate(NULL, (const void **)kCTFontCollectionRemoveDuplicatesOption, (const void **)&value, 1,
                                              &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CTFontCollectionRef collection = CTFontCollectionCreateFromAvailableFonts(dict);
    CFArrayRef fontDescriptorArray = CTFontCollectionCreateMatchingFontDescriptorsSortedWithCallback(collection, &fontNameSortCallback, NULL);
    CFIndex n = CFArrayGetCount(fontDescriptorArray);
    for(CFIndex i=0; i<n; i++) {
        CTFontDescriptorRef fontRef = (CTFontDescriptorRef)CFArrayGetValueAtIndex(fontDescriptorArray, i);
        EGstring psName = CFStringToEGstring((CFStringRef)CTFontDescriptorCopyAttribute(fontRef, kCTFontNameAttribute));
        EGstring fontName = CFStringToEGstring((CFStringRef)CTFontDescriptorCopyAttribute(fontRef, kCTFontDisplayNameAttribute));
        EGstring fontFamily = CFStringToEGstring((CFStringRef)CTFontDescriptorCopyAttribute(fontRef, kCTFontFamilyNameAttribute));
        EGstring fontStyle = CFStringToEGstring((CFStringRef)CTFontDescriptorCopyAttribute(fontRef, kCTFontStyleNameAttribute));
        EGFontRec fontRec = EGFont::createFontRecord(psName, fontFamily, fontStyle, fontName);
        EGFontPtr font(new EGFontMac(fontRec, fontRef));
        EGFont::addFont(font);
    }
    CFRelease(fontDescriptorArray);
    CFRelease(collection);
    CFRelease(dict);
}


/* EGTextMac */

EGTextMac::EGTextMac() : cgCtx(0), ctFont(0), atAttributes(0)
{
    if (!ctx) {
        ctx = EGTextContextPtr(new EGTextContextMac());
    }
}

EGTextMac::~EGTextMac()
{
    deleteFontContext();
    deleteContext();
}

void EGTextMac::createContext(EGuint width, EGuint height)
{
    texWidth = width;
    texHeight = height;
    texData = malloc(texWidth * texHeight);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    cgCtx = CGBitmapContextCreate(texData, texWidth, texHeight, 8, texWidth, colorSpace, kCGImageAlphaNone);
    CGContextSetGrayFillColor(cgCtx, 1.0, 1.0);
    CFRelease(colorSpace);
    
    CGContextSetTextMatrix(cgCtx, CGAffineTransformIdentity);
    CGContextTranslateCTM(cgCtx, 0, texHeight);
    CGContextScaleCTM(cgCtx, 1.0, -1.0);
}

void EGTextMac::deleteContext()
{
    if (cgCtx) {
        CGContextRelease(cgCtx);
        texWidth = texHeight = 0;
        cgCtx = 0;
    }
    if (texData) {
        free(texData);
        texData = 0;
    }
    rendered = false;
}

void EGTextMac::createFontContext()
{
    if (ctFont) return;
    
    EGFontPtr font = EGFont::findFont(fontFace, (EGFontStyle)fontStyle);
    if (font) {
        EGFontMac *macfont = static_cast<EGFontMac*>(font.get());
        ctFont = macfont->getCTFont(fontSize);
    } else {
        ctFont = CTFontCreateUIFontForLanguage(kCTFontSystemFontType, fontSize, CFSTR("en"));
    }
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGFloat whiteComps[] = { 1, 1, 1, 1 };
    CGColorRef whiteColor = CGColorCreate(colorSpace, whiteComps);
    const void* keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
    const void* values[] = { ctFont, whiteColor };
    atAttributes = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 2, NULL, NULL);
    CFRelease(colorSpace);
}

void EGTextMac::deleteFontContext()
{
    if (ctFont) {
        ctFont = 0;
    }
    if (atAttributes) {
        CFRelease(atAttributes);
        atAttributes = 0;
    }
    glyphRects.clear();
    updated = rendered = sized = false;
}

void EGTextMac::getTextSize(EGuint &width, EGuint &height)
{
    glyphRects.clear();
    
    CFStringRef mystring = CFStringCreateWithCString(kCFAllocatorDefault, textStr.c_str(), kCFStringEncodingUTF8);
    CFAttributedStringRef myattrstring = CFAttributedStringCreate(kCFAllocatorDefault, mystring, atAttributes);
    CTLineRef ctLine = CTLineCreateWithAttributedString(myattrstring);
    
    ctwidth =  CTLineGetTypographicBounds(ctLine, &ctascent, &ctdescent, &ctleading);
    width = (EGint)ceilf(ctwidth);
    if (maxWidth > 0) width = (std::min)(width, maxWidth);
    //height = (EGint)ceilf(ctascent + ctdescent);
    height = fontSize + fontSize / 3;

    CFRelease(ctLine);
    CFRelease(myattrstring);
    CFRelease(mystring);
}

void EGTextMac::calcGlyphRects()
{
    if (glyphRects.size() > 0) {
        return;
    }
    
    CFStringRef mystring = CFStringCreateWithCString(kCFAllocatorDefault, textStr.c_str(), kCFStringEncodingUTF8);
    CFIndex count = CFStringGetLength(mystring);
    UniChar *chars = new UniChar[count];
    CGGlyph *glyphs = new CGGlyph[count];
    CGSize *advances = new CGSize[count];
    CGPoint *positions = new CGPoint[count];
    CFStringGetCharacters (mystring, CFRangeMake(0, count), chars);
    CTFontGetGlyphsForCharacters(ctFont, chars, glyphs, count);
    CTFontGetAdvancesForGlyphs(ctFont, kCTFontHorizontalOrientation, glyphs, advances, count);

    EGfloat dx = (flags & EGTextHAlignCenter) ? - (EGfloat)w * 0.5f : flags & EGTextHAlignRight ? -(EGfloat)w : 0;
    EGfloat dy = (flags & EGTextVAlignCenter) ? - (EGfloat)h * 0.5f : flags & EGTextVAlignTop ? 0 : -(EGfloat)h;
    for (CFIndex i = 0; i < count; i++) {
        EGint cx = (EGint)floorf(dx);
        EGint cy = (EGint)floorf(dy);
        EGint cw = (EGint)ceilf(advances[i].width);
        EGint ch = (EGint)h;
        if (maxWidth > 0 && cx + cw > maxWidth) break;
        glyphRects.push_back(EGRect(cx, cy, cw, ch));
        dx += advances[i].width;
    }
    
    delete [] positions;
    delete [] chars;
    delete [] glyphs;
    delete [] advances;
    CFRelease(mystring);
}

void EGTextMac::renderText()
{
    if (rendered) return;
    
    if (texWidth < wp2 || texHeight < hp2) {
        deleteContext();
        createContext(wp2, hp2);
    }
    
    CFStringRef mystring = CFStringCreateWithCString(kCFAllocatorDefault, textStr.c_str(), kCFStringEncodingUTF8);
#if 0
    CFAttributedStringRef myattrstring = CFAttributedStringCreate(kCFAllocatorDefault, mystring, atAttributes);
    CTLineRef ctLine = CTLineCreateWithAttributedString(myattrstring);
    CGContextClearRect(cgCtx, CGRectMake(0, 0, texWidth, texHeight));
    CGContextSetTextDrawingMode(cgCtx, kCGTextFill);
    EGfloat dx = flags & EGTextHAlignCenter ? (texWidth - w) * 0.5f : flags & EGTextHAlignRight ? texWidth - w : 0;
    EGfloat dy = flags & EGTextVAlignCenter ? (texHeight - fontSize + ctdescent) * 0.5f : flags & EGTextVAlignTop ? texHeight - fontSize + ctdescent : ctdescent;
    CGContextSetTextPosition(cgCtx, dx, dy);
    CTLineDraw(ctLine, cgCtx);
    CGContextFlush(cgCtx);
    CFRelease(ctLine);
    CFRelease(myattrstring);
#else
    CFIndex count = CFStringGetLength(mystring);
    UniChar *chars = new UniChar[count];
    CGGlyph *glyphs = new CGGlyph[count];
    CGSize *advances = new CGSize[count];
    CGPoint *positions = new CGPoint[count];
    CFStringGetCharacters (mystring, CFRangeMake(0, count), chars);
    CTFontGetGlyphsForCharacters(ctFont, chars, glyphs, count);
    CTFontGetAdvancesForGlyphs(ctFont, kCTFontHorizontalOrientation, glyphs, advances, count);
    EGfloat dx = flags & EGTextHAlignCenter ? (texWidth - w) * 0.5f : flags & EGTextHAlignRight ? texWidth - w : 0;
    EGfloat dy = flags & EGTextVAlignCenter ? (texHeight - fontSize + ctdescent) * 0.5f : flags & EGTextVAlignTop ? texHeight - fontSize + ctdescent : ctdescent;
    EGfloat x = dx, y = dy;
    EGfloat totalWidth = 0;
    for (CFIndex i = 0; i < count; i++) {
        if (maxWidth > 0 && totalWidth + advances[i].width > maxWidth) {
            count = i;
            break;
        }
        positions[i].x = x;
        positions[i].y = y;
        x += advances[i].width;
        totalWidth += advances[i].width;
    }
    CGContextClearRect(cgCtx, CGRectMake(0, 0, texWidth, texHeight));
    CGContextSetTextDrawingMode(cgCtx, kCGTextFill);
    CTFontDrawGlyphs(ctFont, glyphs, positions, count, cgCtx);
    CGContextFlush(cgCtx);
    delete [] positions;
    delete [] chars;
    delete [] glyphs;
    delete [] advances;
#endif
    CFRelease(mystring);
    
    rendered = true;
}

EGbool EGTextMac::init()
{
    if (!ctx) {
        ctx = EGTextContextPtr(new EGTextContextMac());
    }
    return true;
}

#endif
