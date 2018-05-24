#pragma once

#include "Core/Common.h"



class ColorRGBA8
{
public:

	union 
	{
		struct
		{
			byte r, g, b, a;
		};
		uint m_Mask;
	};

	inline ColorRGBA8() :
		r(0), g(0), b(0), a(0)
	{
	}

	inline ColorRGBA8(byte R, byte G, byte B, byte A) :
		r(R), g(G), b(B), a(A)
	{
	}

	inline ColorRGBA8(int InColor) :
		m_Mask(InColor)
	{
	}

	inline ColorRGBA8 operator * (const ColorRGBA8& Right) const 
	{
		return ColorRGBA8(r * Right.r, g * Right.g, b * Right.g, a * Right.a);
	}

	inline ColorRGBA8 operator / (const ColorRGBA8& Right) const
	{
		return ColorRGBA8(r / Right.r, g / Right.g, b / Right.b, a / Right.a);
	}

	inline ColorRGBA8 operator + (const ColorRGBA8& Right) const
	{
		return ColorRGBA8(r + Right.r, g + Right.g, b + Right.b, a + Right.a);
	}

	inline ColorRGBA8 operator - (const ColorRGBA8& Right) const
	{
		return ColorRGBA8(r - Right.r, g - Right.g, b - Right.b, a - Right.a);
	}

	inline ColorRGBA8& operator *= (const ColorRGBA8& Right)
	{
		r *= Right.r;
		g *= Right.g;
		b *= Right.b;
		a *= Right.a;
		return *this;
	}

	inline ColorRGBA8& operator /= (const ColorRGBA8& Right)
	{
		r /= Right.r;
		g /= Right.g;
		b /= Right.b;
		a /= Right.a;
		return *this;
	}

	inline ColorRGBA8& operator += (const ColorRGBA8& Right)
	{
		r += Right.r;
		g += Right.g;
		b += Right.b;
		a += Right.a;
		return *this;
	}

	inline ColorRGBA8& operator -= (const ColorRGBA8& Right)
	{
		r -= Right.r;
		g -= Right.g;
		b -= Right.b;
		a -= Right.a;
		return *this;
	}

	inline ColorRGBA8& operator ++()
	{
		r++;
		g++;
		b++;
		a++;
		return *this;
	}

	inline ColorRGBA8& operator --()
	{
		r--;
		g--;
		b--;
		a--;
		return *this;
	}

	inline ColorRGBA8& operator = (const ColorRGBA8& Right)
	{
		m_Mask = Right.GetMask();
		return *this;
	}

	inline bool IsZero() const
	{
		return m_Mask == 0;
	}

	inline uint GetMask() const
	{
		return m_Mask;
	}

};


class ColorRGB8
{
public:

	byte r, g, b;
		
	inline ColorRGB8() :
		r(0), g(0), b(0)
	{
	}

	inline ColorRGB8(byte R, byte G, byte B) :
		r(R), g(G), b(B)
	{
	}

	inline ColorRGB8 operator * (const ColorRGB8& Right) const
	{
		return ColorRGB8(r * Right.r, g * Right.g, b * Right.g);
	}

	inline ColorRGB8 operator / (const ColorRGB8& Right) const
	{
		return ColorRGB8(r / Right.r, g / Right.g, b / Right.b);
	}

	inline ColorRGB8 operator + (const ColorRGB8& Right) const
	{
		return ColorRGB8(r + Right.r, g + Right.g, b + Right.b);
	}

	inline ColorRGB8 operator - (const ColorRGB8& Right) const
	{
		return ColorRGB8(r - Right.r, g - Right.g, b - Right.b);
	}

	inline ColorRGB8& operator *= (const ColorRGB8& Right)
	{
		r *= Right.r;
		g *= Right.g;
		b *= Right.b;
		return *this;
	}

	inline ColorRGB8& operator /= (const ColorRGB8& Right)
	{
		r /= Right.r;
		g /= Right.g;
		b /= Right.b;
		return *this;
	}

	inline ColorRGB8& operator += (const ColorRGB8& Right)
	{
		r += Right.r;
		g += Right.g;
		b += Right.b;
		return *this;
	}

	inline ColorRGB8& operator -= (const ColorRGB8& Right)
	{
		r -= Right.r;
		g -= Right.g;
		b -= Right.b;
		return *this;
	}

	inline ColorRGB8& operator ++()
	{
		r++;
		g++;
		b++;
		return *this;
	}

	inline ColorRGB8& operator --()
	{
		r--;
		g--;
		b--;
		return *this;
	}

	inline ColorRGB8& operator = (const ColorRGB8& Right)
	{
		r = Right.r;
		g = Right.g;
		b = Right.b;
		return *this;
	}

	inline bool IsZero() const
	{
		return ((r == 0) && (g == 0) && (b == 0));
	}

};


class ColorBGRA8
{
public:

	union
	{
		struct
		{
			byte b, g, r, a;
		};
		uint m_Mask;
	};

	inline ColorBGRA8() :
		b(0), g(0), r(0), a(0)
	{
	}

	inline ColorBGRA8(byte B, byte G, byte R, byte A) :
		b(R), g(G), r(B), a(A)
	{
	}

	inline ColorBGRA8(int InColor) :
		m_Mask(InColor)
	{
	}

	inline ColorBGRA8 operator * (const ColorBGRA8& Right) const
	{
		return ColorBGRA8(b * Right.b, g * Right.g, r * Right.r, a * Right.a);
	}

	inline ColorBGRA8 operator / (const ColorBGRA8& Right) const
	{
		return ColorBGRA8(b / Right.b, g / Right.g, r / Right.r, a / Right.a);
	}

	inline ColorBGRA8 operator + (const ColorBGRA8& Right) const
	{
		return ColorBGRA8(b + Right.b, g + Right.g, r + Right.r, a + Right.a);
	}

	inline ColorBGRA8 operator - (const ColorBGRA8& Right) const
	{
		return ColorBGRA8(b - Right.b, g - Right.g, r - Right.r, a - Right.a);
	}

	inline ColorBGRA8& operator *= (const ColorBGRA8& Right)
	{
		b *= Right.b;
		g *= Right.g;
		r *= Right.r;
		a *= Right.a;
		return *this;
	}

	inline ColorBGRA8& operator /= (const ColorBGRA8& Right)
	{
		b /= Right.b;
		g /= Right.g;
		r /= Right.r;
		a /= Right.a;
		return *this;
	}

	inline ColorBGRA8& operator += (const ColorBGRA8& Right)
	{
		b += Right.b;
		g += Right.g;
		r += Right.r;
		a += Right.a;
		return *this;
	}

	inline ColorBGRA8& operator -= (const ColorBGRA8& Right)
	{
		b -= Right.b;
		g -= Right.g;
		r -= Right.r;
		a -= Right.a;
		return *this;
	}

	inline ColorBGRA8& operator ++()
	{
		b++;
		g++;
		r++;
		a++;
		return *this;
	}

	inline ColorBGRA8& operator --()
	{
		b--;
		g--;
		r--;
		a--;
		return *this;
	}

	inline ColorBGRA8& operator = (const ColorBGRA8& Right)
	{
		m_Mask = Right.GetMask();
		return *this;
	}

	inline bool IsZero() const
	{
		return m_Mask == 0;
	}

	inline uint GetMask() const
	{
		return m_Mask;
	}

};




