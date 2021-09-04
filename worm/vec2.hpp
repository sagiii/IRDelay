/**
 * Almost copied from Qiita written by @Reputeless : https://qiita.com/Reputeless/items/96226cfe1282a014b147
 */

#pragma once 

#include <cmath>

struct Vec2
{
    double x;
    double y;

    Vec2() = default;

    constexpr Vec2(double _x, double _y)
        : x(_x)
        , y(_y) {}

    double length() const
    {
        return std::sqrt(lengthSquare());
    }

    constexpr double lengthSquare() const
    {
        return dot(*this);
    }

    constexpr double dot(const Vec2& other) const
    {
        return x * other.x + y * other.y;
    }

    double distanceFrom(const Vec2& other) const
    {
        return (other - *this).length();
    }

    Vec2 normalized() const
    {
        return *this / length();
    }

    constexpr Vec2 rotated90() const
    {
        return { -y, x };
    }

    constexpr bool isZero() const
    {
        return x == 0.0 && y == 0.0;
    }

    constexpr Vec2 operator +() const
    {
        return *this;
    }

    constexpr Vec2 operator -() const
    {
        return{ -x, -y };
    }

    constexpr Vec2 operator +(const Vec2& other) const
    {
        return{ x + other.x, y + other.y };
    }

    constexpr Vec2 operator -(const Vec2& other) const
    {
        return{ x - other.x, y - other.y };
    }

    constexpr Vec2 operator *(double s) const
    {
        return{ x * s, y * s };
    }

    constexpr Vec2 operator /(double s) const
    {
        return{ x / s, y / s };
    }

    Vec2& operator +=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator -=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator *=(double s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    Vec2& operator /=(double s)
    {
        x /= s;
        y /= s;
        return *this;
    }
};

inline constexpr Vec2 operator *(double s, const Vec2& v)
{
    return{ s*v.x, s*v.y };
}
/*
template <class Char>
inline std::basic_ostream<Char>& operator <<(std::basic_ostream<Char>& os, const Vec2& v)
{
    return os << Char('(') << v.x << Char(',') << v.y << Char(')');
}

template <class Char>
inline std::basic_istream<Char>& operator >>(std::basic_istream<Char>& is, Vec2& v)
{
    Char unused;
    return is >> unused >> v.x >> unused >> v.y >> unused;
}
*/
