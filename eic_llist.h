#ifndef EIC_LLIST_H
#define EIC_LLIST_H

#include "eic_memmang.h"

namespace eic
{
	template <typename T>
	struct list_node
	{
		T value;
		list_node<T>* next;
	};

	template <typename T>
	struct list
	{
		list_node<T>* first, * last;
		uint16_t size;

		list()
			: first(nullptr)
			, last(nullptr)
			, size(0)
		{
		}

		list(list_node<T>* first, list_node<T>* last, uint16_t size)
			: first(first)
			, last(last)
			, size(size)
		{
		}

		list(list_node<T>* first)
			: first(first)
			, last(first)
		{
			if (first == NULL)
			{
				size = 0;
				return;
			}

			for (size = 1; last->next; last = last->next)
				size++;
		}

		inline bool is_empty() const
		{
			return first == nullptr;
		}

		void push_back(const T& value)
		{
			list_node<T>* node = ealloc<list_node<T>>(1);

			node->value = value;
			node->next = nullptr;

			if (is_empty())
				first = node;
			else
				last->next = node;

			last = node;
			size++;
		}

		void push_front(const T& value)
		{
			list_node<T>* node = ealloc<list_node<T>>(1);

			node->value = value;
			node->next = first;

			if (first == nullptr)
				last = node;

			first = node;
			size++;
		}

		inline void rotate_left()
		{
			if (size <= 1)
				return;

			last->next = first;
			last = first;
			first = first->next;
			last->next = nullptr;
		}

		T shift_left(const T& filler)
		{
			rotate_left();

			T temp = last->value;
			last->value = filler;
			return temp;
		}

		T pop_first()
		{
			if (is_empty())
				return;
			size--;
			T value = first->value;

			if (size)
			{
				list_node<T>* second = first->next;
				efree(first);
				first = second;
			}
			else
			{
				efree(first);
				first = last = nullptr;
			}

			return value;
		}

		T pop(list_node<T>* offset_1)
		{
			size--;
			T value;

			if (offset_1 == nullptr)
			{
				value = first->value;
				if (size)
				{
					list_node<T>* second = first->next;
					efree(first);
					first = second;
				}
				else
				{
					efree(first);
					first = last = nullptr;
				}
			}
			else
			{
				list_node<T>* node = offset_1->next;
				value = node->value;
				if (node->next == nullptr)
				{
					offset_1->next = nullptr;
					last = offset_1;
					efree(node);
				}
				else
				{
					offset_1->next = node->next;
					efree(node);
				}
			}

			return value;
		}

		void clear()
		{
			for (list_node<T>* node = first, *next = nullptr; node; node = next)
			{
				next = node->next;
				efree(node);
			}

			first = last = nullptr;
			size = 0;
		}

		void print(bool new_line = false) const
		{
			if (is_empty())
			{
				std::cout << "Empty List.";
				return;
			}

			list_node<T>* node = first;

			if (new_line)
			{
				std::cout << "{\n  ";
				for (; node->next; node = node->next)
					std::cout << node->value << ",\n  ";
				std::cout << node->value << "\n}[" << size << ']';
				return;
			}

			std::cout << "{";
			for (; node->next; node = node->next)
				std::cout << node->value << ", ";
			std::cout << node->value << "}[" << size << ']';
		}

		list_node<T>* find(list_node<T>* start, bool cond(const T&))
		{
			for (list_node<T>* n = first; n; n = n->next)
				if (cond(n->value))
					return n;
			return nullptr;
		}

		// returns (list_node<T>*)0xFFFFFFFFULL if cant find
		list_node<T>* find_offset_1(list_node<T>* offset_1, bool cond(const T&))
		{
			list_node<T>* temp = offset_1 ? offset_1->next : first;
			if (temp == nullptr)
				return (list_node<T>*)0xFFFFFFFFULL;

			if (cond(temp->value))
				return offset_1;

			for (offset_1 = temp->next; offset_1; temp = offset_1, offset_1 = temp->next)
				if (cond(offset_1->value))
					return temp;

			return (list_node<T>*)0xFFFFFFFFULL;
		}

		// returns (list_node<T>*)0xFFFFFFFFULL if cant find
		list_node<T>* find_offset_2(list_node<T>* offset_2, bool cond(const T&))
		{
			list_node<T>* temp = offset_2 ? offset_2->next : first;
			if (temp->next == nullptr)
				return (list_node<T>*)0xFFFFFFFFULL;

			if (cond(temp->next->value))
				return offset_2;

			for (offset_2 = temp->next->next; offset_2; offset_2 = offset_2->next, temp = temp->next)
				if (cond(offset_2->value))
					return temp;

			return (list_node<T>*)0xFFFFFFFFULL;
		}
	};
}

#endif // !EIC_LLIST_H
