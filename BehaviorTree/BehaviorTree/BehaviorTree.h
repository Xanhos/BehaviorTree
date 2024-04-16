#pragma once
#include <iostream>
#include <list>
#include <memory>
#include <functional>

#define SECURE_TASK(task) (task ? task->tick() : false)

namespace bt
{
	class Node
	{
		inline static unsigned int m_idCounter = 0;
	protected:
		unsigned int m_id;
		Node() : m_id(m_idCounter++) {}

		template<typename T>
		std::shared_ptr<T> getNode() { std::make_shared<T>(*this); }
	public:
		virtual bool tick() = 0;

		unsigned int getID() const { return m_id; }

		template<typename T>
		static std::shared_ptr<T> New(const T& node) { return std::make_shared<T>(node); }
	};

	typedef std::shared_ptr<Node> NodePtr;
	typedef std::list<std::shared_ptr<Node>> NodeList;

#pragma region COMPOSITE
	namespace Composite
	{
		class CompositeNode : public Node
		{
		protected:
			NodeList m_childList;
		public:
			CompositeNode() : Node() {}

			NodeList& getChilds() { return m_childList; }

			template <typename T>
			std::shared_ptr<T> addChild(std::shared_ptr<T> child) { m_childList.push_back(child); return std::dynamic_pointer_cast<T>(m_childList.back()); }


			template <typename T>
			std::shared_ptr<Node>* addChild_INTERNAL_USE(std::shared_ptr<T> child) { m_childList.push_back(child); return &m_childList.back(); }
		};

		class Selector : public CompositeNode
		{
		public:
			Selector() : CompositeNode() {}
			virtual bool tick() {
				for (auto& child : m_childList)
				{
					if (child->tick())
						return true;
				}
				return false;
			}

		};

		class Sequence : public CompositeNode
		{
		public:
			Sequence() : CompositeNode() {}
			virtual bool tick()
			{
				for (auto& child : m_childList)
				{
					if (!child->tick())
						return false;
				}
				return true;
			}
		};

		class Alternate : public CompositeNode
		{
		public:
			Alternate() : CompositeNode() {}
			virtual bool tick()
			{
				auto it = m_childList.begin();
				for (unsigned int index = 0; index < m_childList.size(); index++)
				{
					if (index % 2 == 0)
					{
						if (!(*it)->tick())
							return false;
					}
					else if ((*it)->tick())
						return false;
					it = std::next(it);
				}
				return true;
			}
		};
	}
	

#pragma endregion

#pragma region DECORATOR

	namespace Decorator
	{
		class Decorator : public Node
		{
		protected:
			NodePtr m_task;
		public:
			Decorator() { m_task = NodePtr(); }
			Decorator(NodePtr task) { m_task = task; }

			NodePtr setTask(NodePtr task) { m_task = task; return m_task; }
			NodePtr* setTask_INTERNAL_USE(NodePtr task) { m_task = task; return &m_task; }
		};

		class Inverser : public Decorator
		{
		public:
			Inverser() : Decorator() {}
			Inverser(NodePtr task) : Decorator(task) {}
			virtual bool tick() { return  SECURE_TASK(m_task); }
		};

		class Loop : public Decorator
		{
			unsigned int m_loopNbr;
		public:
			Loop() : Decorator() { m_loopNbr = 1u; }
			Loop(NodePtr task, unsigned int loop) : Decorator(task) { m_loopNbr = loop; }

			void setLoop(unsigned int loop) { m_loopNbr = loop; }

			virtual bool tick() {
				if(m_task)
				{
					if (m_loopNbr)
						for (unsigned int i = 0; i < m_loopNbr; i++)
						{
							if (m_task->tick())
								return true;
						}
					else while (!m_task->tick()) {};

					return false;
				}
			}
		};

		class Cooldown : public Decorator
		{
			float m_executionTimer;
			float m_timer;
		public:
			Cooldown() : Decorator() { m_timer = 0.f; }
			Cooldown(NodePtr task, float timer) : Decorator(task) { m_timer = 0.f; m_executionTimer = timer; }

			void setTimer(float timer) { m_executionTimer = timer; }
			virtual bool tick() {
				if (m_timer > m_executionTimer)
				{
					m_timer = 0.f;
					return SECURE_TASK(m_task);
				}
				return false;
			}
		};

		class Condition : public Decorator
		{
			std::function<bool()> m_condition;
		public:
			Condition() : Decorator() {}
			Condition(NodePtr task, std::function<bool()> condition) : Decorator(task) { m_condition = condition; }

			virtual bool tick() {
				if (m_condition)
					if (m_condition())
						return SECURE_TASK(m_task);
				return false;
			}
		};

	}

#pragma endregion
	
	namespace ActionNode
	{
		class NodeFunc : public Node
		{
			std::function<bool()> m_tick;
		public:
			NodeFunc(std::function<bool()> tick) { m_tick = tick; }
			virtual bool tick() { return (m_tick ? m_tick() : false); }
		};
	}

	
}

