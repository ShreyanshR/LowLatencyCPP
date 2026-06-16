#include <iostream>
#include <ostream>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <print>

class Solution {
public:
	int longestConsecutive(std::vector<int>& nums) {
		std::unordered_set<int> numSet(nums.begin(), nums.end());
		int longest{};
		/*for (auto& num: numSet) {
			std::cout << num << std::endl;
		}*/

		for (auto& num: numSet) {
			if (!numSet.contains(num - 1)) {
				int length = 1;
				while (numSet.contains(num + length)) {
					length++;
				}
				longest = longest > length ? longest : length;
			}
		}

		return longest;
	}
};

int main() {
	Solution sol;

	// Pattern: build an input, call the function, assert the expected result.
	// assert() aborts and prints the failing line if the condition is false.
	std::vector<int> nums{2, 20, 4, 10, 3, 4, 5};
	assert(sol.longestConsecutive(nums) == 4);   // streak: 2,3,4,5

	std::vector<int> empty{};
	assert(sol.longestConsecutive(empty) == 0);  // edge case: no elements

	std::vector<int> one{42};
	assert(sol.longestConsecutive(one) == 1);    // edge case: single element

	std::vector<int> dupes{1, 2, 2, 3};
	assert(sol.longestConsecutive(dupes) == 3);  // duplicates don't extend streak

	std::println("All tests passed!");
	return 0;
}