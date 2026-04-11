#include <memory>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

template <typename T>
struct DataWrapper
{
	T data{ };
	bool is_last_chunk{ false };
};

template <typename T, typename Callback>
class SPSC
{
public:
	SPSC(Callback callback) : on_consume(callback), consumer{[this]{Consume();}}
	{
		// Implement
	}
	SPSC(const SPSC& other) = delete;
	SPSC& operator=(const SPSC& other) = delete;
	SPSC(const SPSC&& other) = delete;
	SPSC&& operator=(SPSC&& other) = delete;
	void PushWork(const DataWrapper<T>& wrapper)
	{
		std::scoped_lock lk(mtx);
		queue_.push(wrapper);
		data_cond.notify_one();
	}
	~SPSC() {
		shutdown_.store(true, std::memory_order_relaxed);
		PushWork({0, true});
		consumer.join();
	}

private:
	Callback on_consume;
	std::queue<DataWrapper<T>> queue_;
	std::mutex mtx;
	std::condition_variable data_cond;
	std::thread consumer;
	std::atomic<bool> shutdown_;
	void Consume()
	{
		while (true) {
			std::unique_lock<std::mutex> lock(mtx);
			if (queue_.empty()) {
				data_cond.wait(lock, [this]{return !queue_.empty();});
			}
			auto consumed = queue_.front();
			queue_.pop();

			on_consume(consumed.data);
			if (consumed.is_last_chunk) {
				break;
			}
		}
	}
};

int main() {
	/*
	std::atomic<bool> called{ false };
	auto callback = [&called](int)
	{
		std::cout << "Hello!" << std::endl;
		called.store(true, std::memory_order_relaxed);
	};

	{
		SPSC<int, decltype(callback)> q(callback);
		q.PushWork({42, false});
		q.PushWork({0,  true}); // poison-pill, signals shutdown.
	}

	while (!called.load(std::memory_order_relaxed)) {
		std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
	}

	std::cout << "We said 'Hello'!" << std::endl;
	*/

	auto callback = [](int x) {
		std::cout << "Got: " << x << "\n";
	};

	{
		SPSC<int, decltype(callback)> q(callback);
		q.PushWork({42, false});
		q.PushWork({99, false});
		// No poison pill pushed!
		// Destructor runs here...
	}

	std::cout << "Done\n"; // never reaches here

	return 0;
}