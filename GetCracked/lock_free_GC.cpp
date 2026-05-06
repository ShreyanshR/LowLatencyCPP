// Write your solution here
// C++23 using GCC 14.2
// Debug with std::cerr or std::clog.
// !!! IMPORTANT !!!
// 99% of headers are pre-compiled for you server-side.
// If your submission fails to compile due to a missing header, add it to your submission.

#include <atomic>
#include <cstddef>
#include <vector>

namespace getcracked {
    template <typename T>
    class SPSCQ
    {
    public:
        SPSCQ(size_t capacity) : data_(capacity){
            if((capacity & (capacity - 1)) != 0 || capacity < 2){
                throw "Not Power of 2";
            }
        }

        bool push(const T& item)
        {
            auto const writerX = writerX_.load(std::memory_order_relaxed);
            auto nextWriter = writerX + 1;
            if((writerX - readX_.load(std::memory_order_acquire)) == data_.size()){
                return false;
            }
            data_[writerX & (data_.size() - 1)] = item;
            writerX_.store(nextWriter, std::memory_order_release);
            return true;
        }

        bool pop(T& item)
        {
            auto const readX = readX_.load(std::memory_order_relaxed);
            if(readX == writerX_.load(std::memory_order_acquire)){
                //no elements, so return false
                return false;
            }
            item = data_[readX & (data_.size() - 1)];
            auto nextReader = readX + 1;
            readX_.store(nextReader, std::memory_order_release);
            return true;
        }

        [[nodiscard]] bool full() const
        {
            return (writerX_.load(std::memory_order_acquire) - readX_.load(std::memory_order_acquire)) == data_.size();
        }

        [[nodiscard]] size_t size() const
        {
            return writerX_.load(std::memory_order_acquire) - readX_.load(std::memory_order_acquire);
        }

        [[nodiscard]] bool empty() const
        {
            return writerX_.load(std::memory_order_acquire) == readX_.load(std::memory_order_acquire);
        }

    private:
        std::vector<T> data_;
        std::atomic<size_t> writerX_;
        std::atomic<size_t> readX_;
    };
}
