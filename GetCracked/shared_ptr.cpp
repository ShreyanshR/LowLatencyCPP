#include <cstddef>
#include <atomic>
#include <mutex>

namespace getcracked
{
	struct control_block
	{
		size_t count_{ };
		mutable std::mutex mutex_;
	};

	template <typename T>
	class shared_ptr
	{
	public:

		shared_ptr() :  m_ptr(nullptr), m_cb(nullptr)  { }
		shared_ptr(T* pointer) : m_ptr{pointer}, m_cb(new control_block{1}){}

		shared_ptr(const shared_ptr& other) noexcept : m_ptr(other.m_ptr), m_cb(other.m_cb)
		{
			if (m_cb) {
				std::lock_guard<std::mutex> lock(m_cb->mutex_);
				++m_cb->count_;
			}
		}


		shared_ptr& operator=(const shared_ptr& other) noexcept
		{
			if (this == &other) return *this;
			release();

			if (other.m_cb)
			{
				std::lock_guard<std::mutex> lock(other.m_cb->mutex_);
				++other.m_cb->count_;
			}

			m_ptr = other.m_ptr;
			m_cb = other.m_cb;

			return *this;
		}

		shared_ptr(shared_ptr&& other) noexcept : m_ptr(other.m_ptr), m_cb(other.m_cb)
		{
			other.m_cb = nullptr;
			other.m_ptr = nullptr;

		}

		shared_ptr& operator=(shared_ptr&& other) noexcept
		{
			if (this == &other) return *this;
			release();

			m_cb = other.m_cb;
			m_ptr = other.m_ptr;

			other.m_cb = nullptr;
			other.m_ptr = nullptr;

			return *this;
		}

		~shared_ptr()
		{
			release();
		}

		void reset(T* pointer)
		{
			release();
			if (pointer) {
				m_ptr = pointer;
				m_cb = new control_block{1};
			}
		}


		size_t get_count() const
		{
			if (!m_cb) return 0;
			std::lock_guard<std::mutex> lock(m_cb->mutex_);
			return m_cb->count_;
		}

		T* operator->() const { return m_ptr;}
		T& operator*() const { return *m_ptr;}
		operator bool() const noexcept { return m_ptr != nullptr;}

	private:
		T* m_ptr;
		control_block* m_cb;
		void release() {
			if (!m_cb) return;
			bool should_delete = false;
			{
				std::lock_guard<std::mutex> lock(m_cb->mutex_);
				should_delete = (--m_cb->count_ == 0);
			}
			if (should_delete) {
				delete m_ptr;
				delete m_cb;
			}
			m_ptr = nullptr;
			m_cb = nullptr;
		}
	};
}