/*
 *  EGFont.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGFont.h"

/* EGFont */

static const char class_name[] = "EGFont";

const std::string EGFontFamilyAny = "*";
const std::string EGFontNameAny = "*";

const EGint EGFont::weightTable[] = {
    -1,     // EGFontWeightAny,
    100,    // EGFontWeightThin,
    200,    // EGFontWeightExtraLight,
    200,    // EGFontWeightUltraLight,
    300,    // EGFontWeightLight,
    350,    // EGFontWeightSemiLight,
    350,    // EGFontWeightBook,
    400,    // EGFontWeightNormal,
    400,    // EGFontWeightRegular,
    500,    // EGFontWeightMedium,
    600,    // EGFontWeightDemiBold,
    600,    // EGFontWeightSemiBold,
    700,    // EGFontWeightBold,
    800,    // EGFontWeightExtraBold,
    800,    // EGFontWeightUltraBold,
    900,    // EGFontWeightBlack,
    900,    // EGFontWeightHeavy,
    950,    // EGFontWeightExtraBlack,
    950,    // EGFontWeightUltraBlack,
};

const EGint EGFont::slantTable[] = {
    -1,     // EGFontSlantAny,
    0,      // EGFontSlantNone,
    1,      // EGFontSlantOblique,
    1,      // EGFontSlantItalic,
};

const EGint EGFont::stretchTable[] = {
    -1,     // EGFontStretchAny,
    1,      // EGFontStretchUltraCondensed,
    2,      // EGFontStretchExtraCondensed,
    3,      // EGFontStretchCondensed,
    4,      // EGFontStretchSemiCondensed,
    5,      // EGFontStretchMedium,
    6,      // EGFontStretchSemiExpanded,
    7,      // EGFontStretchExpanded,
    8,      // EGFontStretchExtraExpanded,
    9,      // EGFontStretchUltraExpanded,
};

const EGfloat EGFont::stretchPercentTable[] = {
    -1,     // EGFontStretchAny,
    50,     // EGFontStretchUltraCondensed,
    62.5f,  // EGFontStretchExtraCondensed,
    75,     // EGFontStretchCondensed,
    87.5f,  // EGFontStretchSemiCondensed,
    100,    // EGFontStretchMedium,
    112.5f, // EGFontStretchSemiExpanded,
    125,    // EGFontStretchExpanded,
    150,    // EGFontStretchExtraExpanded,
    200,    // EGFontStretchUltraExpanded,
};

const EGint EGFont::spacingTable[] = {
    -1,     // EGFontSpacingAny,
    0,      // EGFontSpacingNormal,
    1,      // EGFontSpacingMonospaced,
};

const char* EGFont::weightName[] = {
    "Any",
    "Thin",
    "ExtraLight",
    "UltraLight",
    "Light",
    "SemiLight",
    "Book",
    "Normal",
    "Regular",
    "Medium",
    "DemiBold",
    "SemiBold",
    "Bold",
    "ExtraBold",
    "UltraBold",
    "Black",
    "Heavy",
    "ExtraBlack",
    "UltraBlack",
    NULL
};

const char* EGFont::slantName[] = {
    "Any",
    "None",
    "Oblique",
    "Italic",
    NULL
};

const char* EGFont::stretchName[] = {
    "Any",
    "UltraCondensed",
    "ExtraCondensed",
    "Condensed",
    "SemiCondensed",
    "Medium",
    "SemiExpanded",
    "Expanded",
    "ExtraExpanded",
    "UltraExpanded",
    NULL
};

const char* EGFont::spacingName[] = {
    "Any",
    "Normal",
    "Monospaced",
    NULL
};

const EGFontTokenEntry EGFont::fontTokens[] = {
    { "thin",           EGFontTokenWeight,  EGFontWeightThin,               true, true, false },
    { "extralight",     EGFontTokenWeight,  EGFontWeightExtraLight,         true, true, false },
    { "ultralight",     EGFontTokenWeight,  EGFontWeightUltraLight,         true, true, false },
    { "light",          EGFontTokenWeight,  EGFontWeightLight,              true, true, false },
    { "semilight",      EGFontTokenWeight,  EGFontWeightSemiLight,          true, true, false },
    { "book",           EGFontTokenWeight,  EGFontWeightBook,               true, true, false },
    { "normal",         EGFontTokenWeight,  EGFontWeightNormal,             true, true, false },
    { "regular",        EGFontTokenWeight,  EGFontWeightRegular,            true, true, false },
    { "plain",          EGFontTokenWeight,  EGFontWeightRegular,            true, true, false },
    { "roman",          EGFontTokenWeight,  EGFontWeightRegular,            true, true, false },
    { "medium",         EGFontTokenWeight,  EGFontWeightMedium,             true, true, false },
    { "Med",            EGFontTokenWeight,  EGFontWeightMedium,             true, true, true },
    { "demibold",       EGFontTokenWeight,  EGFontWeightDemiBold,           true, true, false },
    { "semibold",       EGFontTokenWeight,  EGFontWeightSemiBold,           true, true, false },
    { "extrabold",      EGFontTokenWeight,  EGFontWeightExtraBold,          true, true, false },
    { "ultrabold",      EGFontTokenWeight,  EGFontWeightUltraBold,          true, true, false },
    { "bold",           EGFontTokenWeight,  EGFontWeightBold,               true, true, false },
    { "heavy",          EGFontTokenWeight,  EGFontWeightHeavy,              true, true, false },
    { "extrablack",     EGFontTokenWeight,  EGFontWeightExtraBlack,         true, true, false },
    { "ultrablack",     EGFontTokenWeight,  EGFontWeightUltraBlack,         true, true, false },
    { "black",          EGFontTokenWeight,  EGFontWeightBlack,              true, true, false },
    { "oblique",        EGFontTokenSlant,   EGFontSlantOblique,             true, true, false },
    { "inclined",       EGFontTokenSlant,   EGFontSlantOblique,             true, true, false },
    { "Ob",             EGFontTokenSlant,   EGFontSlantOblique,             false, true, true },
    { "italic",         EGFontTokenSlant,   EGFontSlantItalic,              true, true, false },
    { "It",             EGFontTokenSlant,   EGFontSlantItalic,              true, true, true },
    { "ultracondensed", EGFontTokenStretch, EGFontStretchUltraCondensed,    true, true, false },
    { "extracondensed", EGFontTokenStretch, EGFontStretchExtraCondensed,    true, true, false },
    { "semicondensed",  EGFontTokenStretch, EGFontStretchSemiCondensed,     true, true, false },
    { "condensed",      EGFontTokenStretch, EGFontStretchCondensed,         true, true, false },
    { "Cond",           EGFontTokenStretch, EGFontStretchCondensed,         true, true, true },
    { "semiexpanded",   EGFontTokenStretch, EGFontStretchSemiExpanded,      true, true, false },
    { "extraexpanded",  EGFontTokenStretch, EGFontStretchExtraExpanded,     true, true, false },
    { "ultraexpanded",  EGFontTokenStretch, EGFontStretchUltraExpanded,     true, true, false },
    { "expanded",       EGFontTokenStretch, EGFontStretchExpanded,          true, true, false },
    { "extended",       EGFontTokenStretch, EGFontStretchExpanded,          true, true, false },
    { "monospaced",     EGFontTokenSpacing, EGFontSpacingMonospaced,        true, true, false },
    { "mono",           EGFontTokenSpacing, EGFontSpacingMonospaced,        true, true, false },
    { NULL,             EGFontTokenNone,    0,                              false, false, false },
};

const EGFontRec EGFont::styleMapping[] = {
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightThin,        EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightThin,        EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightExtraLight,  EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightExtraLight,  EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightLight,       EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightLight,       EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightRegular,     EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightRegular,     EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightMedium,      EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightMedium,      EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightSemiBold,    EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightSemiBold,    EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightBold,        EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightBold,        EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightExtraBold,   EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightExtraBold,   EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightBlack,       EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightBlack,       EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightExtraBlack,  EGFontSlantNone,    EGFontStretchAny, EGFontSpacingAny),
    EGFontRec(EGFontFamilyAny,  EGFontNameAny,  EGFontWeightExtraBlack,  EGFontSlantItalic,  EGFontStretchAny, EGFontSpacingAny),
};

EGFontSpec EGFontRec::fontSpec() const
{
    return EGFontSpec(fontFamily, fontName, EGFont::weightTable[fontWeight], EGFont::slantTable[fontSlant], EGFont::stretchTable[fontStretch], EGFont::spacingTable[fontSpacing]);
}

EGstring EGFontRec::toString() const
{
    std::stringstream ss;
    ss << "[EGFontRec"
       << " family=" << std::setw (28) << std::setiosflags(std::ios_base::left) << fontFamily
       << " name=" << std::setw (36) << std::setiosflags(std::ios_base::left) << fontName
       << EGFont::weightTable[fontWeight];
    if (fontStretch != EGFontStretchMedium) ss << " " << EGFont::stretchName[fontStretch];
    ss << " " << EGFont::weightName[fontWeight];
    if (fontSlant != EGFontSlantNone) ss << " " << EGFont::slantName[fontSlant];
    if (fontSpacing != EGFontSpacingNormal) ss << " " << EGFont::spacingName[fontSpacing];
    ss << "]";
    return ss.str();
}

EGstring EGFontSpec::toString() const
{
    std::stringstream ss;
    ss << "[EGFontSpec"
       << " family=" << std::setw (28) << std::setiosflags(std::ios_base::left) << fontFamily
       << " name=" << std::setw (36) << std::setiosflags(std::ios_base::left) << fontName
       << " wt=" << fontWeight
       << " sl=" << fontSlant
       << " st=" << fontStretch
       << " sp=" << fontSpacing
       << "]";
    return ss.str();
}
EGbool EGFont::debug = false;
EGFontList EGFont::fontList;
EGFontRecMap EGFont::fontRecMap;
EGFontSpecMap EGFont::fontSpecMap;
EGFontNameMap EGFont::fontNameMap;
EGFontFamilyMap EGFont::fontFamilyMap;

EGFont::EGFont(EGFontRec fontRec) : fontRec(fontRec) {}

EGFont::~EGFont() {}

EGstring EGFont::synthesizeFontName(EGstring familyName, EGFontWeight fontWeight, EGFontSlant fontSlant, EGFontStretch fontStretch)
{
    std::stringstream ss;
    ss << familyName;
    if (fontStretch != EGFontStretchMedium) ss << " " << EGFont::stretchName[fontStretch];
    ss << " " << EGFont::weightName[fontWeight];
    if (fontSlant != EGFontSlantNone) ss << " " << EGFont::slantName[fontSlant];
    return ss.str();
}

EGFontRec EGFont::createFontRecord(EGstring psName, EGstring familyName, EGstring styleName, EGstring fontName)
{
    EGFontStretch fontStretch = EGFontStretchMedium;
    EGFontWeight fontWeight = EGFontWeightRegular;
    EGFontSlant fontSlant = EGFontSlantNone;
    EGFontSpacing fontSpacing = EGFontSpacingNormal;
    EGstring fontNameTemp = psName;
    EGint tokensProcessed;
    EGbool foundHyphen = false;
    
    size_t offset = fontNameTemp.length(), tokenLen;
    do {
        tokensProcessed = 0;
        if (offset > 1 && fontNameTemp.c_str()[offset - 1] == '-') {
            foundHyphen = true;
            fontNameTemp.erase(offset - 1, 1);
            offset--;
            tokensProcessed++;
        }
        const EGFontTokenEntry *token = fontTokens;
        while (token->name != NULL) {
            if ((token->leftOfHyphen || !foundHyphen) && offset > (tokenLen = strlen(token->name)) &&
                ((!token->caseSensitive && strncasecmp(fontNameTemp.c_str() + offset - tokenLen, token->name, tokenLen) == 0) ||
                 (token->caseSensitive && strncmp(fontNameTemp.c_str() + offset - tokenLen, token->name, tokenLen) == 0)) )
            {
                switch (token->tokenType) {
                    case EGFontTokenWeight:
                        fontWeight = (EGFontWeight)token->tokenEnum;
                        break;
                    case EGFontTokenSlant:
                        fontSlant = (EGFontSlant)token->tokenEnum;
                        break;
                    case EGFontTokenStretch:
                        fontStretch = (EGFontStretch)token->tokenEnum;
                        break;
                    case EGFontTokenSpacing:
                        fontSpacing = (EGFontSpacing)token->tokenEnum;
                        break;
                    default:
                        break;
                }
                if (token->eatToken) {
                    fontNameTemp.erase(offset - tokenLen, tokenLen);
                }
                offset -= tokenLen;
                tokensProcessed++;
            }
            token++;
        }
    } while (tokensProcessed > 0);
    
    if (fontName.length() == 0) {
        if (styleName.length() == 0) {
            fontName = synthesizeFontName(familyName, fontWeight, fontSlant, fontStretch);
        } else {
            fontName = familyName + EGstring(" ") + styleName;
        }
    }
    
    return EGFontRec(familyName, fontName, fontWeight, fontSlant, fontStretch, fontSpacing);
}

void EGFont::addFont(EGFontPtr font)
{
    EGFontRecMap::iterator fri = fontRecMap.find(font->fontRec);
    if (fri == fontRecMap.end()) {
        fontList.push_back(font);
        fontRecMap.insert(std::pair<EGFontRec,EGFontPtr>(font->fontRec, font));
        fontSpecMap.insert(std::pair<EGFontSpec,EGFontPtr>(font->fontRec.fontSpec(), font));
        fontNameMap.insert(std::pair<EGstring,EGFontPtr>(font->fontRec.fontName, font));
        EGFontFamilyMap::iterator ffi = fontFamilyMap.find(font->fontRec.fontFamily);
        if (ffi == fontFamilyMap.end()) {
            ffi = fontFamilyMap.insert(std::pair<EGstring,EGFontList>(font->fontRec.fontFamily, EGFontList())).first;
        }
        (*ffi).second.push_back(font);
        if (debug) {
            EGDebug("%s:%s %s\n", class_name, __func__, font->fontRec.toString().c_str());
        }
    }
}

EGFontPtr EGFont::findFont(EGFontRec fontRec)
{
    EGFontRecMap::iterator fri = fontRecMap.find(fontRec);
    return (fri != fontRecMap.end()) ? (*fri).second : EGFontPtr();
}

EGFontPtr EGFont::findFont(EGFontSpec fontSpec)
{
    EGFontSpecMap::iterator fsi = fontSpecMap.find(fontSpec);
    return (fsi != fontSpecMap.end()) ? (*fsi).second : EGFontPtr();
}

EGFontPtr EGFont::findFont(EGstring fontFamily, EGFontStyle fontStyle)
{
    EGFontFamilyMap::iterator ffi = fontFamilyMap.find(fontFamily);
    if (ffi != fontFamilyMap.end()) {
        EGFontList &fontList = (*ffi).second;
        EGFontSpec fontSpec = styleMapping[fontStyle].fontSpec();
        std::equal_to<EGFontSpec> test;
        for (EGFontList::iterator fli = fontList.begin(); fli != fontList.end(); fli++) {
            EGFontSpec matchSpec = (*fli)->fontRec.fontSpec();
            if (test(fontSpec, matchSpec)) {
                return *fli;
            }
        }
    }
    return EGFontPtr();
}

EGFontPtr EGFont::findFont(EGstring fontName)
{
    EGFontNameMap::iterator fni = fontNameMap.find(fontName);
    if (fni != fontNameMap.end()) {
        return (*fni).second;
    }
    return EGFontPtr();
}

const EGFontList& EGFont::getFontList()
{
    return fontList;
}

const EGFontRec& EGFont::getFontRec()
{
    return fontRec;
}
