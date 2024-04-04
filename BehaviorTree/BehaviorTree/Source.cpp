#include "BehaviorTree.h"



struct Fridge
{
	int beer = 10;
	bool isOpen = false;
};


int main()
{
	Fridge f;
	auto root = Node::New(Sequence());
	//auto openFridge = root->addChild(Node::New(Selector()));
	//openFridge->addChild(Node::New(FridgeIsOpen(&f)));
	//openFridge->addChild(Node::New(OpenFridge(&f)));
	//auto takeABeer = root->addChild(Node::New(Selector()));
	//takeABeer->addChild(Node::New(TakeABeer(&f)));
	//takeABeer->addChild(Node::New(CloseFridge(&f)));
	//root->addChild(Node::New(CloseFridge(&f)));

	auto openFridge = root->addChild(Node::New(Selector()));

	openFridge->addChild(Node::New(Inverser(Node::New(NodeFunc([&f] {
		if (f.isOpen)
			return true;
		return false;
		})))));
	openFridge->addChild(Node::New(NodeFunc([&f] {
		f.isOpen = true;
		return true;
		})));
	auto takeBeer = root->addChild(Node::New(Selector()));
	takeBeer->addChild(Node::New(NodeFunc([&f] {
		if (f.beer > 0)
		{
			f.beer--;
			return true;
		}
		return false;
		})));
	takeBeer->addChild(Node::New(NodeFunc([&f] {
		f.isOpen = false;
		return true;
		})));
	root->addChild(Node::New(NodeFunc([&f] {
		if(f.isOpen)
		{
			f.isOpen = false;
			return true;
		}
		return false;
		})));


	while (root->tick())
		std::cout << "Etat du frigo : " << (!f.isOpen ? "Fermer" : "Ouvert") << "\nNombre de bière : " << f.beer << "\n";

	std::cout << "Etat du frigo : " << (!f.isOpen ? "Fermer" : "Ouvert") << "\nNombre de bière : " << f.beer << "\n";
}