//
// Created by Shreyansh Rathore on 25/02/2026.
//
#include <string>
#include <iostream>

int main() {
	std::string key = "";
	int freq = 2;
	key += '#' + freq;
	std::cout << "key: [" << key << "]  length: " << key.size() << std::endl;
}