
#include "Grammar.h"

void Grammar::generateProductionsMap() {
	_productionsMap = std::unordered_map<char, Production>();
	for (int i = 0; i < _productions.size(); i++) {
		Production production = _productions[i];
		_productionsMap.emplace(production.predecessor, production);
	}
}

std::string Grammar::getDerived() {
	return derivedString;
}

std::string Grammar::runDerivation() {
	std::string currentAxiom = derivedString;
	if (derivedString == "") {
		currentAxiom = _axiom;
	}

	// First pass to get new array length
	int newDerivedLength = 0;
	for (int i = 0; i < currentAxiom.length(); i++) {
		if (_productionsMap.count(currentAxiom[i])) {
			Production p = _productionsMap.find(currentAxiom[i])->second;
			newDerivedLength += p.successor.length();
		}
		else {
			newDerivedLength += 1;
		}
	}

	// Second pass to actually create new string
	char* newDerivedString = new char[newDerivedLength + 1];
	newDerivedString[newDerivedLength] = '\0';
	int successorOffset = 0;
	for (int i = 0; i < currentAxiom.length(); i++) {
		if (_productionsMap.count(currentAxiom[i])) {
			Production p = _productionsMap.find(currentAxiom[i])->second;
			
			for (int j = 0; j < p.successor.length(); j++) {
				newDerivedString[successorOffset + j] = p.successor[j];
			}
			successorOffset += p.successor.length();
		}
		else {
			newDerivedString[successorOffset] = currentAxiom[i];
			successorOffset += 1;
		}
	}

	derivedString = std::string(newDerivedString);
	delete [] newDerivedString;

	return derivedString;
}