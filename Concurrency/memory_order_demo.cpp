
#include <atomic>
#include <thread>
#include <print>

std::atomic<int>  data{0};
std::atomic<bool> flag{false};
std::atomic<int>  bugs{0};

// ── RELAXED (broken) ─────────────────────────────────────────────────────────
// no guarantees: CPU/compiler can reorder data.store and flag.store
void producer_relaxed(int val) {
    data.store(val, std::memory_order_relaxed);  // may appear AFTER flag store
    flag.store(true, std::memory_order_relaxed);
}

void consumer_relaxed(int expected) {
    while (!flag.load(std::memory_order_relaxed)) {}
    int val = data.load(std::memory_order_relaxed); // may still see 0!
    if (val != expected) {
        bugs.fetch_add(1, std::memory_order_relaxed);
        std::println("  BUG: expected {}, got {}", expected, val);
    }
}

// ── ACQUIRE / RELEASE (correct) ──────────────────────────────────────────────
// release on store: everything above it is flushed before the store is visible
// acquire on load:  everything below it waits until the load completes
void producer_correct(int val) {
    data.store(val, std::memory_order_relaxed);
    flag.store(true, std::memory_order_release); // RELEASE WALL ↑
}

void consumer_correct(int expected) {
    while (!flag.load(std::memory_order_acquire)) {} // ACQUIRE WALL ↓
    int val = data.load(std::memory_order_relaxed);  // guaranteed to see expected
    if (val != expected)
        std::println("  BUG (should never happen): expected {}, got {}", expected, val);
}

void run(const char* label, void(*prod)(int), void(*cons)(int), int iterations) {
    bugs.store(0);
    for (int i = 1; i <= iterations; i++) {
        data.store(0);
        flag.store(false);
        std::thread p(prod, i);
        std::thread c(cons, i);
        p.join();
        c.join();
    }
    std::println("[{}] bugs found: {} / {}", label, bugs.load(), iterations);
}

int main() {
    constexpr int N = 100'000;
    std::println("Running {} iterations each...\n", N);
    run("relaxed ", producer_relaxed, consumer_relaxed, N);
    run("acq_rel ", producer_correct, consumer_correct, N);
}