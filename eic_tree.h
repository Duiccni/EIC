#ifndef EIC_TREE_H
#define EIC_TREE_H

#include "eic_llist.h"
#include "eic_token.h"

namespace eic
{
	template <typename T>
	concept is_pointer = std::is_pointer<T>::value;

	template <typename T>
	struct tree_node
	{
		T value;
		list<tree_node<T>>* childs;

		tree_node()
			: value()
			, childs(nullptr)
		{
		}

		tree_node(const T& value)
			: value(value)
			, childs(nullptr)
		{
		}

		inline void create_childs_list()
		{
			childs = ealloc<list<tree_node<T>>>(1);
			*childs = list<tree_node<T>>();
		}

		tree_node(const T& value, const list<T>& _list)
			: value(value)
		{
			create_childs_list();

			for (list_node<T>* node = _list.first; node; node = node->next)
				childs->push_back(tree_node<T>(node->value));
		}

		template <typename U> requires std::same_as<T, U*>
		tree_node(const T& value, const list<U>& _list)
			: value(value)
		{
			create_childs_list();

			for (list_node<U>* node = _list.first; node; node = node->next)
				childs->push_back(tree_node<T>(&node->value));
		}

		void clear()
		{
			if (childs == nullptr)
				return;

			for (list_node<tree_node<T>>* node = childs->first, *next = nullptr;
				node; node = node->next)
			{
				next = node->next;
				node->value.clear();
				efree(node);
			}

			efree(childs);
		}

		void _print(uint8_t depth)
		{
			for (uint8_t i = 0; i < depth; i++)
				std::cout << '|' << ' ';

			std::cout << value << '\n';

			if (childs)
			{
				depth++;
				for (list_node<tree_node<T>>* node = childs->first; node; node = node->next)
					node->value._print(depth);
			}
		}

		inline void print()
		{
			_print(0);
		}

		void _print_ptr(uint8_t depth) requires is_pointer<T>
		{
			std::cout << "\033[90m";
			for (uint8_t i = 0; i < depth; i++)
				std::cout << '|' << ' ';
			std::cout << "\033[0m";

			if (value)
				std::cout << *value;
			else
				std::cout << "NULL";
			std::cout << '\n';

			if (childs)
			{
				depth++;
				for (list_node<tree_node<T>>* node = childs->first; node; node = node->next)
					node->value._print_ptr(depth);
			}
		}

		inline void print_ptr()
		{
			_print_ptr(0);
		}

		void _reassemble(token* tkn) requires std::same_as<T, token*>
		{
			if (childs)
			{
				if (value == nullptr)
				{
					list_node<tree_node<token*>>* node;
					for (node = childs->first; node->next; node = node->next)
					{
						node->value._reassemble(nullptr);
						std::cout << '\n';
					}
					node->value._reassemble(nullptr);
				}
				else if (childs->size == 1)
				{
					std::cout << value->str;
					childs->first->value._reassemble(nullptr);
				}
				else if (childs->size == 2)
				{
					bool paranthesis = tkn != nullptr && tkn->str.compare(value->str) == false;

					if (paranthesis)
						std::cout << '(';

					childs->first->value._reassemble(value);
					std::cout << ' ' << value->str << ' ';
					childs->last->value._reassemble(value);

					if (paranthesis)
						std::cout << ')';
				}

				return;
			}

			if (value)
				std::cout << value->str;
		}

		inline void reassemble() requires std::same_as<T, token*>
		{
			_reassemble(nullptr);
		}
	};
}

#endif // !EIC_TREE_H
