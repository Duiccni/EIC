#ifndef EIC_STRING_H
#define EIC_STRING_H

#include "eic_memmang.h"

namespace eic
{
	inline char* find_end(char* str)
	{
		while (*str != '\0')
			str++;
		return str;
	}

	constexpr const char* find_end(const char* str)
	{
		while (*str != '\0')
			str++;
		return str;
	}

	inline char* find_first(char* str, char c)
	{
		while (*str != '\0' && *str != c)
			str++;
		return str;
	}

	inline const char* find_first(const char* str, char c)
	{
		while (*str != '\0' && *str != c)
			str++;
		return str;
	}

	inline char* find_first_not(char* str, char c)
	{
		while (*str == c)
			str++;
		return str;
	}

	inline char* find_first(char* str, const char* chars)
	{
		while (*str != '\0')
		{
			for (const char* c = chars; *c != '\0'; c++)
				if (*str == *c)
					return str;
			str++;
		}
		return str;
	}

	inline char* find_first(char* str, bool cond(char c))
	{
		while (*str != '\0' && cond(*str) == false)
			str++;
		return str;
	}

	inline char* find_first_not(char* str, bool cond(char c))
	{
		while (cond(*str))
			str++;
		return str;
	}

	// 0xFF if not in
	inline uint8_t find_in(char* str, const char** list, uint8_t amount)
	{
		for (uint8_t i = 0; i < amount; i++)
			if (strcmp(str, list[i]) == 0)
				return i;
		return 0xFF;
	}

	char cstring_default_temp_location = '\0';

	struct string
	{
		char* start;
		uint16_t length;

		constexpr string()
			: start(nullptr)
			, length(0)
		{
		}

		string(char* start, uint16_t length)
			: start(start)
			, length(length)
		{
		}

		inline char* end() const
		{
			return start + length;
		}

		consteval string(const char* cstr)
			: length(find_end(cstr) - cstr)
			, start(const_cast<char*>(cstr))
		{
		}

		inline void set_cstring(char* temp_location = &cstring_default_temp_location) const
		{
			*temp_location = start[length];
			start[length] = '\0';
		}

		inline void undo_cstring(char* temp_location = &cstring_default_temp_location) const
		{
			start[length] = *temp_location;
		}

		inline bool compare(const string& str) const
		{
			if (str.length != length)
				return false;

			for (uint8_t i = 0; i < length; i++)
				if (start[i] != str.start[i])
					return false;
			return true;
		}
	};

	std::ostream& operator<<(std::ostream& os, const string& str)
	{
		for (uint16_t i = 0; i < str.length; i++)
			os << str.start[i];
		return os;
	}
}

#endif // !EIC_STRING_H
