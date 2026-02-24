// Write your solution here
// C++23 using GCC 14.2
// Debug with std::cerr or std::clog.
// All headers are already included for you in the test environment.
// !!! IMPORTANT !!!
// 99% of headers are pre-compiled for you server-side.
// If your submission fails to compile due to a missing header, add it to your submission.


#include <cstddef>
#include <iostream>

namespace getcracked {
	template <typename Element>
	class vector {
	public:
		vector(): size(0), capacity(1), data(static_cast<Element *>(operator new(capacity * sizeof(Element)))){}
		~vector() {
			for (int i = 0; i < size; i++) {
				data[i].~Element();
			}
			operator delete(data);
		};
		void push_back(Element element) {
			if(size == capacity){
				resize();
			}
			new(data + size) Element(element);
			size++;
			if(size == capacity){
				resize();
			}
		}
		void resize() {
			int newCapacity = 3 * capacity;
			Element* newData = static_cast<Element *>(operator new(newCapacity * sizeof(Element)));

			for (int i = 0; i < size; i++) {
				new(newData + i) Element(data[i]);
				data[i].~Element();
			}
			operator delete(data);
			data = newData;
			capacity = newCapacity;
		}

		const Element& at(std::size_t index) const {
			if (index >= size) {
				throw std::out_of_range("Index Out of Bounds");
			}
			return data[index];
		}
		std::size_t get_size() const {
			return size;
		}
		std::size_t get_capacity() const {
			return capacity;
		}
		void shrink_to_fit() {
			capacity = size;
			Element* newData = static_cast<Element *>(operator new(capacity * sizeof(Element)));

			for (int i = 0; i < size; i++) {
				new(newData + i) Element(data[i]);
				data[i].~Element();
			}

			operator delete(data);
			data = newData;

		}
		void pop_back() {
			data[size - 1].~Element();
			size--;
		}
	private:
		int size;
		int capacity = 1;
		Element* data;
	};
}

/*
int main() {
	getcracked::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	std::cout << v.get_capacity() <<"\n";
}

*/