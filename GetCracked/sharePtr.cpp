#include <atomic>
#include <cassert>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

template <typename T>
class shared_ptr {
private:
	T* m_ptr;
	std::atomic<int>* m_ref_count;
public:
	shared_ptr(T* ptr) : m_ptr(ptr), m_ref_count(new std::atomic<int>(1)) {}
	shared_ptr(shared_ptr const& other) : m_ptr(other.m_ptr), m_ref_count(other.m_ref_count) {
		m_ref_count->fetch_add(1);
	}
	shared_ptr& operator=(shared_ptr const& other) {
			if (this != other) {
				if (m_ref_count->fetch_sub(1) == 1) {
					delete m_ptr;
					delete m_ref_count;
				}
				m_ptr = other.m_ptr;
				m_ref_count = other.m_ref_count;
				m_ref_count->fetch_add(1);
			}
		return *this;
	}
	~shared_ptr() {
		if (m_ref_count->fetch_sub(1) == 1) {
			delete m_ptr;
			delete m_ref_count;
		}
	}

	T*  get() const noexcept{ return m_ptr;}
	size_t use_count() const noexcept{return *m_ref_count;}

};

int main() {
	shared_ptr<int> const ptr{new int{0}};
	size_t const num_thread{std::thread::hardware_concurrency()};
	std::vector<std::thread> threads(num_thread);

	std::mutex mtx{};
	for (auto& thread: threads) {
		thread = std::thread([ptr, &mtx]() {
			std::lock_guard<std::mutex> lock{mtx};
			++(*ptr.get());
		});
	}

	for (auto& thread: threads) {
		thread.join();
	}

	assert(*ptr.get() == num_thread);

	return 0;
}