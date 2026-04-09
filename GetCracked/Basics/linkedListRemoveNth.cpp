#include <iostream>
#include <memory>
#include <print>

struct ListNode {
	int val;
	ListNode* next;
	ListNode(int x, ListNode* next = nullptr) : val(x), next(next) {}
	~ListNode() {
		std::println("~ListNode({}) destroyed", val);
	}
};

int main() {
	ListNode* n3 = new ListNode(3);
	ListNode* n2 = new ListNode(2, n3);
	ListNode* n1 = new ListNode(1, n2);

	{
		auto dummy = std::make_unique<ListNode>(0, n1);
		std::println("Dummy addr: {}", (void*)dummy.get());
		std::println("Dummy->next addr: {}", (void*)dummy->next);
		std::println("n1 addr: {}", (void*)n1);
	}

	std::println("After Scope n1->val = {}", n1->val);

	delete n1; delete n2; delete n3;
}