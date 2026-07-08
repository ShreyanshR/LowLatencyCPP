// Write your solution here
// C++23 using GCC 14.2
// Debug with std::cerr or std::clog.
// !!! IMPORTANT !!!
// 99% of headers are pre-compiled for you server-side.
// If your submission fails to compile due to a missing header, add it to your submission.

#include <exception>
#include <stdexcept>

namespace getcracked
{
  template <typename T>
  struct custom_deleter
  {
    void operator()(T* pointer) const
    {
      delete pointer;
    }
  };

  template <typename T, typename custom_deleter = custom_deleter<T>>
  class unique_ptr
  {
  public:
    unique_ptr() : ptr(nullptr)
    {
    }
    explicit unique_ptr(T* pointer) : ptr(pointer){}

    unique_ptr(const unique_ptr&) {
      throw std::runtime_error("Copy not possible");
    }
    unique_ptr& operator=(const unique_ptr&) {
      throw std::runtime_error("Copy assignment not possible");
    }

    unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr) {
      other.ptr = nullptr;
    }

    unique_ptr& operator=(unique_ptr&& other) noexcept
    {
      if (this != &other) {
        delete ptr;
        ptr = other.ptr;
        other.ptr = nullptr;
      }

      return *this;

    }

    ~unique_ptr()
    {
      deleter(ptr);
    }

    T* release(){
      T* temp = ptr;
      ptr = nullptr;
      return temp;
    }

    void reset(T* pointer){
      if (ptr != pointer) {
        deleter(ptr);
        ptr = pointer;
      }
    }

    bool is_owning() const {
      return ptr != nullptr;
    }


    T& operator*() const {
      return *ptr;
    }
    T* operator->() const {
      return ptr;
    }
    operator bool() const {
      return ptr != nullptr;
    }

  private:
    T* ptr;
    [[no_unique_address]] custom_deleter deleter;
  };
}
