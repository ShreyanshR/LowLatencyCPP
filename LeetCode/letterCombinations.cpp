#include <ostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <print>

using std::vector;
using std::string;
using std::unordered_map;

class Solution {
public:
	vector<string> letterCombinations(string digits) {
		unordered_map<char, string> mapping{
			{'2', "abc"},
			{'3',"def"},
			{'4', "ghi"},
			{'5', "jkl"},
			{'6', "mno"},
			{'7', "pqrs"},
			{'8', "tuv"},
			{'9', "wxyz"}
	};
		vector<string> res;
		if (!digits.empty()) {
			dfs(0, "", digits, mapping, res);
		}

		std::println("res: {}", res);
		return res;
	}

	void dfs(int i, string curStr, string& digits, unordered_map<char, string>& charToDigits, vector<string>& res) {
		if (curStr.size() == digits.size()) {
			//base case when curStr is the same size as digits
			res.push_back(curStr);
			return;
		}

		for (auto& c: charToDigits[digits[i]]) {
			std::println("i:{}, curStr:{}, res: {}", i, curStr, res);
			dfs(i+1, curStr + c, digits, charToDigits, res);
		}
	}
};



int main() {
	const string digits = "34";
	Solution so;
	so.letterCombinations(digits);


	return 0;
}