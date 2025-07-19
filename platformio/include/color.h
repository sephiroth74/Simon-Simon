#ifndef __SIMON_COLOR_H__
#define __SIMON_COLOR_H__

#include <Arduino.h>

namespace simon {

class ColorRGB {
public:
    uint8_t r; // Red component
    uint8_t g; // Green component
    uint8_t b; // Blue component

    ColorRGB(uint8_t red, uint8_t green, uint8_t blue)
        : r(red)
        , g(green)
        , b(blue)
    {
    }

    ColorRGB(uint32_t rgb)
        : r((rgb >> 16) & 0xFF)
        , g((rgb >> 8) & 0xFF)
        , b(rgb & 0xFF)
    {
    }

    ColorRGB()
        : r(0)
        , g(0)
        , b(0)
    {
    }

    uint32_t toRGB() const
    {
        return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | b;
    }

    bool operator==(const ColorRGB& other) const
    {
        return r == other.r && g == other.g && b == other.b;
    }

    bool operator!=(const ColorRGB& other) const
    {
        return !(*this == other);
    }

    String toString() const
    {
        return String("ColorRGB(R: ") + r + ", G: " + g + ", B: " + b + ")";
    }

    static ColorRGB fromRGB(uint32_t rgb)
    {
        return ColorRGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
    }
};

} // namespace simon

#endif // __SIMON_COLOR_H__