#include "BehaviorTree.h"

using namespace bt;

enum NODETYPE { NONE = -1, SEQUENCE, SELECTOR, ALTERNATE, INVERSER, CONDITION, LOOP, COOLDOWN, OPEN_FRIDGE, FORCE_FRIDGE, TAKE_BEER, CLOSE_FRIDGE };

class PatronNode
{
	NODETYPE m_type;
	std::list<PatronNode> m_child;
	PatronNode* m_parent;
public:
	inline static std::unordered_map<NODETYPE, std::string> m_idToString;

	PatronNode(NODETYPE type) : m_type(type) {};

	PatronNode& add(const PatronNode& node) {
		if (node.m_type != NONE and this->m_type <= COOLDOWN)
		{
			m_child.push_back(node);
			m_child.back().m_parent = this;
			return m_child.back();
		}
		else
		{
			std::cout << "Invalid Node Type" << std::endl;
			return *this;
		}
	}

	NODETYPE getType() { return m_type; }
	std::list<PatronNode> getChildren() { return m_child; }
	PatronNode* getParent() { return m_parent; }

	void display(unsigned int space) {
		for (int i = 0; i < space; i++) std::cout << " ";
			std::cout << m_idToString[m_type] << std::endl;
			for (auto& child : m_child) child.display(space + 2);
	}
};


NodePtr NodeFactory(NODETYPE type)
{
	switch (type)
	{
	case NONE:
		break;
	case SEQUENCE:
		return Node::New(bt::Composite::Sequence());
		break;
	case SELECTOR:
		return Node::New(bt::Composite::Selector());;
		break;
	case ALTERNATE:
		return Node::New(bt::Composite::Alternate());
		break;
	case INVERSER:
		return Node::New(bt::Decorator::Inverser());
		break;
	case CONDITION:
		return Node::New(bt::Decorator::Condition());
		break;
	case LOOP:
		return Node::New(bt::Decorator::Loop());
		break;
	case COOLDOWN:
		return Node::New(bt::Decorator::Cooldown());
		break;
	case OPEN_FRIDGE:
		return Node::New(bt::ActionNode::NodeFunc([]() {std::cout << "Open Fridge" << std::endl; return true; }));
		break;
	case FORCE_FRIDGE:
		return Node::New(bt::ActionNode::NodeFunc([]() {std::cout << "Force Fridge" << std::endl; return true; }));
		break;
	case TAKE_BEER:
		return Node::New(bt::ActionNode::NodeFunc([]() {std::cout << "Take Beer" << std::endl; return true; }));
		break;
	case CLOSE_FRIDGE:
		return Node::New(bt::ActionNode::NodeFunc([]() {std::cout << "Close Fridge" << std::endl; return true; }));
		break;
	default:
		return Node::New(bt::Composite::Sequence());
		break;
	}
}

void CreateBehaviorTree(NodePtr bt, PatronNode& patron);

NodePtr InitBehavior(PatronNode& patron)
{
	NodePtr node;

	node = NodeFactory(patron.getType());

	for (auto& i : patron.getChildren())
	{
		CreateBehaviorTree(node, i);
	}

	return node;
}


void CreateBehaviorTree(NodePtr bt, PatronNode& patron)
{
	NodePtr* node = nullptr;
	if(patron.getParent() == nullptr)
		*node = bt;
	else
	{
		if (patron.getParent()->getType() < 3)
			node = std::dynamic_pointer_cast<bt::Composite::CompositeNode>(bt)->addChild_INTERNAL_USE(Node::New(bt::Composite::Selector()));
		else if (patron.getParent()->getType() < 7)
			node = std::dynamic_pointer_cast<bt::Decorator::Decorator>(bt)->setTask_INTERNAL_USE(Node::New(bt::Composite::Sequence()));
		else 
		{
			std::cerr << "Invalid Node Type" << std::endl;
			return;
		}
	}

	*node = NodeFactory(patron.getType());

	for (auto& i : patron.getChildren())
	{
		CreateBehaviorTree(*node, i);
	}
}



int main()
{
	PatronNode::m_idToString[NONE] = "NONE";
	PatronNode::m_idToString[SEQUENCE] = "SEQUENCE";
	PatronNode::m_idToString[SELECTOR] = "SELECTOR";
	PatronNode::m_idToString[ALTERNATE] = "ALTERNATE";
	PatronNode::m_idToString[INVERSER] = "INVERSER";
	PatronNode::m_idToString[CONDITION] = "CONDITION";
	PatronNode::m_idToString[LOOP] = "LOOP";
	PatronNode::m_idToString[COOLDOWN] = "COOLDOWN";
	PatronNode::m_idToString[OPEN_FRIDGE] = "OPEN_FRIDGE";
	PatronNode::m_idToString[FORCE_FRIDGE] = "FORCE_FRIDGE";
	PatronNode::m_idToString[TAKE_BEER] = "TAKE_BEER";
	PatronNode::m_idToString[CLOSE_FRIDGE] = "CLOSE_FRIDGE";
		

	PatronNode root(SEQUENCE);
	auto &Selector_Open = root.add(PatronNode(SELECTOR));
	Selector_Open.add(PatronNode(OPEN_FRIDGE));
	Selector_Open.add(PatronNode(FORCE_FRIDGE));

	auto& Selector_Beer = root.add(PatronNode(SELECTOR));
	auto& Condition_Beer = Selector_Beer.add(PatronNode(CONDITION));
	Condition_Beer.add(PatronNode(TAKE_BEER));
	Selector_Beer.add(PatronNode(CLOSE_FRIDGE));
	root.add(PatronNode(CLOSE_FRIDGE));


	auto root_bt = InitBehavior(root);


	root_bt->tick();



	root.display(0);
}