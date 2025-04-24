#pragma once
#include <cstdint>


namespace Math
{
	static inline int IsDigit(char c)
	{
		return c >= '0' && c <= '9';
	}

	static inline int ToDigit(char c)
	{
		return c - '0';
	}

	inline int ToInt(const char* str) 
	{
		int signCoefficient = 1;
		if (*str == '-')
		{
			str++;
			signCoefficient = -1;
		}
		
		int integer = 0;

		while (IsDigit(*str))
		{
			integer = integer * 10 + ToDigit(*str++);
		}
		
		return integer * signCoefficient;
	}
	
	inline uint32_t ToUnsignedInt(const char* str) 
	{
		uint32_t integer = 0;

		while (IsDigit(*str))
		{
			integer = integer * 10 + ToDigit(*str++);
		}
		
		return integer;
	}
}