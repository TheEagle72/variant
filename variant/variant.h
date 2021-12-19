#pragma once

#include <memory>
#include <typeinfo>

template <typename... Types>
struct variant;

template <>
struct variant<>
{
	variant(size_t index) {}
	template <typename T> variant(size_t index, const T& t) {}
	template <typename T> variant& operator=(const T& value) { return *this; }
	static size_t index() { throw::std::exception("variant holds no value"); }
	[[nodiscard]] static bool valueless_by_exception() { return true; }
	[[nodiscard]] bool operator<(const variant& other) const { return false; }
};

template <typename Head, typename... Tail>
struct variant<Head, Tail...> : variant<Tail...>
{
private:
	bool holds_value = false;
	std::unique_ptr<void*> ptr_;
	size_t index_;
	void reset();

public:
	variant();
	variant(size_t index);

	template <typename T> variant(const T& value);
	template <typename T> variant(size_t index, const T& value);

	variant(const variant& other);
	//variant(variant&& other) noexcept; //todo implement
	~variant();

	template<typename T> variant& operator=(const variant& other);
	template<typename T> variant& operator=(const T& value);
	//template<typename T> variant& operator=(T&& value) noexcept;

	bool operator<(const variant& other) noexcept;

	[[nodiscard]] size_t index() const;
	[[nodiscard]] bool valueless_by_exception() const;
	void swap(variant& other) noexcept;
	template <class T, class... Args> T& emplace(Args&&... args);

	typedef variant<Tail...> base_type;
	typedef Head value_type;

	base_type& base = static_cast<base_type&>(*this);
};

template <typename Head, typename ... Tail>
void variant<Head, Tail...>::reset()
{
	if (holds_value)
	{
		delete static_cast<value_type*>(*ptr_);
		ptr_.reset();
		holds_value = false;
	}
}

template <typename Head, typename ... Tail>
variant<Head, Tail...>::variant()
	: variant<Tail...>(static_cast<size_t>(1)), index_(0) {}

template <typename Head, typename ... Tail>
variant<Head, Tail...>::variant(const size_t index)
	: variant<Tail...>(index + 1), index_(index) {}

template <typename Head, typename ... Tail>
template <typename T>
variant<Head, Tail...>::variant(const T& value)
	: variant<Tail...>(1, value), index_(0)
{
	if (typeid(value_type) == typeid(decltype(value)))
	{
		ptr_ = std::make_unique<void*>(static_cast<void*>(new T{ value }));
		holds_value = true;
	}
}

template <typename Head, typename ... Tail>
template <typename T>
variant<Head, Tail...>::variant(const size_t index, const T& value)
	: variant<Tail...>(index + 1, value), index_(index)
{
	if (typeid(value_type) == typeid(decltype(value)))
	{
		ptr_ = std::make_unique<void*>(static_cast<void*>(new T{ value }));
		holds_value = true;
	}
}

template <typename Head, typename ... Tail>
variant<Head, Tail...>::variant(const variant& other)
{
	ptr_ = { other.ptr_ };
	base = { other.base };
}


template <typename Head, typename ... Tail>
variant<Head, Tail...>::~variant()
{
	delete static_cast<value_type*>(*ptr_);
}

template <typename Head, typename ... Tail>
template <typename T>
variant<Head, Tail...>& variant<Head, Tail...>::operator=(const variant& other)
{
		
}

template <typename Head, typename ... Tail>
template <typename T>
variant<Head, Tail...>& variant<Head, Tail...>::operator=(const T& value)
{
	reset();
	if (typeid(value_type) == typeid(decltype(value)))
	{
		holds_value = true;
		ptr_ = std::make_unique<void*>(static_cast<void*>(new T{ value }));
	}

	base.operator=(value);
	return *this;
}

template <typename Head, typename ... Tail>
bool variant<Head, Tail...>::operator<(const variant& other) noexcept
{
	if (holds_value != other.holds_value)
	{
		return false;
	}

	if (holds_value)
	{
		value_type lhs = *static_cast<value_type*>(*ptr_);
		value_type rhs = *static_cast<value_type*>(*other.ptr_);
		return lhs < rhs;
	}

	return (base < other.base);
}

template <typename Head, typename... Tail>
size_t variant<Head, Tail...>::index() const
{
	if (holds_value)
	{
		return index_;
	}
	return base.index();
}

template <typename Head, typename ... Tail>
bool variant<Head, Tail...>::valueless_by_exception() const
{
	if (holds_value)
	{
		return false;
	}

	return base.valueless_by_exception();
}

template <typename Head, typename ... Tail>
void variant<Head, Tail...>::swap(variant& other) noexcept
{
	ptr_.swap(other.ptr_);
	base.swap(other.base);
}
