/*
 *  EGFont.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGFont_H
#define EGFont_H

class EGFont;
typedef std::shared_ptr<EGFont> EGFontPtr;
typedef std::vector<EGFontPtr> EGFontList;
struct EGFontRec;
struct EGFontSpec;
typedef std::map<EGFontRec,EGFontPtr> EGFontRecMap;
typedef std::map<EGFontSpec,EGFontPtr> EGFontSpecMap;
typedef std::map<EGstring,EGFontPtr> EGFontNameMap;
typedef std::map<EGstring,EGFontList> EGFontFamilyMap;


extern const std::string EGFontFamilyAny;
extern const std::string EGFontNameAny;

/* EGFontStyle */

enum EGFontStyle {
    EGFontStyleThin             = 0,
    EGFontStyleThinItalic       = 1,
    EGFontStyleExtraLight       = 2,
    EGFontStyleExtraLightItalic = 3,
    EGFontStyleLight            = 4,
    EGFontStyleLightItalic      = 5,
    EGFontStyleNormal           = 6,
    EGFontStyleNormalItalic     = 7,
    EGFontStyleItalic           = EGFontStyleNormalItalic,
    EGFontStyleMedium           = 8,
    EGFontStyleMediumItalic     = 9,
    EGFontStyleSemiBold         = 10,
    EGFontStyleSemiBoldItalic   = 11,
    EGFontStyleBold             = 12,
    EGFontStyleBoldItalic       = 13,
    EGFontStyleExtraBold        = 14,
    EGFontStyleExtraBoldItalic  = 15,
    EGFontStyleBlack            = 16,
    EGFontStyleBlackItalic      = 17,
    EGFontStyleExtraBlack       = 18,
    EGFontStyleExtraBlackItalic = 19,
    EGFontStyleCount = 20,
};


/* EGFontCacheKey */

struct EGFontCacheKey
{
    EGstring fontFace;
    EGfloat fontSize;
    EGenum fontStyle;
};

namespace std {
    template <> struct equal_to <EGFontCacheKey> {
        inline bool operator()(const EGFontCacheKey &f1, const EGFontCacheKey &f2) const {
            return (f1.fontFace == f2.fontFace && f1.fontSize == f2.fontSize && f1.fontStyle == f2.fontStyle);
        }
    };
    template <> struct less <EGFontCacheKey> {
        inline bool operator()(const EGFontCacheKey &f1, const EGFontCacheKey &f2) const {
            if (f1.fontFace < f2.fontFace) return true;
            if (f1.fontFace == f2.fontFace) {
                if (f1.fontSize < f2.fontSize) return true;
                if (f1.fontSize == f2.fontSize) {
                    if (f1.fontStyle < f2.fontStyle) return true;
                }
            }
            return false;
        }
    };
}


/* EGFontWeight */

enum EGFontWeight {
    EGFontWeightAny,
    EGFontWeightThin,
    EGFontWeightExtraLight,
    EGFontWeightUltraLight,
    EGFontWeightLight,
    EGFontWeightSemiLight,
    EGFontWeightBook,
    EGFontWeightNormal,
    EGFontWeightRegular,
    EGFontWeightMedium,
    EGFontWeightDemiBold,
    EGFontWeightSemiBold,
    EGFontWeightBold,
    EGFontWeightExtraBold,
    EGFontWeightUltraBold,
    EGFontWeightBlack,
    EGFontWeightHeavy,
    EGFontWeightExtraBlack,
    EGFontWeightUltraBlack,
    EGFontWeightCount,
};


/* EGFontSlant */

enum EGFontSlant {
    EGFontSlantAny,
    EGFontSlantNone,
    EGFontSlantOblique,
    EGFontSlantItalic,
    EGFontSlantCount,
};


/* EGFontStretch */

enum EGFontStretch {
    EGFontStretchAny,
    EGFontStretchUltraCondensed,
    EGFontStretchExtraCondensed,
    EGFontStretchCondensed,
    EGFontStretchSemiCondensed,
    EGFontStretchMedium,
    EGFontStretchSemiExpanded,
    EGFontStretchExpanded,
    EGFontStretchExtraExpanded,
    EGFontStretchUltraExpanded,
    EGFontStretchCount,
};


/* EGFontSpacing */

enum EGFontSpacing {
    EGFontSpacingAny,
    EGFontSpacingNormal,
    EGFontSpacingMonospaced,
    EGFontSpacingCount,
};


/* EGFontTokenType */

enum EGFontTokenType {
    EGFontTokenNone,
    EGFontTokenWeight,
    EGFontTokenSlant,
    EGFontTokenStretch,
    EGFontTokenSpacing,
};


/* EGFontTokenEntry */

struct EGFontTokenEntry {
    const char* name;
    EGFontTokenType tokenType;
    EGenum tokenEnum;
    EGbool leftOfHyphen;
    EGbool eatToken;
    EGbool caseSensitive;
};


/* EGFontSpec */

struct EGFontSpec
{
    EGstring fontFamily;
    EGstring fontName;
    EGint fontWeight;
    EGint fontSlant;
    EGint fontStretch;
    EGint fontSpacing;
    
    EGFontSpec(EGstring fontFamily,
               EGstring fontName,
               EGint fontWeight = 400,
               EGint fontSlant = 0,
               EGint fontStretch = 5,
               EGint fontSpacing = 0)
        : fontFamily(fontFamily), fontName(fontName), fontWeight(fontWeight), fontSlant(fontSlant), fontStretch(fontStretch), fontSpacing(fontSpacing) {}

    EGstring toString() const;
};

namespace std {
    template <> struct equal_to <EGFontSpec> {
        inline bool operator()(const EGFontSpec &f1, const EGFontSpec &f2) const {
            return ((f1.fontFamily == f2.fontFamily || f1.fontFamily == "*" || f2.fontFamily == "*") &&
                    (f1.fontName == f2.fontName || f1.fontName == "*" || f2.fontName == "*") &&
                    (f1.fontWeight == f2.fontWeight || f1.fontWeight == -1 || f2.fontWeight == -1) &&
                    (f1.fontSlant == f2.fontSlant || f1.fontSlant == -1 || f2.fontSlant == -1) &&
                    (f1.fontStretch == f2.fontStretch || f1.fontStretch == -1 || f2.fontStretch == -1) &&
                    (f1.fontSpacing == f2.fontSpacing || f1.fontSpacing == -1 || f2.fontSpacing == -1));
        }
    };
    template <> struct less <EGFontSpec> {
        inline bool operator()(const EGFontSpec &f1, const EGFontSpec &f2) const {
            if (f1.fontFamily < f2.fontFamily && f1.fontFamily != EGFontFamilyAny && f2.fontFamily != EGFontFamilyAny) return true;
            if (f1.fontFamily == f2.fontFamily || f1.fontFamily == EGFontFamilyAny || f2.fontFamily == EGFontFamilyAny) {
                if (f1.fontName < f2.fontName && f1.fontName != EGFontNameAny && f2.fontName != EGFontNameAny) return true;
                if (f1.fontName == f2.fontName || f1.fontName == EGFontNameAny || f2.fontName == EGFontNameAny) {
                    if (f1.fontWeight < f2.fontWeight && f1.fontWeight != -1 && f2.fontWeight != -1) return true;
                    if (f1.fontWeight == f2.fontWeight || f1.fontWeight == -1 || f2.fontWeight == -1) {
                        if (f1.fontSlant < f2.fontSlant && f1.fontSlant != -1 && f2.fontSlant != -1) return true;
                        if (f1.fontSlant == f2.fontSlant|| f1.fontSlant == -1 || f2.fontSlant == -1) {
                            if (f1.fontStretch < f2.fontStretch && f1.fontStretch != -1 && f2.fontStretch != -1) return true;
                            if (f1.fontStretch == f2.fontStretch || f1.fontStretch == -1 || f2.fontStretch == -1) {
                                if (f1.fontSpacing < f2.fontSpacing && f1.fontSpacing != -1 && f2.fontSpacing != -1) return true;
                            }
                        }
                    }
                }
            }
            return false;
        }
    };
}


/* EGFontRec */

struct EGFontRec
{
    EGstring fontFamily;
    EGstring fontName;
    EGFontWeight fontWeight;
    EGFontSlant fontSlant;
    EGFontStretch fontStretch;
    EGFontSpacing fontSpacing;
    
    EGFontRec(EGstring fontFamily,
              EGstring fontName,
              EGFontWeight fontWeight = EGFontWeightNormal,
              EGFontSlant fontSlant = EGFontSlantNone,
              EGFontStretch fontStretch = EGFontStretchMedium,
              EGFontSpacing fontSpacing = EGFontSpacingNormal)
    : fontFamily(fontFamily), fontName(fontName), fontWeight(fontWeight), fontSlant(fontSlant), fontStretch(fontStretch), fontSpacing(fontSpacing) {}
    
    EGFontSpec fontSpec() const;
    
    EGstring toString() const;
};

namespace std {
    template <> struct equal_to <EGFontRec> {
        inline bool operator()(const EGFontRec &f1, const EGFontRec &f2) const {
            return ((f1.fontFamily == f2.fontFamily || f1.fontFamily == EGFontFamilyAny || f2.fontFamily == EGFontFamilyAny) &&
                    (f1.fontName == f2.fontName || f1.fontName == EGFontNameAny || f2.fontName == EGFontNameAny) &&
                    (f1.fontWeight == f2.fontWeight || f1.fontWeight == EGFontWeightAny || f2.fontWeight == EGFontWeightAny) &&
                    (f1.fontSlant == f2.fontSlant || f1.fontSlant == EGFontSlantAny || f2.fontSlant == EGFontSlantAny) &&
                    (f1.fontStretch == f2.fontStretch || f1.fontStretch == EGFontStretchAny || f2.fontStretch == EGFontStretchAny) &&
                    (f1.fontSpacing == f2.fontSpacing || f1.fontSpacing == EGFontSpacingAny || f2.fontSpacing == EGFontSpacingAny));
        }
    };
    template <> struct less <EGFontRec> {
        inline bool operator()(const EGFontRec &f1, const EGFontRec &f2) const {
            if (f1.fontFamily < f2.fontFamily && f1.fontFamily != EGFontFamilyAny && f2.fontFamily != EGFontFamilyAny) return true;
            if (f1.fontFamily == f2.fontFamily || f1.fontFamily == EGFontFamilyAny || f2.fontFamily == EGFontFamilyAny) {
                if (f1.fontName < f2.fontName && f1.fontName != EGFontNameAny && f2.fontName != EGFontNameAny) return true;
                if (f1.fontName == f2.fontName || f1.fontName == EGFontNameAny || f2.fontName == EGFontNameAny) {
                    if (f1.fontWeight < f2.fontWeight && f1.fontWeight != EGFontWeightAny && f2.fontWeight != EGFontWeightAny) return true;
                    if (f1.fontWeight == f2.fontWeight || f1.fontWeight == EGFontWeightAny || f2.fontWeight == EGFontWeightAny) {
                        if (f1.fontSlant < f2.fontSlant && f1.fontSlant != EGFontSlantAny && f2.fontSlant != EGFontSlantAny) return true;
                        if (f1.fontSlant == f2.fontSlant || f1.fontSlant == EGFontSlantAny || f2.fontSlant == EGFontSlantAny) {
                            if (f1.fontStretch < f2.fontStretch && f1.fontStretch != EGFontStretchAny && f2.fontStretch != EGFontStretchAny) return true;
                            if (f1.fontStretch == f2.fontStretch || f1.fontStretch == EGFontStretchAny || f2.fontStretch == EGFontStretchAny) {
                                if (f1.fontSpacing < f2.fontSpacing && f1.fontSpacing != EGFontSpacingAny && f2.fontSpacing != EGFontSpacingAny) return true;
                            }
                        }
                    }
                }
            }
            return false;
        }
    };
}


/* EGFont */

class EGFont
{
protected:
    static EGFontList fontList;
    static EGFontRecMap fontRecMap;
    static EGFontSpecMap fontSpecMap;
    static EGFontNameMap fontNameMap;
    static EGFontFamilyMap fontFamilyMap;
    
    EGFontRec fontRec;
    
public:
    static EGbool debug;
    
    static const EGint weightTable[];
    static const EGint slantTable[];
    static const EGint stretchTable[];
    static const EGfloat stretchPercentTable[];
    static const EGint spacingTable[];

    static const char* weightName[];
    static const char* slantName[];
    static const char* stretchName[];
    static const char* spacingName[];
    
    static const EGFontTokenEntry fontTokens[];
    static const EGFontRec styleMapping[];
    
    static EGstring synthesizeFontName(EGstring familyName, EGFontWeight fontWeight, EGFontSlant fontSlant, EGFontStretch fontStretch);
    static EGFontRec createFontRecord(EGstring psName, EGstring familyName, EGstring styleName = "", EGstring fontName = "");
    static void addFont(EGFontPtr font);
    static EGFontPtr findFont(EGFontRec fontRec);
    static EGFontPtr findFont(EGFontSpec fontSpec);
    static EGFontPtr findFont(EGstring fontName);
    static EGFontPtr findFont(EGstring fontFamily, EGFontStyle fontStyle);
    static const EGFontList& getFontList();
    
    EGFont(EGFontRec fontRec);
    virtual ~EGFont();
    
    const EGFontRec& getFontRec();
};

#endif
