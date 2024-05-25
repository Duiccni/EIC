#include "eic_parser.h"
#include "eic_standart_rules.h"

int main()
{
	eic::lux::lexer lex("test.txt", &std_rules::rules);

	std_rules::active_lexer = &lex;

	std::cout << '\n';

	{
		uint16_t i = 0;
		uint8_t msg;
		while (true)
		{
			msg = lex.next();

			std::cout << i++ << ":\t" << eic::lux::messages::names[msg] << ' ' << lex.last_rule->name << '\n';

			if (msg == eic::lux::messages::end_of_the_file)
				break;
		}
	}

	eic::tree_node<eic::token*> root(nullptr, lex.tokens);

	eic::paimon::connect_all_numerics_with_signs(&root);

	{
		using namespace eic::paimon;

		static eic::string _priority1[] = { "*" };
		static eic::string _priority2[] = { "+", "-" };

		child_lnode* lbracket_offset_1, * rbracket_offset_1;

		while (true)
		{
			lbracket_offset_1 = find_deepest_bracket(&root);
			if (lbracket_offset_1 == (child_lnode*)0xFFFFFFFFULL)
				break;

			rbracket_offset_1 = split_all_acording_bracket(&root, lbracket_offset_1, _priority2, 2);

			root.childs->pop(lbracket_offset_1);
			root.childs->pop(rbracket_offset_1);
		}

		split_all(&root, _priority2, 2);
	}

	std::cout << "\n\033[91mLexer List:\033[0m\n";

	lex.tokens.print(true);

	std::cout << "\n\n\033[91mParser Tree:\033[0m\n";

	root.print_ptr();

	std::cout << "\n\033[91mReassemble:\033[0m\n";

	root.reassemble();

	root.clear();

	std::cout << "\n\n" << eic::memory_state << "\n\n";

	lex.clear();

	std::cout << eic::memory_state << '\n';
}