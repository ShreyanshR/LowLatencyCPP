#include <iostream>
#include <string>

class Chair {
private:
	int number_of_legs;
	std::string colour;

public:
	virtual ~Chair(){};
	virtual std::string createChair() const = 0;
};

class VictorianChair: public Chair {
public:
	std::string createChair() const override {
		return "Creating Victorian Chair";
	}
};

class ModernChair: public Chair {
public:
	std::string createChair() const override {
		return "Creating Modern Chair";
	}
};

class Sofa {
public:
	virtual ~Sofa(){};
	virtual std::string createSofa() const = 0;
};

class VictorianSofa: public Sofa {
public:
	std::string createSofa() const override {
		return "Creating Victorian Sofa";
	}
};

class ModernSofa: public Sofa {
public:
	std::string createSofa() const override {
		return "Creating Modern Sofa";
	}
};

class Furniture {
public:
	virtual ~Furniture(){};
	virtual Chair *createChair() const = 0;
	virtual Sofa *createSofa() const = 0;
};

class VictorianFurniture: public Furniture {
public:
	Chair *createChair() const override {
		return new VictorianChair();
	}
	Sofa *createSofa() const override {
		return new VictorianSofa();
	}
};

class ModernFurniture: public Furniture {
public:
	Chair *createChair() const override {
		return new ModernChair();
	}
	Sofa *createSofa() const override {
		return new ModernSofa();
	}
};

void clientGUI(const Furniture& furniture) {
	const Chair *chairA = furniture.createChair();
	const Sofa *sofaA = furniture.createSofa();
	std::cout << chairA->createChair() << std::endl;
	std::cout << sofaA->createSofa() << std::endl;

	delete chairA;
	delete sofaA;
}

int main() {
std::cout << "Client: Testing code\n";
	VictorianFurniture *f1 = new VictorianFurniture();
	clientGUI(*f1);
	delete f1;

	ModernFurniture *s1 = new ModernFurniture();
	clientGUI(*s1);
	delete s1;

	return 0;
}