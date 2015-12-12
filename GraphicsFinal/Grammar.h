
#ifndef GraphicsFinal_Grammar_h
#define GraphicsFinal_Grammar_h

#include <string>
#include <vector>
#include <unordered_map>

struct Production {
	char predecessor;
	std::string successor;
};

class Grammar {
private:
	std::string _axiom;
	std::vector<Production> _productions;
	std::unordered_map<char, Production> _productionsMap;

	std::string derivedString = "";

	void generateProductionsMap();
public:
	Grammar(std::string axiom, std::vector<Production> productions) {
		_axiom = axiom;
		_productions = productions;
		generateProductionsMap();
	}

	std::string runDerivation();
	std::string getDerived();
};

#endif
