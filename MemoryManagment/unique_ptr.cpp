

// unique_ptr<T,D>
// D is default deleter
//We can pass our own

#include <codecvt>
#include <iostream>

namespace managing_memory_book {
	template<typename T>
	struct deleter_pointer_wrapper {
		void(*pf)(T*);
		deleter_pointer_wrapper(void(*pf)(T*)) : pf{pf}{}
		void operator()(T* p) const {
			std::cout<<"pf(p)\n";
			pf(p);
		}
	};
	template<typename T>
	struct default_deleter {
		void operator()(T* p) const {
			std::cout <<"delete p\n";
			delete p;
		}
	};
	template <typename T>
	struct default_deleter<T[]> {
		void operator()(T* p) const {
			std::cout <<"delete [] p\n";
			delete[] p;
		}
	};
	template <typename T>
	struct is_deleter_function_candidate : std::false_type{};
	template <typename T>
	struct is_deleter_function_candidate<void (*)(T*)> : std::true_type{};
	template <typename T>
	constexpr auto is_deleter_function_candidate_v =
		is_deleter_function_candidate<T>::value;

	// Unique Pointer General
	template<typename T, typename D = default_deleter<T>>
	class unique_ptr : std::conditional_t<
		is_deleter_function_candidate_v<D>,
		deleter_pointer_wrapper<T>,
		D
		> {
		using deleter_type = std::conditional_t<is_deleter_function_candidate_v<D>,
			deleter_pointer_wrapper<T>,
			D
			>;
		T* p = nullptr;

	public:
		unique_ptr() = default;
		unique_ptr(T* p) : p{p} {
			std::cout << "Genearal Template\n";
		}
		unique_ptr(T* p, void(*pf)(T*)) : deleter_type{pf}, p{p} {
			std::cout << "Specialization for fxn pointer\n";
		}
		~unique_ptr() {
			(*static_cast<deleter_type*>(this))(p);
		}
	};

	template <typename T, typename D>
	class unique_ptr<T[], D> : std::conditional_t<
		is_deleter_function_candidate_v<D>,
		deleter_pointer_wrapper<T>,
		D
		> {
			using deleter_type = std::conditional_t<
				is_deleter_function_candidate_v<D>,
				deleter_pointer_wrapper<T>,
				D
				>;
			T* p = nullptr;
	public:
		unique_ptr() = default;
		unique_ptr(T* p) : p{p} {
			std::cout<<"Specialization for arrays\n";
		}
		unique_ptr(T* p, void(*pf)(T*)) : deleter_type(pf), p(p) {
			std::cout<<"Specialization for arrays with fxn pointer\n";
		}
		~unique_ptr() {
			(*static_cast<deleter_type*>(this))(p);
		}
	};
}

int main() {
	using managing_memory_book::unique_ptr;
	unique_ptr<int> p0{new int{3}};
}