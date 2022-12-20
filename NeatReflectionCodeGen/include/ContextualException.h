#pragma once
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include <type_traits>
#include <format>


class ContextualException : public std::exception
{
public:
	// Constructors
	ContextualException() = default;
	ContextualException(std::string message);
	ContextualException(std::string message, std::string context);

	// Modifiers
	void add_context(std::string context);

	// Accessors
	[[nodiscard]] char const* what() const override; // Note: Not threadsafe

private:
	std::string message;
	std::vector<std::string> context;

	mutable std::string formatted_message;
};

template<typename... TArgs>
class ContextArea
{
public:
	ContextArea(std::string context_fmt, TArgs&&... args);
	ContextArea(std::string_view context_fmt, TArgs&&... args);
	~ContextArea();

private:
	// TODO: This can be optimized out with another template param
	std::string context_fmt_owned;
	std::string_view context_fmt;
	std::tuple<TArgs...> args;
};


// Implementation
// ============================================================================

namespace Detail
{
	template<typename... TArgs, size_t... Indices>
	auto format_with_tuple_args(std::string_view fmt, std::tuple<TArgs...>& args, std::index_sequence<Indices...>)
	{
		return std::format(fmt, std::get<Indices>(args)...);
	}
}

template<typename... TArgs>
ContextArea<TArgs...>::ContextArea(std::string context_fmt, TArgs&&... args)
	: context_fmt_owned(std::move(context_fmt))
	, context_fmt(context_fmt_owned)
	, args(std::forward<TArgs>(args)...)
{
}

template<typename... TArgs>
ContextArea<TArgs...>::ContextArea(std::string_view context_fmt, TArgs&&... args)
	: context_fmt(context_fmt)
	, args(std::forward<TArgs>(args)...)
{
}

template<typename... TArgs>
ContextArea<TArgs...>::~ContextArea()
{
	if (std::uncaught_exceptions() > 0) [[unlikely]]
	{
		try {
			std::rethrow_exception(std::current_exception());
		}
		catch (ContextualException& exception) {
			if constexpr (sizeof...(TArgs) == 0)
			{
				exception.add_context(std::string{ context_fmt });
			}
			else
			{
				auto context = Detail::format_with_tuple_args(context_fmt, args, std::make_index_sequence<sizeof...(TArgs)>{});
				exception.add_context(std::move(context));
			}
		}
	}
}