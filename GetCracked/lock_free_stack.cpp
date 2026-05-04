#include <atomic>
#include <memory>
#include <print>


template <typename T>
class lock_free_stack {
private:
	struct node {
		T data;
		node* next;
		node(T const& data_) : data(data_) {}
	};
	std::atomic<node*> head;

public:
	void push(T const& data) {
		node* const new_node = new node(data);
		new_node->next = head.load();
		while (!head.compare_exchange_weak(new_node->next, new_node)); //if head != new_node->next then it sets it to new_node
	}

	void pop(T& result) {
		node* old_head = head.load();
		while (!head.compare_exchange_weak(old_head, old_head->next));
		result = old_head->data;
	}
};

template <typename T>
class lock_free_stack_smart {
private:
	struct node {
		std::shared_ptr<T> data;
		node* next;
		node(T const& data_) : data(std::make_shared<T>(data_)) {}
	};
	std::atomic<node*> head;
	std::atomic<unsigned> threads_in_pop;
	void try_reclaim(node* old_head);3

public:
	void push(T const& data) {
		node* const new_node = new node(data);
		new_node->next = head.load();
		while (!head.compare_exchange_weak(new_node->next, new_node));
	}
	std::shared_ptr<T> pop() {
		++threads_in_pop;
		node* old_head = head.load();
		while (old_head && !head.compare_exchange_weak(old_head, old_head->next)); //check if old_head is not null ptr
		std::shared_ptr<T> res;
		if (old_head) {
			res.swap(old_head->head);
		}
		try_reclaim(old_head);

		return res;

		//return old_head ? old_head->data : std::shared_ptr<T>(); //if it's null return null otherwise data
	}
};