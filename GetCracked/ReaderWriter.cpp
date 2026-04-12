#include <memory>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <cassert>
#include <chrono>

namespace getcracked
{
	class ReaderWriterLock
	{
	public:

		unsigned ReaderLock()
		{
			std::unique_lock<std::mutex> lk(mtx);
			++waiting_readers;
			cv_reader.wait(lk, [&](){return !active_writers && waiting_writers == 0;});
			--waiting_readers;
			const unsigned before = active_readers;
			++active_readers;
			return before;
		}

		unsigned ReaderUnlock()
		{
			std::unique_lock<std::mutex> lk(mtx);
			--active_readers;
			if (active_readers == 0 && waiting_writers > 0) {
				cv_writer.notify_one();
			}
			return active_readers;
		}

		void WriterLock()
		{
			std::unique_lock<std::mutex> lk(mtx);
			++waiting_writers;
			cv_writer.wait(lk, [&](){return !active_writers && active_readers == 0;});
			--waiting_writers;
			active_writers = true;
		}

		void WriterUnlock()
		{
			std::unique_lock<std::mutex> lk(mtx);
			active_writers = false;
			if (waiting_writers > 0) {
				cv_writer.notify_one();
			} else if (waiting_readers > 0) {
				cv_reader.notify_all();
			}
		}

	private:
		std::mutex mtx;
		std::condition_variable cv_reader;
		std::condition_variable cv_writer;
		int active_readers{0};
		int waiting_writers{0};
		int waiting_readers{0};
		bool active_writers{false};
	};

	// Test 1: Multiple readers can enter simultaneously
	bool test_multiple_readers()
	{
		ReaderWriterLock rw;
		std::vector<unsigned> counts;
		std::mutex log_mtx;

		auto reader = [&]() {
			unsigned before = rw.ReaderLock();
			{
				std::lock_guard<std::mutex> lk(log_mtx);
				counts.push_back(before);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			rw.ReaderUnlock();
		};

		std::thread t1(reader), t2(reader), t3(reader);
		t1.join(); t2.join(); t3.join();

		// At least one reader should have seen another reader already inside
		unsigned max_before = *std::max_element(counts.begin(), counts.end());
		bool passed = max_before >= 1;
		std::cout << "[multiple-readers]: " << (passed ? "PASS" : "FAIL") << "\n";
		return passed;
	}

	// Test 2: Only one writer at a time (another writer tries to join)
	bool test_single_writer_writer()
	{
		ReaderWriterLock rw;
		int inside = 0;
		bool overlap = false;
		std::mutex log_mtx;

		auto writer = [&]() {
			rw.WriterLock();
			{
				std::lock_guard<std::mutex> lk(log_mtx);
				++inside;
				if (inside > 1) overlap = true;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			{
				std::lock_guard<std::mutex> lk(log_mtx);
				--inside;
			}
			rw.WriterUnlock();
		};

		std::thread t1(writer), t2(writer);
		t1.join(); t2.join();

		bool passed = !overlap;
		std::cout << "[single-writer-(writer)]: " << (passed ? "PASS" : "FAIL") << "\n";
		return passed;
	}

	// Test 3: Only one writer at a time (a reader tries to join)
	bool test_single_writer_reader()
	{
		ReaderWriterLock rw;
		bool overlap = false;
		bool writer_inside = false;
		std::mutex log_mtx;

		rw.WriterLock();
		writer_inside = true;

		std::thread reader([&]() {
			rw.ReaderLock();
			{
				std::lock_guard<std::mutex> lk(log_mtx);
				if (writer_inside) overlap = true;
			}
			rw.ReaderUnlock();
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		writer_inside = false;
		rw.WriterUnlock();

		reader.join();

		bool passed = !overlap;
		std::cout << "[single-writer-(reader)]: " << (passed ? "PASS" : "FAIL") << "\n";
		return passed;
	}

	// Test 4: Writer gets priority over pending readers
	bool test_writer_priority()
	{
		ReaderWriterLock rw;
		std::vector<std::string> log;
		std::mutex log_mtx;

		// First writer holds the lock
		rw.WriterLock();

		// Reader queues up
		std::thread reader([&]() {
			rw.ReaderLock();
			{
				std::lock_guard<std::mutex> lk(log_mtx);
				log.push_back("reader");
			}
			rw.ReaderUnlock();
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		// Second writer queues up after reader
		std::thread writer2([&]() {
			rw.WriterLock();
			{
				std::lock_guard<std::mutex> lk(log_mtx);
				log.push_back("writer");
			}
			rw.WriterUnlock();
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		// Release — writer2 should go before reader
		rw.WriterUnlock();

		reader.join();
		writer2.join();

		bool passed = log.size() == 2 && log[0] == "writer" && log[1] == "reader";
		std::cout << "[writer-given-priority]: " << (passed ? "PASS" : "FAIL") << "\n";
		return passed;
	}
}

int main()
{
	bool all_passed = true;
	all_passed &= getcracked::test_multiple_readers();
	all_passed &= getcracked::test_single_writer_writer();
	all_passed &= getcracked::test_single_writer_reader();
	all_passed &= getcracked::test_writer_priority();

	std::cout << "\n" << (all_passed ? "All tests passed!" : "Some tests FAILED.") << "\n";
	return all_passed ? 0 : 1;
}