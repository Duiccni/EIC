#ifndef EIC_PARSER_H
#define EIC_PARSER_H

#include "eic_tree.h"
#include "eic_lexer.h"

namespace eic
{
	namespace paimon
	{
		using parser_node = tree_node<token*>;
		using child_lnode = list_node<parser_node>;

		void split_to_two(parser_node* parent, child_lnode* offset_2)
		{
			parent->childs->size -= 2;

			child_lnode* offset_1 = offset_2 ? offset_2->next : parent->childs->first;
			child_lnode* node = offset_1->next;

			if (node->value.childs != nullptr)
				throw std::invalid_argument("This node is already have a list.");

			node->value.childs = ealloc<list<parser_node>>(1);

			*node->value.childs = {
				offset_1,
				node->next,
				2
			};

			offset_1->next = node->next;

			child_lnode* offset2 = node->next->next;
			if (offset2 == nullptr)
			{
				node->next = nullptr;
				parent->childs->last = node;
			}
			else
			{
				node->next->next = nullptr;
				node->next = offset2;
			}

			(offset_2 ? offset_2->next : parent->childs->first) = node;
		}

		void connect_with_next(parser_node* parent, child_lnode* node)
		{
			parent->childs->size--;

			if (node->value.childs != nullptr)
				throw std::invalid_argument("This node is already have a list.");

			node->value.childs = ealloc<list<parser_node>>(1);

			child_lnode* offset1 = node->next;
			*node->value.childs = {
				offset1,
				offset1,
				1
			};

			node->next = offset1->next;
			if (offset1->next == nullptr)
				parent->childs->last = node;
			else
				offset1->next = nullptr;
		}

		child_lnode* find_deepest_bracket(parser_node* parent)
		{
			child_lnode* offset_1 = nullptr, * lbracket_offset_1, * node = nullptr;
			while (true)
			{
				lbracket_offset_1 = offset_1;
				offset_1 = parent->childs->find_offset_1(node, [](const parser_node& node) -> bool
					{ return *node.value->str.start == '(' || *node.value->str.start == ')'; }
				);

				if ((uint64_t)offset_1 == 0xFFFFFFFFULL)
					return (child_lnode*)0xFFFFFFFFULL;

				node = offset_1 ? offset_1->next : parent->childs->first;

				if (*node->value.value->str.start == ')')
					return lbracket_offset_1;
			}
		}

		string __str_temp;
		void split_all(parser_node* parent, const string& str)
		{
			child_lnode* offset_2 = nullptr;

			__str_temp = str;

			while (true)
			{
				offset_2 = parent->childs->find_offset_2(offset_2, [](const parser_node& node) -> bool { return node.childs == nullptr && __str_temp.compare(node.value->str); });
				if ((uint64_t)offset_2 == 0xFFFFFFFFULL)
					break;

				eic::paimon::split_to_two(parent, offset_2);
			}
		}

		string* __str_ptr_temp;
		uint8_t __str_amount_temp;
		void split_all(parser_node* parent, string* strs, uint8_t str_amount)
		{
			child_lnode* offset_2 = nullptr;

			__str_ptr_temp = strs;
			__str_amount_temp = str_amount;

			while (true)
			{
				offset_2 = parent->childs->find_offset_2(offset_2, [](const parser_node& node) -> bool
					{
						for (uint8_t i = 0; i < __str_amount_temp; i++)
							if (node.childs == nullptr && node.value->str.compare(__str_ptr_temp[i]))
								return true;
						return false;
					}
				);
				if ((uint64_t)offset_2 == 0xFFFFFFFFULL)
					break;

				eic::paimon::split_to_two(parent, offset_2);
			}
		}

		// returns r-bracket offset -1
		child_lnode* split_all_acording_bracket(parser_node* parent, child_lnode* lbracket_offset_1, const string& str)
		{
			child_lnode* offset_2 = lbracket_offset_1;

			__str_temp = str;

			while (true)
			{
				offset_2 = parent->childs->find_offset_2(offset_2, [](const parser_node& node) -> bool
					{ return *node.value->str.start == ')' || (node.childs == nullptr && __str_temp.compare(node.value->str)); }
				);

				if (*offset_2->next->next->value.value->str.start == ')')
					return offset_2->next;

				eic::paimon::split_to_two(parent, offset_2);
			}
		}

		// returns r-bracket offset -1
		child_lnode* split_all_acording_bracket(parser_node* parent, child_lnode* lbracket_offset_1, string* strs, uint8_t str_amount)
		{
			child_lnode* offset_2 = lbracket_offset_1;

			__str_ptr_temp = strs;
			__str_amount_temp = str_amount;

			while (true)
			{
				offset_2 = parent->childs->find_offset_2(offset_2, [](const parser_node& node) -> bool
					{
						if (*node.value->str.start == ')')
							return true;
						for (uint8_t i = 0; i < __str_amount_temp; i++)
							if (node.childs == nullptr && node.value->str.compare(__str_ptr_temp[i]))
								return true;
						return false;
					}
				);

				if (*offset_2->next->next->value.value->str.start == ')')
					return offset_2->next;

				eic::paimon::split_to_two(parent, offset_2);
			}
		}

		void connect_all_numerics_with_signs(parser_node* parent)
		{
			if (parent->childs->size < 3)
				return;

			for (child_lnode* node = parent->childs->first; node->next->next; node = node->next)
			{
				const string&
					str0 = node->value.value->str,
					str1 = node->next->value.value->str;

				if (node->value.value->type.basic == token_types::symbol && *str0.start != ')'
					&& str1.length == 1 && (*str1.start == '-' || *str1.start == '+')
					&& node->next->next->value.value->type.basic != token_types::symbol)
				{
					connect_with_next(parent, node->next);
					node = node->next;
					if (node->next == nullptr || node->next->next == nullptr)
						return;
				}
			}
		}
	}
}

#endif // !EIC_PARSER_H
