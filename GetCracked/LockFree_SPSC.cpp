#include <cstddef>
#include <atomic>
#include <vector>

namespace getcracked
{
	template <typename T>
	class SPSCQ
	{
	public:
		SPSCQ(size_t capacity) : data_(capacity) {
			if ((capacity & (capacity - 1)) != 0 || capacity < 2) {
				throw "NOT POWER OF 2";
			}
		}

		bool push(const T& item)
		{
			auto const writerIdx = writerX_.load(std::memory_order_relaxed);
			auto next_writerIdx = writerIdx + 1;
			if ((writerIdx - readerX_.load(std::memory_order_acquire)) == data_.size()) {
				return false;
			}
			data_[writerIdx & (data_.size() - 1)] = item;
			writerX_.store(next_writerIdx, std::memory_order_release);
			return true;
		}

		bool pop(T& item)
		{
			auto const readerIdx = readerX_.load(std::memory_order_relaxed);
			if (readerIdx == writerX_.load(std::memory_order_acquire)) {
				return false;
			}
			item = data_[readerIdx & (data_.size() - 1)];
			auto nextReader = readerIdx + 1;
			readerX_.store(nextReader, std::memory_order_release);
			return true;
		}

		[[nodiscard]] bool full() const
		{
			return (writerX_.load(std::memory_order_acquire) - readerX_.load(std::memory_order_acquire)) == data_.size();
		}

		[[nodiscard]] size_t size() const
		{
			return writerX_.load(std::memory_order_acquire) - readerX_.load(std::memory_order_acquire);
		}

		[[nodiscard]] bool empty() const
		{
			return writerX_.load(std::memory_order_acquire) == readerX_.load(std::memory_order_acquire);
		}

	private:
		std::vector<T> data_;
		alignas(64) std::atomic<size_t> readerX_;
		alignas(64) std::atomic<size_t> writerX_;
	};
}