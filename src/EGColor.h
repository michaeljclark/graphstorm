// See LICENSE for license details.

#pragma once

/* EGColor */

class EGColor
{
public:
    EGfloat red;
    EGfloat green;
    EGfloat blue;
    EGfloat alpha;
    
    EGColor(EGfloat red, EGfloat green, EGfloat blue, EGfloat alpha) : red(red), green(green), blue(blue), alpha(alpha) {}
    EGColor(const EGColor &o) : red(o.red), green(o.green), blue(o.blue), alpha(o.alpha) {}
    EGColor() : red(0), green(0), blue(0), alpha(0) {}
        
    inline char hexdigit(EGint d) const { return (d < 10) ? '0' + d : 'A' + (d-10); }
    
    std::string htmlColor() const
    {
        char buf[8];
        EGubyte r = (EGubyte)(red * 255.0f);
        EGubyte g = (EGubyte)(green * 255.0f);
        EGubyte b = (EGubyte)(blue * 255.0f);
        sprintf(buf, "#%c%c%c%c%c%c",
                hexdigit(r / 16), hexdigit(r % 16), hexdigit(g / 16), hexdigit(g % 16), hexdigit(b / 16), hexdigit(b % 16));
        return std::string(buf);
    }

    static EGColor fromHex(std::string hex)
    {
        if (!hex.length()) {
            return EGColor(1, 1, 1, 1);
        }
        if (hex[0] == '#') {
            hex = hex.substr(1);
        }
        EGint rgba;
        sscanf(hex.c_str(), "%x", &rgba);
        if (hex.length() == 8) {
            return EGColor(((rgba >> 24) & 0x0ff) / 255.0f, ((rgba >> 16) & 0xff) / 255.0f, ((rgba >> 8) & 0xff) / 255.0f, (rgba & 0xff) / 255.0f);
        } else if (hex.length() == 6) {
            return EGColor(((rgba >> 16) & 0xff) / 255.0f, ((rgba >> 8) & 0xff) / 255.0f, (rgba & 0xff) / 255.0f, 1);
        } else if (hex.length() == 2) {
            return EGColor((rgba & 0xff) / 255.0f, (rgba & 0xff) / 255.0f, (rgba & 0xff) / 255.0f, 1);
        } else {
            return EGColor(1, 1, 1, 1);
        }
    }
    
    EGColor saturate(EGfloat f) const
    {
        EGfloat g = 0.299f * red + 0.587f * green + 0.114f * blue; // CCIR601 perceived luminance
        return EGColor(f * red + (1.0f-f) * g, f * green + (1.0f-f) * g, f * blue + (1.0f-f) * g, alpha);
    }
    
    EGColor brighten(EGfloat f) const
    {
        EGfloat r = red * f;
        EGfloat g = green * f;
        EGfloat b = blue * f;
        return EGColor(r > 1.0f ? 1.0f : r, g > 1.0f ? 1.0f : g, b > 1.0f ? 1.0f : b, alpha);
    }
        
    unsigned int rgba32() const
    {
        union { char c[4]; EGuint rgba; } tou32;
        tou32.c[0] = (EGubyte)(red * 255.0f);
        tou32.c[1] = (EGubyte)(green * 255.0f);
        tou32.c[2] = (EGubyte)(blue * 255.0f);
        tou32.c[3] = (EGubyte)(alpha * 255.0f);
        return tou32.rgba;
    }
    
    bool operator==(const EGColor &o)
    {
        return red == o.red && green == o.green && blue == o.blue && alpha == o.alpha;
    }
    
    bool operator!=(const EGColor &o)
    {
        return !(*this == o);
    }
};
