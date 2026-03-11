#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
	std::string filename = argv[argc - 1];
	std::string flag = argv[1];
	std::cout << "filename: " << filename << " flag: " << flag << "\n";

	if (argc < 2) {
		std::cerr << "Please provide a file name\n";
		return 1;
	}

	std::ifstream file;
	std::wifstream wFile;
	if (argc == 3) {
		file.open(filename);
		wFile.open(filename);
		wFile.imbue(std::locale(""));
		if (!file.is_open()) {
			std::cerr << "File not open!\n";
			return 1;
		}
	}
	bool hasFlag = argc == 3 || (argc == 2 && flag.starts_with('-'));
	std::istream& input = (hasFlag && argc == 2) ? std::cin : file;
	//file.seekg(0, std::ios::end); //get to the end of file, so that tellg can get size
	//auto size = file.tellg();
	//file.seekg(0);

	char c;
	int newlines{};
	int wordC{};
	bool inWord{};
	int byteCount{};
	wchar_t charC;
	int chars{};

	while (wFile.get(charC)) {
		chars++;
	}

	while (input.get(c)) {
		byteCount++;
		if (c == '\n') {
			newlines++;
		}
		if (std::isspace(c)) {
			inWord = false;
		} else {
			if (!inWord) {
				wordC++;
			}
			inWord = true;
		}
	}

	if (argc == 2 && !flag.starts_with('-')) {
		std::cout << newlines << " " << wordC << " " << byteCount << "\n";
	} else if(flag == "-c") {
		std::cout << byteCount << "\n";
	} else if (flag == "-l") {
		std::cout << newlines << "\n";
	} else if (flag == "-w") {
		std::cout << wordC << "\n";
	}else if (flag == "-m") {
		std::cout << chars << "\n";
	} else {
		std::cerr << "Unknown Flag: " << flag << "\n";
		return 1;
	}
}