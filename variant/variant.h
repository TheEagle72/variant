#pragma once

#include <memory>

template<int I, typename... Types>
struct get_nth_type;

template <int I, typename Head, typename... Tail>
struct get_nth_type<I, Head, Tail...>
{
	using type = typename get_nth_type<I - 1, Tail...>::type;
};

template <typename Head, typename... Tail>
struct get_nth_type<0, Head, Tail...>
{
	using type = Head;
};

template<typename... Types>
struct get_max_size;

template <typename Head, typename... Tail>
struct get_max_size<Head, Tail...>
{
	constexpr static size_t size()
	{
		return std::max(sizeof(Head), get_max_size<Tail...>::size());
	}
};

template <typename Head>
struct get_max_size<Head>
{
	constexpr static size_t size() { return sizeof(Head); }
};

template <int I, typename Find, typename... Types>
struct get_index_of_type;

template <int I, typename Find, typename Head, typename... Tail>
struct get_index_of_type<I, Find, Head, Tail...>
{
	constexpr static size_t index()
	{
		if (std::is_same_v<Find, Head>)
		{
			return I;
		}
		else
		{
			return  get_index_of_type < I + 1, Find, Tail...>::index();
		}

	}
};

template <int I, typename Find>
struct get_index_of_type<I, Find>
{
	constexpr static size_t index()
	{
		return -1;
	}
};

template <typename  ... Types>
struct unique_types;

template <class T1, class T2, class ... Tail>
struct unique_types<T1, T2, Tail ...>
{
	constexpr static  bool is_true = (unique_types<T1, T2>::is_true && unique_types<T1, Tail ...>::is_true && unique_types < T2, Tail ...>::is_true);
};

template <class T1, class T2>
struct unique_types<T1, T2>
{
	constexpr static  bool is_true = (!std::is_same_v<T1, T2>);
};


template <typename... Types>
struct variant
{
private:
	bool holds_value = false;
	std::unique_ptr<void*> ptr_;
	size_t index_;
	size_t byte_size_ = 0;
public:
	variant() requires unique_types<Types...>::is_true;
	template <typename T> variant(const T& value) requires unique_types<Types...>::is_true;
	variant(const variant& other);
	variant(variant&& other) noexcept; //todo implement
	~variant() = default;

	variant& operator=(const variant& other);

	template<typename T>
	variant& operator=(const T& value);

	// template<typename T>
	// variant& operator=(T&& value) noexcept;

	bool operator==(const variant& other) noexcept;
	bool operator!=(const variant& other) noexcept;
	bool operator<(const variant& other) noexcept;
	bool operator>(const variant& other) noexcept;

	[[nodiscard]] size_t index() const;
	[[nodiscard]] bool valueless_by_exception() const;
	void swap(variant& other) noexcept;

	template<size_t I, typename... TypesOf>
	friend typename  get_nth_type<I, TypesOf...>::type get(const variant <TypesOf...>& v);

	template<typename Head, typename... TypesOf>
	friend typename  get_nth_type<get_index_of_type<0, Head, TypesOf...>::index(), TypesOf...>::type get(const variant <TypesOf...>& v);
};

template <typename ... Types>
variant<Types...>::variant() requires unique_types<Types...>::is_true : index_(0), byte_size_(get_max_size<Types...>::size())
{
	holds_value = true;
	ptr_ = std::make_unique<void*>(new char[byte_size_]);
}

template <typename ... Types>
template <typename T>
variant<Types...>::variant(const T& value) requires unique_types<Types...>::is_true : index_(get_index_of_type<0, T, Types...>::index()), byte_size_(get_max_size<Types...>::size())
{
	holds_value = true;
	ptr_ = std::make_unique<void*>(new char[byte_size_]);
	memcpy(ptr_.get(), &value, byte_size_);
}

template <typename ... Types>
variant<Types...>::variant(const variant& other) : index_(other.index())
{
	holds_value = other.holds_value;
	memcpy(ptr_.get(), other.ptr_.get(), byte_size_);
}

template <typename ... Types>
variant<Types...>::variant(variant&& other) noexcept : index_(other.index())
{
	holds_value = other.holds_value;
	ptr_.reset(other.ptr_.release());
}

template <typename ... Types>
variant<Types...>& variant<Types...>::operator=(const variant& other)
{
	holds_value = other.holds_value;
	if (byte_size_ != other.byte_size_)
	{
		throw std::exception("different sizes");
	}

	index_ = other.index();
	memcpy(ptr_.get(), other.ptr_.get(), byte_size_);
	return *this;
}

template <typename ... Types>
template <typename T>
variant<Types...>& variant<Types...>::operator=(const T& value)
{
	holds_value = true;
	index_ = get_index_of_type<0, T, Types...>::index();
	memcpy(ptr_.get(), &value, sizeof(T));

	return *this;
}

template <typename ... Types>
bool variant<Types...>::operator==(const variant& other) noexcept
{
	return (!(*this < other) && !(*this > other));
}

template <typename ... Types>
bool variant<Types...>::operator!=(const variant& other) noexcept
{
	return !(*this == other);
}

template <typename ... Types>
bool variant<Types...>::operator<(const variant& other) noexcept
{
	if (index_ != other.index())
	{
		throw std::exception("trying to compare variant with diffent index");
	}

	return false;
}

template <typename ... Types>
bool variant<Types...>::operator>(const variant& other) noexcept
{
	return !(*this < other);
}


template <typename ... Types>
size_t variant<Types...>::index() const
{
	return index_;
}

template <typename ... Types>
bool variant<Types...>::valueless_by_exception() const
{
	return !holds_value;
}

template <typename ... Types>
void variant<Types...>::swap(variant& other) noexcept
{
	ptr_.swap(other.ptr_);
}

template<size_t I, typename... Types>
typename  get_nth_type<I, Types...>::type get(const variant <Types...>& v)
{
	using type = typename get_nth_type<I, Types...>::type;

	if (v.valueless_by_exception())
	{
		throw std::exception("variant have no value");
	}

	if (v.index_ != I)
	{
		throw std::exception("wrong type");
	}

	type value{};
	memset(&value, 0, sizeof(value));
	memcpy(&value, v.ptr_.get(), sizeof(value));
	return value;
}

template<typename Head, typename... TypesOf>
typename  get_nth_type<get_index_of_type<0, Head, TypesOf...>::index(), TypesOf...>::type get(const variant <TypesOf...>& v)
{
	constexpr int I = get_index_of_type<0, Head, TypesOf...>::index();
	using type = typename get_nth_type<I, TypesOf...>::type;

	if (v.valueless_by_exception())
	{
		throw std::exception("variant have no value");
	}

	if (v.index_ != I)
	{
		throw std::exception("wrong type");
	}

	type value;
	memset(&value, 0, sizeof(value));
	memcpy(&value, v.ptr_.get(), sizeof(type));
	return value;
}