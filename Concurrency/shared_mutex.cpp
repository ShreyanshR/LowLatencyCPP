#include <iostream>
#include <map>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <print>
#include <functional>

/*
class dns_entry;
class dns_cache {
	std::map<std::string, dns_entry> entries;
	mutable std::shared_mutex entry_mutex;

public:
	dns_entry find_entry(std::string const& domain) const {
		std::shared_lock<std::shared_mutex> lk(entry_mutex);
		std::map<std::string, dns_entry>::const_iterator const it =
			entries.find(domain);
		return (it==entries.end()) ? dns_entry() : it->second;
	}

	void update_or_add_entry(std::string const& domain, dns_entry const& dns_details) {
		std::lock_guard<std::shared_mutex> lk(entry_mutex);
		entries[domain] = dns_details;
	}
};

*/
class hierarchical_mutex {
	std::mutex internal_mutex;
	unsigned long const hierarchy_value;
	unsigned long previous_hierarchy_value;
	static thread_local unsigned long this_thread_hierarchy_value;
	void check_for_hierarchy_violation() {
		if (this_thread_hierarchy_value <= hierarchy_value) {
			throw std::logic_error("Mutex hierarchy violated");
		}
	}

	void update_hierarchy_value() {
		previous_hierarchy_value=this_thread_hierarchy_value;
		this_thread_hierarchy_value=hierarchy_value;
	}
public:
	explicit hierarchical_mutex(unsigned long value): hierarchy_value(value), previous_hierarchy_value(0)
	{}
	void lock() {
		check_for_hierarchy_violation();
		internal_mutex.lock();
		update_hierarchy_value();
	}
	void unlock() {
		if (this_thread_hierarchy_value != hierarchy_value)
			throw std::logic_error("Mutex Hierarchy violated");
		internal_mutex.unlock();
	}
	bool try_lock() {
		check_for_hierarchy_violation();
		if (!internal_mutex.try_lock())
			return false;
		update_hierarchy_value();
		return true;
	}
};

thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(ULONG_MAX);

hierarchical_mutex high_level_mutex(10000);
hierarchical_mutex low_level_mutex(5000);
hierarchical_mutex other_mutex(6000);
int do_low_level_stuff() {
	return 5000;
}

int low_level_func() {
	std::lock_guard<hierarchical_mutex> lk(low_level_mutex);
	return do_low_level_stuff();
}

void high_level_stuff(int some_param) {
	some_param++;
}
void high_level_func() {
	std::lock_guard<hierarchical_mutex> lk(high_level_mutex);
	high_level_stuff(low_level_func());
}
void thread_a() {
	high_level_func();
}
void do_ohter_stuff() {
	std::cout << "Do other stuff" << std::endl;
}
void other_stuff() {
	high_level_func();
	do_ohter_stuff();
}

void thread_b() {
	std::lock_guard<hierarchical_mutex> lk(other_mutex);
	other_stuff();
}

std::mutex m, m1;

void increase_lock(int& counter) {
	m.lock();
	for (int i = 0; i < 100000; i++) {
		counter++;
		if (counter == 50000)
			return;
			//throw std::logic_error("just throwing");
	}
	m.unlock();
	std::cout << "Counter from lock: "<< counter << std::endl;
}
void increase_with_lock_guard(int& counter) {
	std::lock_guard<std::mutex> lk(m);
	std::cout << "Init Value: " << counter << "\n";
	for (int i = 0; i < 100000; i++) {
		counter++;
		if (counter == 50000)
			return;
			//throw std::logic_error("just throwing from lock guard");
	}
	std::cout << "Counter from lock guard: "<< counter << std::endl;
}

void increase_with_scoped_lock(int& counter) {
	std::scoped_lock lk(m, m1);
	std::cout << "Init Value: " << counter << "\n";
	for (int i = 0; i < 100000; i++) {
		counter++;
		if (counter == 50000)
			return;
			//throw std::logic_error("just throwing from scoped lock");
	}
	std::cout << "Counter from scoped lock: "<< counter << std::endl;
}

int accA = 1000;
int accB = 1000;
void transfer_a_to_b(int amount) {
	std::scoped_lock lk(m, m1);

	//std::lock_guard<std::mutex> lk(m);
	//std::lock_guard<std::mutex> lk1(m1);
	accA -= amount;
	accB += amount;
}

void transfer_b_to_a(int amount) {
	//std::scoped_lock lk(m, m1);
	std::lock_guard<std::mutex> lk1(m1);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::lock_guard<std::mutex> lk(m);
	accB -= amount;
	accA += amount;
}

int main() {
	//low_level_func();
	//high_level_func();
	//thread_a();
	//thread_b();
	/*
	int counter = 0;
	std::thread t1(increase_lock, std::ref(counter));
	std::thread t2(increase_lock, std::ref(counter));

	t1.join();
	t2.join();

	std::cout << "Counter: " << counter << "\n";
*/
	int c1 = 0;
	std::thread t3(increase_with_lock_guard, std::ref(c1));
	std::thread t4(increase_with_lock_guard, std::ref(c1));

	t3.join();
	t4.join();

	std::cout << "Counter: " << c1 << "\n";

	int c2 = 0;
	std::thread t5(increase_with_lock_guard, std::ref(c2));
	std::thread t6(increase_with_lock_guard, std::ref(c2));

	t5.join();
	t6.join();

	std::cout << "Counter: " << c1 << "\n";


	std::thread t7([&]() {
		for (int i = 0; i < 100000; i++) {
			transfer_a_to_b(1);
		}
	});
	std::thread t8([&]() {
		for (int i = 0; i < 100000; i++) {
			transfer_b_to_a(1);
		}
	});

	t7.join();
	t8.join();

	std::cout << "Amount A: " << accA << "\n";
	std::cout << "Amount B: " << accB << "\n";

	std::println("Total Amount: {}", accA + accB);
	return 0;
}