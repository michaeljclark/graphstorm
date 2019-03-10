// See LICENSE for license details.

#pragma once

struct EGSize;
struct EGPoint;
struct EGRect;
struct EGLine;
struct EGIndent;
typedef std::vector<EGSize> EGSizeList;
typedef std::vector<EGPoint> EGPointList;
typedef std::vector<EGRect> EGRectList;
typedef std::vector<EGLine> EGLineList;
typedef std::shared_ptr<EGSizeList> EGSizeListPtr;
typedef std::shared_ptr<EGPointList> EGPointListPtr;
typedef std::shared_ptr<EGRectList> EGRectListPtr;
typedef std::shared_ptr<EGLineList> EGLineListPtr;
typedef EGIndent EGMargin;
typedef EGIndent EGPadding;


/* EGPoint */

struct EGPoint
{
    EGint x;
    EGint y;
    
    EGPoint() : x(0), y(0) {}
    EGPoint(EGint x, EGint y) : x(x), y(y) {}
    EGPoint(const EGPoint &o) : x(o.x), y(o.y) {}
    
    bool operator==(const EGPoint &o) { return x == o.x && y == o.y; }
    bool operator!=(const EGPoint &o) { return !(*this == o); }

    EGPoint& operator+=(const EGPoint &o) { x += o.x; y += o.y; return *this; }
    EGPoint& operator-=(const EGPoint &o) { x -= o.x; y -= o.y; return *this; }
    
    EGPoint operator+(const EGPoint &o) { return EGPoint(x + o.x, y + o.y); }
    EGPoint operator-(const EGPoint &o) { return EGPoint(x - o.x, y - o.y); }
    
    EGPoint operator+(const EGfloat f) { return EGPoint(x + f, y + f); }
    EGPoint operator-(const EGfloat f) { return EGPoint(x - f, y - f); }
    
    EGPoint operator+(const EGint i) { return EGPoint(x + i, y + i); }
    EGPoint operator-(const EGint i) { return EGPoint(x - i, y - i); }
};
    
/* EGLine */

struct EGLine
{
    EGPoint p1;
    EGPoint p2;
    
    EGLine(EGPoint p1, EGPoint p2) : p1(p1), p2(p2) {}
    
    bool operator==(const EGLine &o) { return p1 == o.p1 && p2 == o.p2; }
    bool operator!=(const EGLine &o) { return !(*this == o); }
};
    
        
/* EGIndent */

struct EGIndent
{
    EGint top;
    EGint left;
    EGint bottom;
    EGint right;
    
    EGIndent() : top(0), left(0), bottom(0), right(0) {}
    EGIndent(EGint top, EGint left, EGint bottom, EGint right) : top(top), left(left), bottom(bottom), right(right) {}
    
    bool operator==(const EGIndent &o) { return top == o.top && left == o.left && bottom == o.bottom && right == o.right; }
    bool operator!=(const EGIndent &o) { return !(*this == o); }
};
    
            
/* EGSize */

struct EGSize
{
    EGint width;
    EGint height;
    
    EGSize() : width(0), height(0) {}
    EGSize(EGint width, EGint height) : width(width), height(height) {}
    EGSize(const EGSize &o) : width(o.width), height(o.height) {}
    
    EGSize expand(EGMargin margin) { return EGSize(width + (margin.left + margin.right), height + (margin.top + margin.bottom)); }
    EGSize expand(EGint margin) { return EGSize(width + margin * 2, height + margin * 2); }
    EGSize contract(EGMargin margin) { return EGSize(width - (margin.left + margin.right), height - (margin.top + margin.bottom)); }
    EGSize contract(EGint margin) { return EGSize(width - margin * 2, height - margin * 2); }
    
    bool operator==(const EGSize &o) { return width == o.width && height == o.height; }
    bool operator!=(const EGSize &o) { return !(*this == o); }
};
    
                
/* EGRect */

struct EGRect
{
    EGint x;
    EGint y;
    EGint width;
    EGint height;
    
    EGRect() : x(0), y(0), width(0), height(0) {}
    EGRect(EGint x, EGint y, EGint width, EGint height) : x(x), y(y), width(width), height(height) {}
    EGRect(const EGRect &o) : x(o.x), y(o.y), width(o.width), height(o.height) {}
    
    EGRect expand(EGMargin margin) { return EGRect(x - margin.left, y - margin.top, width + (margin.left + margin.right), height + (margin.top + margin.bottom)); }
    EGRect expand(EGint margin) { return EGRect(x - margin, y - margin, width + margin * 2, height + margin * 2); }
    EGRect contract(EGMargin margin) { return EGRect(x + margin.left, y + margin.top, width - (margin.left + margin.right), height - (margin.top + margin.bottom)); }
    EGRect contract(EGint margin) { return EGRect(x + margin, y + margin, width - margin * 2, height - margin * 2); }
    
    EGSize size() { return EGSize(width, height); }
    
    EGbool contains(EGPoint point) { return point.x >= x && point.x < x + width && point.y >= y && point.y < y + height; }
    
    bool operator==(const EGRect &o) { return x == o.x && y == o.y && width == o.width && height == o.height; }
    bool operator!=(const EGRect &o) { return !(*this == o); }
};    
