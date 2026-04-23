#include <future>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <string>

// ─── INTERRUPT MECHANISM ───────────────────────────────────────────────────

class thread_interrupted {};
void interruption_point();

class interrupt_flag {
    std::atomic<bool> flag;
    std::condition_variable* thread_cond;
    std::condition_variable_any* thread_cond_any;
    std::mutex set_clear_mtx;
public:
    interrupt_flag() : flag(false), thread_cond(nullptr), thread_cond_any(nullptr) {}

    void set() {
        flag.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lk(set_clear_mtx);
        if (thread_cond)          thread_cond->notify_all();
        else if (thread_cond_any) thread_cond_any->notify_all();
    }

    bool is_set() const { return flag.load(std::memory_order_relaxed); }

    void set_condition_variable(std::condition_variable& cv) {
        std::lock_guard<std::mutex> lk(set_clear_mtx);
        thread_cond = &cv;
    }

    void clear_condition_variable() {
        std::lock_guard<std::mutex> lk(set_clear_mtx);
        thread_cond = nullptr;
    }

    struct clear_cv_on_destruct {
        interrupt_flag* flag;
        clear_cv_on_destruct(interrupt_flag* f) : flag(f) {}
        // RAII: even if we leave via exception, cv pointer is always cleared
        ~clear_cv_on_destruct() { flag->clear_condition_variable(); }
    };
};

// Every thread gets its own private interrupt_flag (thread_local = one copy per thread)
thread_local interrupt_flag this_thread_interrupt_flag;

void interruption_point() {
    if (this_thread_interrupt_flag.is_set())
        throw thread_interrupted{};
}

// ─── INTERRUPTIBLE WAIT ────────────────────────────────────────────────────

void interruptible_wait(std::condition_variable& cv,
                        std::unique_lock<std::mutex>& lk,
                        auto predicate)
{
    // Check #1: was interrupt() already called before we even got here?
    interruption_point();

    this_thread_interrupt_flag.set_condition_variable(cv);
    interrupt_flag::clear_cv_on_destruct guard(&this_thread_interrupt_flag);

    // Check #2: did interrupt() fire in the tiny window between registering
    //           the cv pointer and entering wait? Without this, we'd sleep forever
    //           because the notify_all() already happened and we missed it.
    interruption_point();

    // Wait until real data arrives OR the interrupt flag is set.
    // The compound predicate means interrupt() + notify_all() will wake us here.
    cv.wait(lk, [&]{
        return predicate() || this_thread_interrupt_flag.is_set();
    });

    // Check #3: we woke up — but was it because of an interrupt, not real data?
    interruption_point();
}

// ─── INTERRUPT_THREAD ──────────────────────────────────────────────────────

class interrupt_thread {
    std::thread internal_thread;
    interrupt_flag* flag;
public:
    template<typename FunctionType>
    interrupt_thread(FunctionType f) {
        std::promise<interrupt_flag*> p;

        // Worker's first job: publish its own thread_local flag address (walkie-talkie channel)
        internal_thread = std::thread([f, &p] {
            p.set_value(&this_thread_interrupt_flag);
            try {
                f();
            } catch (thread_interrupted&) {
                // Cook received the stop signal — put down the knife, exit cleanly
                std::cout << "[worker]  caught thread_interrupted — exiting cleanly\n";
            }
        });

        // Caller blocks here until the worker publishes its flag address
        flag = p.get_future().get();
    }

    void interrupt() { if (flag) flag->set(); }
    void join()      { internal_thread.join(); }
};

// ─── SHARED STATE (simulates incoming market data) ─────────────────────────

std::mutex              data_mutex;
std::condition_variable data_cv;
std::string             latest_price;
bool                    new_data_available = false;

// ─── WORKER FUNCTION: feed handler waiting for market ticks ───────────────

void feed_handler() {
    int ticks = 0;
    while (true) {
        std::unique_lock<std::mutex> lk(data_mutex);

        std::cout << "[worker]  blocking — waiting for market data...\n";

        // Truly sleeps here until: (a) real data arrives, or (b) interrupt fires
        interruptible_wait(data_cv, lk, []{ return new_data_available; });

        // If we get here without throwing, real data arrived
        std::string price = latest_price;
        new_data_available = false;   // consume the tick
        lk.unlock();

        std::cout << "[worker]  processing tick #" << ++ticks << ": " << price << "\n";

        // Also check between ticks — interrupt could fire while we're processing
        interruption_point();
    }
}

// ─── MAIN ──────────────────────────────────────────────────────────────────

int main() {
    std::cout << "[main]    starting feed handler...\n\n";
    interrupt_thread worker(feed_handler);

    // Helper to simulate market data arriving
    auto send_tick = [](std::string price) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        {
            std::lock_guard<std::mutex> lk(data_mutex);
            latest_price       = price;
            new_data_available = true;
        }
        std::cout << "\n[main]    sending market tick: " << price << "\n";
        data_cv.notify_all();
    };

    send_tick("BTC $62,400");
    send_tick("BTC $62,450");
    send_tick("BTC $62,380");

    // End of trading day — shut down the feed handler
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    std::cout << "\n[main]    market closed — calling interrupt()...\n";
    worker.interrupt();
    data_cv.notify_all(); // wake the worker so it sees the interrupt flag immediately

    worker.join();
    std::cout << "[main]    feed handler shut down cleanly.\n";
    return 0;
}
