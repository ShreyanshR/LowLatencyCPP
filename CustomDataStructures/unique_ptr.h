#include <iostream>

template<typename T>
class UniquePtr {
private:
  T* ptr;
public:
  explicit UniquePtr(T* p) : ptr(p) {}

  UniquePtr(const UniquePtr&) = delete;
  UniquePtr& operator=(const UniquePtr&) = delete;

  UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
    // so we assinged other resource to our ptr, so we have to remove the other resource
    //there is no way to delete a ptr in Cpp witouth deleting the resoruse it's pointing to, so
    //we set it to nullptr
    other.ptr = nullptr;
  }

  // move assignment, cleans up the current object, then takes over other's ptr
  UniquePtr& operator=(UniquePtr&& other) noexcept {
    if (this != &other) {
      delete ptr;
      ptr = other.ptr;
      other.ptr = nullptr;
    }

    return *this;
  }

  //dereference operator to access the object
  T& operator*() const {
    return *ptr;
  }

  //arrow operator to access object's members
  T* operator->() const {
    return ptr;
  }

  //return the raw ptr without releasing ownership
  T* get() const {
    return ptr;
  }

  T* release() {
    T* temp = ptr;
    ptr = nullptr;
    return temp;
  }

  void reset(T* p = nullptr) {
    if (ptr != p) {
      delete ptr;
      ptr = p;
    }
  }
  ~UniquePtr() {
    delete ptr;
  }

};
