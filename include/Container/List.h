#ifndef __MEMORY_LIST_H__
#define __MEMORY_LIST_H__

#include <cassert>
#include <utility>
#include <new>
#include <cstddef>

namespace Memory
{
	template<typename T>
	class List
	{
	private:
		struct NodeBase
		{
			NodeBase* prev;
			NodeBase* next;

			NodeBase()
				: prev(nullptr)
				, next(nullptr)
			{}

			NodeBase(const NodeBase& other)
				: prev(other.prev)
				, next(other.next)
			{}
		};
		struct Node : NodeBase
		{
			T item;

			Node()
				: NodeBase()
				, item()
			{}

			Node(const Node& other)
				: NodeBase(other)
				, item(other.item)
			{}
		};

	public:
		class Iterator
		{
		public:
			Iterator(NodeBase* node)
				: m_node(node)
			{}

			~Iterator() = default;

			Iterator& operator++()
			{
				if (nullptr == m_node)
				{
					return *this;
				}

				m_node = m_node->next;

				return *this;
			}

			Iterator& operator--()
			{
				if (nullptr == m_node)
				{
					return *this;
				}

				m_node = m_node->prev;

				return *this;
			}

			Iterator operator++(int)
			{
				Iterator itr = *this;
				++(*this);
				return itr;
			}

			Iterator operator--(int)
			{
				Iterator itr = *this;
				--(*this);
				return itr;
			}

			bool operator==(const Iterator& other) const
			{
				return m_node == other.m_node;
			}

			bool operator!=(const Iterator& other) const
			{
				return m_node != other.m_node;
			}

			T* operator->() const
			{
				if (nullptr == m_node)
				{
					return nullptr;
				}

				Node* node = static_cast<Node*>(m_node);
				return &(node->item);
			}

			T& operator*() const
			{
				Node* node = static_cast<Node*>(m_node);
				return node->item;
			}

		private:
			friend class List;

			NodeBase* m_node;
		};

	public:
		List()
			: m_count(0)
			, m_end()
		{
			m_end.prev = &m_end;
			m_end.next = &m_end;
		}

		List(const List<T>& other) = delete;

		explicit List(List<T>&& other) noexcept
			: List()
		{
			this->Splice(Begin(), other);
		}

		~List()
		{
			Clear();
		}

		List& operator=(const List<T>& other) = delete;

		List& operator=(List<T>&& other) noexcept
		{
			if (this != &other)
			{
				Clear();
				this->Splice(End(), other);
			}

			return *this;
		}

	public:
		void PushFront(const T& item)
		{
			Iterator begin = Begin();

			Insert(begin, item);
		}

		void PushBack(const T& item)
		{
			Iterator end = End();

			Insert(end, item);
		}

		T PopFront()
		{
			Iterator begin = Begin();

			T beginValue = *begin;

			Erase(begin);

			return beginValue;
		}

		T PopBack()
		{
			Iterator back = --End();

			T backValue = *back;

			Erase(back);

			return backValue;
		}

		T& Front()
		{
			Node* head = static_cast<Node*>(m_end.next);
			assert(nullptr != head && "List is empty.");
			return head->item;
		}

		T& Back()
		{
			Node* tail = static_cast<Node*>(m_end.prev);
			assert(nullptr != tail && "List is empty.");
			return tail->item;
		}

		Iterator Begin()
		{
			return Iterator(m_end.next);
		}

		Iterator End()
		{
			return Iterator(&m_end);
		}

		const Iterator Begin() const
		{
			return Iterator(m_end.next);
		}

		const Iterator End() const
		{
			return Iterator(&m_end);
		}

		Iterator Insert(Iterator pos, const T& item)
		{
			NodeBase* node = pos.m_node;
			if (nullptr == node || nullptr == node->prev || nullptr == node->next)
			{
				return End();
			}

			NodeBase* prev = node->prev;
			Node* newNode = new (std::nothrow) Node;
			if (nullptr == newNode)
			{
				return End();
			}

			newNode->item = item;
			newNode->prev = prev;
			newNode->next = node;

			prev->next = static_cast<NodeBase*>(newNode);
			node->prev = static_cast<NodeBase*>(newNode);

			m_count++;

			return Iterator(newNode);
		}

		Iterator Erase(const Iterator itr)
		{
			if (itr == End())
			{
				return End();
			}

			NodeBase* node = itr.m_node;
			if (nullptr == node || nullptr == node->prev || nullptr == node->next)
			{
				return End();
			}

			NodeBase* prev = node->prev;
			NodeBase* next = node->next;

			prev->next = next;
			next->prev = prev;

			m_count--;

			delete node;

			return Iterator(next);
		}

		Iterator Find(const T& item)
		{
			NodeBase* node = m_end.next;
			while (node != m_end)
			{
				const T& nodeItem = static_cast<Node*>(node)->item;
				if (nodeItem == item)
				{
					return Iterator(node);
				}

				node = node->next;
			}

			return End();
		}

		void Splice(const Iterator pos, List<T>& other)
		{
			if (this == &other)
			{
				return;
			}

			NodeBase* node = pos.m_node;
			if (nullptr == node || nullptr == node->prev || nullptr == node->next)
			{
				return;
			}

			NodeBase* prev = node->prev;
			NodeBase* head = other.m_end.next;
			NodeBase* tail = other.m_end.prev;

			prev->next = head;
			head->prev = prev;

			node->prev = tail;
			tail->next = node;

			other.m_end.next = &other.m_end;
			other.m_end.prev = &other.m_end;
			m_count += other.m_count;
			other.m_count = 0;
		}

		void Splice(const Iterator pos, List<T>& other, const Iterator itr)
		{
			if (pos == itr)
			{
				return;
			}

			NodeBase* node = pos.m_node;
			if (nullptr == node || nullptr == node->prev)
			{
				return;
			}

			NodeBase* otherNode = itr.m_node;
			if (nullptr == otherNode || nullptr == otherNode->prev || nullptr == otherNode->next)
			{
				return;
			}

			NodeBase* otherPrev = otherNode->prev;
			NodeBase* otherNext = otherNode->next;

			otherPrev->next = otherNext;
			otherNext->prev = otherPrev;

			NodeBase* prev = node->prev;
			prev->next = otherNode;
			otherNode->prev = prev;

			node->prev = otherNode;
			otherNode->next = node;

			other.m_count--;
			m_count++;
		}

		void Splice(const Iterator pos, List<T>& other, const Iterator first, const Iterator last)
		{
			if (pos == last || first == last)
			{
				return;
			}

			NodeBase* node = pos.m_node;
			if (nullptr == node || nullptr == node->prev)
			{
				return;
			}

			NodeBase* firstNode = first.m_node;
			NodeBase* endNode = last.m_node->prev;
			if (nullptr == firstNode || nullptr == firstNode->prev ||
				nullptr == endNode || nullptr == endNode->next)
			{
				return;
			}

			NodeBase* firstPrev = firstNode->prev;
			NodeBase* lastNext = endNode->next;

			firstPrev->next = lastNext;
			lastNext->prev = firstPrev;

			Node* prev = node->prev;

			prev->next = firstNode;
			firstNode->prev = prev;

			node->prev = endNode;
			endNode->next = node;

			if (this != &other)
			{
				NodeBase* current = firstNode;
				while (current != node)
				{
					other.m_count--;
					m_count++;
					current = current->next;
				}
			}
		}

		void Remove(const T& item)
		{
			Iterator itr = Begin();

			while (itr != End())
			{
				if (*itr == item)
				{
					itr = Erase(itr);
				}
				else
				{
					itr++;
				}
			}
		}

		void Clear()
		{
			while (!Empty())
			{
				this->PopFront();
			}
		}

		bool Empty() const
		{
			return &m_end == m_end.prev && &m_end == m_end.next;
		}

		size_t Size() const
		{
			return m_count;
		}

	private:
		NodeBase m_end;
		size_t m_count;
	};
};

#endif // __MEMORY_LIST_H__