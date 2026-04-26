#include <atomic>
#include <print>

int main() {
	std::atomic_bool flag(false);
	std::println("After init    — flag: {}", flag.load());

	bool x = flag.load(std::memory_order_acquire);
	std::println("After load    — flag: {}, x: {}", flag.load(), x);

	flag.store(true);
	std::println("After store   — flag: {}", flag.load());

	x = flag.exchange(false, std::memory_order_acq_rel);
	std::println("After exchange — flag: {}, x (old value): {}", flag.load(), x);

	return 0;
}
