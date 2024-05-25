#ifndef EIC_LEXER_H
#define EIC_LEXER_H

#include "eic_llist.h"
#include "eic_token.h"
#include <fstream>

namespace eic
{
	inline bool is_escape_char(char* c)
	{
		unsigned int i = 1;
		while (*(c - i) == '\\')
			i++;
		return (i & 1) ^ 1;
	}

	namespace lux
	{
		namespace messages
		{
			constexpr uint8_t
				continueM = 0,
				process = 1,
				skip = 2,
				end_of_the_file = 3,
				broken = 4;

			const char* names[] = {
				"continue (none of the rules)",
				"process",
				"skip",
				"end of the file",
				"broken"
			};
		}

		struct rule
		{
			const char* name;

			uint8_t(*check)(char*);
			token(*process)(char*);
			char* (*skip)(char*);

			rule(const char* name,
				uint8_t(*check)(char*),
				token(*process)(char*),
				char* (*skip)(char*))
				: name(name)
				, check(check)
				, process(process)
				, skip(skip)
			{
			}

			rule()
				: name(nullptr)
				, check(nullptr)
				, process(nullptr)
				, skip(nullptr)
			{
			}
		};

		struct lexer
		{
			char* buffer, * current;
			uint32_t file_size;

			list<token> tokens;
			list<rule>* rules;

			rule* last_rule;
			bool broken;

			lexer(const char* file_path, list<rule>* rules)
				: tokens()
				, rules(rules)
				, last_rule(nullptr)
				, broken(false)
			{
				std::ifstream file(file_path);
				assert(file.is_open());

				file.seekg(0, std::ios::end);
				file_size = file.tellg();
				file.seekg(0, std::ios::beg);

				std::cout << "File Size (bytes): " << file_size << '\n';

				buffer = current = ealloc<char>(file_size + 1);

				file.read(buffer, file_size);

				buffer[file_size] = '\0';
			}

			uint8_t next()
			{
				if (broken)
					return messages::broken;

				if (*current == '\0')
				{
					broken = true;
					return messages::end_of_the_file;
				}

				token temp;
				for (auto rule = rules->first; rule; rule = rule->next)
				{
					switch (rule->value.check(current))
					{
					case messages::skip:
						last_rule = &rule->value;
						current = rule->value.skip(current);
						return messages::skip;
					case messages::process:
						last_rule = &rule->value;
						temp = rule->value.process(current);
						tokens.push_back(temp);
						current = temp.str.end();
						return messages::process;
					case messages::continueM:
					default:
						break;
					}
				}

				broken = true;
				return messages::continueM;
			}

			inline void clear()
			{
				tokens.clear();
				efree(buffer);
				broken = true;
			}
		};
	}
}

#endif // !EIC_LEXER_H
