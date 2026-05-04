#include <atomic>
#include <array>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

template <typename T>
struct DataWrapper
{
  T data{ };
  bool is_last_chunk{ false };
};

template<typename T, size_t Size>
class SPSC_LF {
private:
  std::array<DataWrapper<T>, Size> queue_;
  std::atomic<size_t> head{0};
  std::atomic<size_t> tail{0};
public:
  SPSC_LF(){}

  SPSC_LF(const SPSC_LF& other) = delete;
  SPSC_LF& operator=(const SPSC_LF& other) = delete;
  SPSC_LF(const SPSC_LF&& other) = delete;
  SPSC_LF&& operator=(const SPSC_LF&& other) = delete;

  bool Push(const DataWrapper<T>& wrapper) {
    if ((tail.load(std::memory_order_relaxed) - head.load(std::memory_order_acquire)) >= queue_.size()) {
      return false;
    }
    queue_[tail.load(std::memory_order_relaxed) % queue_.size()] = wrapper;
    tail.fetch_add(1, std::memory_order_release);
    return true;
  }

  bool Pop(DataWrapper<T>& result) {
    if (tail.load(std::memory_order_acquire) == head.load(std::memory_order_relaxed)) {
      return false;
    }
    result = queue_[head.load(std::memory_order_relaxed) % queue_.size()];
    head.fetch_add(1, std::memory_order_release);
    return true;
  }
};

// ============================================================
// TEST 1: Basic push and pop — does data come out correctly?
// ============================================================
void test_basic_push_pop() {
  SPSC_LF<int, 4> q;

  q.Push({10, false});
  q.Push({20, false});
  q.Push({30, false});

  DataWrapper<int> result;

  q.Pop(result);
  assert(result.data == 10);

  q.Pop(result);
  assert(result.data == 20);

  q.Pop(result);
  assert(result.data == 30);

  std::cout << "[PASS] test_basic_push_pop: FIFO order preserved\n";
}

// ============================================================
// TEST 2: Pop from empty queue — should return false
// ============================================================
void test_empty_pop() {
  SPSC_LF<int, 4> q;
  DataWrapper<int> result;

  assert(q.Pop(result) == false);

  // Push one, pop one, now empty again
  q.Push({42, false});
  q.Pop(result);
  assert(q.Pop(result) == false);

  std::cout << "[PASS] test_empty_pop: empty queue returns false\n";
}

// ============================================================
// TEST 3: Full buffer — should reject pushes
// ============================================================
void test_full_buffer() {
  SPSC_LF<int, 4> q;

  assert(q.Push({1, false}) == true);
  assert(q.Push({2, false}) == true);
  assert(q.Push({3, false}) == true);
  assert(q.Push({4, false}) == true);

  // 5th push should fail — buffer is full
  assert(q.Push({5, false}) == false);

  // Pop one, now there's room for one more
  DataWrapper<int> result;
  q.Pop(result);
  assert(result.data == 1);

  // Now push should work again
  assert(q.Push({5, false}) == true);

  std::cout << "[PASS] test_full_buffer: rejects when full, accepts after pop\n";
}

// ============================================================
// TEST 4: Wraparound — the ring buffer wraps correctly
//
// This is the critical test. With capacity 4, after pushing
// and popping 4 items, tail=4 and head=4. The next push goes
// to slot 4 % 4 = 0, wrapping back to the start. If the
// modulo is wrong, this crashes or gives garbage.
// ============================================================
void test_wraparound() {
  SPSC_LF<int, 4> q;
  DataWrapper<int> result;

  // Fill and drain completely — moves head and tail to 4
  for (int i = 0; i < 4; i++) q.Push({i, false});
  for (int i = 0; i < 4; i++) {
    q.Pop(result);
    assert(result.data == i);
  }

  // Now push again — these wrap around to slots 0,1,2,3
  for (int i = 100; i < 104; i++) q.Push({i, false});
  for (int i = 100; i < 104; i++) {
    q.Pop(result);
    assert(result.data == i);
  }

  // Do it again — slots 0,1,2,3 reused a third time
  for (int i = 200; i < 204; i++) q.Push({i, false});
  for (int i = 200; i < 204; i++) {
    q.Pop(result);
    assert(result.data == i);
  }

  std::cout << "[PASS] test_wraparound: ring buffer wraps correctly across 3 cycles\n";
}

// ============================================================
// TEST 5: Concurrent producer-consumer
//
// This is the real test. One thread pushes 1,000,000 items,
// another thread pops them. At the end:
// - Every value from 0..999999 must appear exactly once
// - They must appear in order (FIFO)
// - No crashes, no garbage data
//
// If your memory orderings are wrong, this test will either
// produce wrong values, miss items, or hang.
// ============================================================
void test_concurrent() {
  constexpr int NUM_ITEMS = 1'000'000;
  SPSC_LF<int, 1024> q;

  std::thread producer([&] {
    for (int i = 0; i < NUM_ITEMS; i++) {
      while (!q.Push({i, false})) {
        // spin — buffer is full, wait for consumer
      }
    }
  });

  std::vector<int> consumed;
  consumed.reserve(NUM_ITEMS);

  std::thread consumer([&] {
    DataWrapper<int> result;
    int count = 0;
    while (count < NUM_ITEMS) {
      if (q.Pop(result)) {
        consumed.push_back(result.data);
        count++;
      }
      // else: spin — buffer is empty, wait for producer
    }
  });

  producer.join();
  consumer.join();

  // Verify: every value in order
  assert(consumed.size() == NUM_ITEMS);
  for (int i = 0; i < NUM_ITEMS; i++) {
    assert(consumed[i] == i);
  }

  std::cout << "[PASS] test_concurrent: 1M items transferred correctly between threads\n";
}

int main() {
  test_basic_push_pop();
  test_empty_pop();
  test_full_buffer();
  test_wraparound();
  test_concurrent();

  std::cout << "\n=== ALL TESTS PASSED ===\n";
  return 0;
}