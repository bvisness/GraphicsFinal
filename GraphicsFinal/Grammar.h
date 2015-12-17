
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
	float _delta;

	std::string derivedString = "";

	void generateProductionsMap();
public:
	Grammar(std::string axiom, std::vector<Production> productions, float delta) {
		_axiom = axiom;
		_productions = productions;
		_delta = delta;
		generateProductionsMap();
	}

	std::string runDerivation();
	std::string getDerived();
	void reset();
	float getDelta();
};

#endif
