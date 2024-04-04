#pragma once
#include <iostream>
#include <list>
#include <memory>
#include <functional>

class Node
{
public:
	virtual bool tick() = 0;
	template<typename T>
	static std::shared_ptr<T> New(T node) { return std::make_shared<T>(node); }
};

typedef std::shared_ptr<Node> NodePtr;
typedef std::list<std::shared_ptr<Node>> NodeList;

#pragma region COMPOSITE


class CompositeNode : public Node
{
protected:
	NodeList m_childList;
public:
	NodeList& getChilds() { return m_childList; }
	template <typename T>
	std::shared_ptr<T> addChild(std::shared_ptr<T> child) { m_childList.push_back(child); return child; }
};

class Selector : public CompositeNode
{
public:
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


#pragma endregion

#pragma region DECORATOR



class Decorator : public Node
{
protected:
	NodePtr m_task;
public:
	Decorator(NodePtr task) { m_task = task; }
};

class Inverser : public Decorator
{
public:
	Inverser(NodePtr task) : Decorator(task) {}
	virtual bool tick() { return !m_task->tick(); }
};

class Loop : public Decorator
{
	int m_loopNbr;
public:
	Loop(NodePtr task, int loop) : Decorator(task) { m_loopNbr = loop; }
	virtual bool tick() {
		if (m_loopNbr)
			for (int i = 0; i < m_loopNbr; i++)
			{
				if (m_task->tick())
					return true;
			}
		else while (!m_task->tick()) {};
		
		return false;
	}
};

class Cooldown : public Decorator
{
	float m_executionTimer;
	float m_timer;
public:
	Cooldown(NodePtr task, float timer) : Decorator(task) { m_timer = 0.f; m_executionTimer = timer; }
	virtual bool tick() {
		if (m_timer > m_executionTimer)
		{
			m_timer = 0.f;
			return m_task->tick();
		}
		return false;
	}
};

class Condition : public Decorator
{
	std::function<bool()> m_condition;
public:
	Condition(NodePtr task, std::function<bool()> condition) : Decorator(task) { m_condition = condition; }
	virtual bool tick() {
		if (m_condition)
			if (m_condition())
				return m_task->tick();
		return false;
	}
};

#pragma endregion


class NodeFunc : public Node
{
	std::function<bool()> m_tick;
public:
	NodeFunc(std::function<bool()> tick) { m_tick = tick; }
	virtual bool tick() { return m_tick(); }
};