#ifndef EIC_STANDART_RULES_H
#define EIC_STANDART_RULES_H

#include "eic_lexer.h"

namespace std_rules
{
	using namespace eic::lux;

	lexer* active_lexer = nullptr;

	namespace _funcs
	{
		bool _isspace(char c)
		{
			return isspace(c) != 0;
		}

		bool _isnumeric(char c)
		{
			return (isdigit(c) != 0) || (c == '_');
		}

		bool _isalnum(char c)
		{
			return (isalnum(c) != 0) || (c == '_');
		}

		uint8_t check_space(char* c)
		{
			return isspace(*c) ? messages::skip : messages::continueM;
		}

		char* skip_space(char* c)
		{
			return eic::find_first_not(c + 1, _isspace);
		}

		uint8_t check_numeric(char* c)
		{
			return isdigit(*c) ? messages::process : messages::continueM;
		}

		eic::token process_numeric(char* c)
		{
			char* end;

			bool defined_base, alnum = false;

			if (defined_base = (c[0] == '0'))
			{
				switch (c[1])
				{
				case 'x':
				case 'X':
					alnum = true;
					[[fallthrough]];
				case 'b':
				case 'B':
					end = c + 2;
					break;
				default:
					end = c + 1;
					break;
				}
			}
			else
				end = c;

			end = eic::find_first_not(end, alnum ? _isalnum : _isnumeric);

			if (*end == '.')
			{
				if (defined_base)
					throw std::invalid_argument("base-defined float.");

				return eic::token(
					eic::string(c, eic::find_first_not(end + 1, _isnumeric) - c),
					{
						eic::token_types::literal,
						eic::token_types::sub::literal::floatK
					}
				);
			}

			return eic::token(
				eic::string(c, end - c),
				{
					eic::token_types::literal,
					eic::token_types::sub::literal::intK
				}
			);
		}

		uint8_t check_alpha(char* c)
		{
			return ((*c == '_') ? isalnum(*eic::find_first_not(c + 1, '_')) : isalpha(*c)) != 0
				? messages::process : messages::continueM;
		}

		eic::token process_alpha(char* c)
		{
			eic::string temp(c, eic::find_first_not(c, _isalnum) - c);

			return eic::token(
				temp,
				{
					eic::token_types::alpha,
					eic::token_types::sub::alpha::get_sub_type(temp)
				}
			);
		}

		uint8_t check_comment(char* c)
		{
			return (c[0] == '/') && ((c[1] == '/') || (c[1] == '*')) ? messages::skip : messages::continueM;
		}

		char* skip_comment(char* c)
		{
			if (c[1] == '/')
				return eic::find_first(c + 2, '\n') + 1;

			c += 2;
			if (*c == '/')
				throw std::invalid_argument("nuh uh");

			while (*c != '/')
				c = eic::find_first(c, '*') + 1;
			return c + 1;
		}

		uint8_t check_symbol(char* c)
		{
			return *c >= '!' && *c <= '~';
		}

		eic::token process_symbol(char* c)
		{
			char C = *c;
			if (C == '\'' || C == '"')
			{
				if (active_lexer == nullptr)
					throw std::invalid_argument("set the eic::lux::active_lexer.");

				active_lexer->tokens.push_back(eic::token(
					eic::string(c, 1),
					{ eic::token_types::symbol, 0 }
				));

				char* end = c++;

				do
					end = eic::find_first(end + 1, C);
				while (eic::is_escape_char(end));

				active_lexer->tokens.push_back(eic::token(
					eic::string(c, end - c),
					{
						eic::token_types::literal,
						C == '"'
							? eic::token_types::sub::literal::stringK
							: eic::token_types::sub::literal::charK
					}
				));

				return eic::token(
					eic::string(end, 1),
					{ eic::token_types::symbol, 0 }
				);
			}

			static const char* doublable_chars = "&+-/:<>|";
			static const char* equal_combo = "!&*+-/<=>|~^?:";

			uint8_t length;
			if ((C == '.' && c[1] == '.' && c[2] == '.') || // ...
				(c[2] == '=' && C == c[1] && (C == '<' || C == '>'))) // <<=, >>=
				length = 3;
			else if ((C == '-' && c[1] == '>') || (C == '<' && c[1] == '-') || // ->, <-
				(c[1] == '=' && eic::find_first(equal_combo, C) != equal_combo + 14) || // <=, +=, &= ...
				(C == c[1] && eic::find_first(doublable_chars, C) != doublable_chars + 8)) // &&, ++ ...
				length = 2;
			else
				length = 1;

			return eic::token(
				eic::string(c, length),
				{ eic::token_types::symbol, 0 }
			);
		}
	}

	eic::list_node<rule> rule_symbol = {
		rule("symbol", &_funcs::check_symbol, &_funcs::process_symbol, nullptr),
		nullptr
	};

	eic::list_node<rule> rule_comment = {
		rule("comment", &_funcs::check_comment, nullptr, &_funcs::skip_comment),
		&rule_symbol
	};

	eic::list_node<rule> rule_alpha = {
		rule("alphabetic", &_funcs::check_alpha, &_funcs::process_alpha, nullptr),
		&rule_comment
	};

	eic::list_node<rule> rule_numeric = {
		rule("numeric", &_funcs::check_numeric, &_funcs::process_numeric, nullptr),
		&rule_alpha
	};

	eic::list_node<rule> rule_whitespace = {
		rule("whitespace", &_funcs::check_space, nullptr, &_funcs::skip_space),
		&rule_numeric
	};

	eic::list<rule> rules(&rule_whitespace, &rule_symbol, 5);
}

#endif // !EIC_STANDARD_RULES_H
