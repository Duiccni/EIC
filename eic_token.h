#ifndef EIC_TOKEN_H
#define EIC_TOKEN_H

#include "eic_string.h"

namespace eic
{
	struct token_type
	{
		uint8_t basic : 2;
		uint8_t sub : 6;
	};

	namespace token_types
	{
		constexpr uint8_t other = 0, symbol = 1, alpha = 2, literal = 3;

		const char* names[] = { "other", "symbol", "alpha", "literal" };

		namespace sub
		{
			namespace literal
			{
				constexpr uint8_t intK = 0, floatK = 1, charK = 2, stringK = 3;

				const char* names[] = { "intager", "float", "char", "string" };
			}

			namespace alpha
			{
				constexpr uint8_t other = 0;

				const char* keywords[] = {
					"void",
					"int",
					"if",
					"else",
					"break",
					"return",
					"for",
					"static"
				};

				inline uint8_t get_sub_type(const string& str)
				{
					str.set_cstring();
					uint8_t i = find_in(str.start, keywords, 8);
					str.undo_cstring();
					if (i == 0xFF)
						return other;
					return i + 1;
				}
			}
		}
	}

	struct token
	{
		string str;
		token_type type;
	};

	std::ostream& operator<<(std::ostream& os, const token& tkn)
	{
		os << "{'type': \033[36m" << token_types::names[tkn.type.basic];

		switch (tkn.type.basic)
		{
		case token_types::literal:
			os << '-' << token_types::sub::literal::names[tkn.type.sub];
			break;
		case token_types::alpha:
			os << (tkn.type.sub ? "-keyword" : "-other");
			break;
		}

		return os
			<< "\033[0m (\033[90m"
			<< +tkn.type.basic
			<< ", "
			<< +tkn.type.sub
			<< "\033[0m), 'value': '\033[32m"
			<< tkn.str
			<< "\033[0m'}";
	}
}

#endif // !EIC_TOKEN_H
