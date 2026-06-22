#include <string>
#include "../enums/Symbol.h"

class Player {
private:
	const std::string name_;
	const Symbol symbol_;

private:
	Player(const std::string& name, Symbol symbol): name_(name), symbol_(symbol) {
		if (symbol == Symbol::EMPTY) {
			throw std::invalid_argument("Player cannot have EMPTY symbol");
		}
	}

	
};