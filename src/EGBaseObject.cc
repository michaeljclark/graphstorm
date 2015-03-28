/*
 *  EGBaseObject.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGUtil.h"
#include "EGBaseObject.h"

const EGTypePoint           EGBaseObjectType::Point;
const EGTypeSize            EGBaseObjectType::Size;
const EGTypeRect            EGBaseObjectType::Rect;
const EGTypeIndent          EGBaseObjectType::Indent;
const EGTypeColor           EGBaseObjectType::Color;

/* EGTypePoint */

EGstring EGTypePoint::toString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << static_cast<const EGPoint*>(addr)->x << ", "
       << static_cast<const EGPoint*>(addr)->y;
    return ss.str();
}

EGstring EGTypePoint::toJSONString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << "[ " << toString(addr, nelem) << " ]";
    return ss.str();
}

void EGTypePoint::fromString(const void *addr, const EGstring &str, size_t nelem) const
{
    std::vector<std::string> strvec = EGUtil::split(str.c_str(), ",");
    if (strvec.size() == 2) {
        static_cast<EGPoint*>(const_cast<void*>(addr))->x = atoi(strvec[0].c_str());
        static_cast<EGPoint*>(const_cast<void*>(addr))->y = atoi(strvec[1].c_str());
    }
}

/* EGTypeSize */

EGstring EGTypeSize::toString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << static_cast<const EGSize*>(addr)->width << ", "
       << static_cast<const EGSize*>(addr)->height;
    return ss.str();
}

EGstring EGTypeSize::toJSONString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << "[ " << toString(addr, nelem) << " ]";
    return ss.str();
}

void EGTypeSize::fromString(const void *addr, const EGstring &str, size_t nelem) const
{
    std::vector<std::string> strvec = EGUtil::split(str.c_str(), ",");
    if (strvec.size() == 2) {
        static_cast<EGSize*>(const_cast<void*>(addr))->width = atoi(strvec[0].c_str());
        static_cast<EGSize*>(const_cast<void*>(addr))->height = atoi(strvec[1].c_str());
    }
}

/* EGTypeRect */

EGstring EGTypeRect::toString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << static_cast<const EGRect*>(addr)->x << ", "
       << static_cast<const EGRect*>(addr)->y << ", "
       << static_cast<const EGRect*>(addr)->width << ", "
       << static_cast<const EGRect*>(addr)->height;
    return ss.str();
}

EGstring EGTypeRect::toJSONString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << "[ " << toString(addr, nelem) << " ]";
    return ss.str();
}

void EGTypeRect::fromString(const void *addr, const EGstring &str, size_t nelem) const
{
    std::vector<std::string> strvec = EGUtil::split(str.c_str(), ",");
    if (strvec.size() == 4) {
        static_cast<EGRect*>(const_cast<void*>(addr))->x = atoi(strvec[0].c_str());
        static_cast<EGRect*>(const_cast<void*>(addr))->y = atoi(strvec[1].c_str());
        static_cast<EGRect*>(const_cast<void*>(addr))->width = atoi(strvec[2].c_str());
        static_cast<EGRect*>(const_cast<void*>(addr))->height = atoi(strvec[3].c_str());
    }
}

/* EGTypeIndent */

EGstring EGTypeIndent::toString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << static_cast<const EGIndent*>(addr)->top << ", "
       << static_cast<const EGIndent*>(addr)->bottom << ", "
       << static_cast<const EGIndent*>(addr)->left << ", "
       << static_cast<const EGIndent*>(addr)->right;
    return ss.str();
}

EGstring EGTypeIndent::toJSONString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << "[ " << toString(addr, nelem) << " ]";
    return ss.str();
}

void EGTypeIndent::fromString(const void *addr, const EGstring &str, size_t nelem) const
{
    std::vector<std::string> strvec = EGUtil::split(str.c_str(), ",");
    if (strvec.size() == 4) {
        static_cast<EGIndent*>(const_cast<void*>(addr))->top = atoi(strvec[0].c_str());
        static_cast<EGIndent*>(const_cast<void*>(addr))->bottom = atoi(strvec[1].c_str());
        static_cast<EGIndent*>(const_cast<void*>(addr))->left = atoi(strvec[2].c_str());
        static_cast<EGIndent*>(const_cast<void*>(addr))->right = atoi(strvec[3].c_str());
    }
}

/* EGTypeColor */

EGstring EGTypeColor::toString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << static_cast<const EGColor*>(addr)->red << ", "
       << static_cast<const EGColor*>(addr)->green << ", "
       << static_cast<const EGColor*>(addr)->blue << ", "
       << static_cast<const EGColor*>(addr)->alpha;
    return ss.str();
}

EGstring EGTypeColor::toJSONString(const void *addr, size_t nelem) const
{
    std::stringstream ss;
    ss << "[ " << toString(addr, nelem) << " ]";
    return ss.str();
}

void EGTypeColor::fromString(const void *addr, const EGstring &str, size_t nelem) const
{
    std::vector<std::string> strvec = EGUtil::split(str.c_str(), ",");
    if (strvec.size() == 4) {
        static_cast<EGColor*>(const_cast<void*>(addr))->red = atof(strvec[0].c_str());
        static_cast<EGColor*>(const_cast<void*>(addr))->green = atof(strvec[1].c_str());
        static_cast<EGColor*>(const_cast<void*>(addr))->blue = atof(strvec[2].c_str());
        static_cast<EGColor*>(const_cast<void*>(addr))->alpha = atof(strvec[3].c_str());
    }
}
