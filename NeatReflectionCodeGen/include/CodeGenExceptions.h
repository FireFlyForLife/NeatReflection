#pragma once
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <tuple>
#include <type_traits>
#include <format>


class CodeGenException : public std::exception
{
public:
	// Constructors
	CodeGenException() = default;
	CodeGenException(std::string message);
	CodeGenException(std::string message, std::string context);

	// Modifiers
	void add_context(std::string context);

	// Accessors
	[[nodiscard]] char const* what() const override;

private:
	std::string message;
	std::vector<std::string> context;

	mutable std::string formatted_message;
};

//namespace Detail
//{
//	template<typename... TArgs, size_t... Indices>
//	auto format_with_tuple_args(std::string_view fmt, std::tuple<TArgs...>& args, std::index_sequence<Indices...>)
//	{
//		return std::format(fmt, std::get<Indices>(args)...);
//	}
//}
//
//template<typename... TArgs>
//class ContextArea
//{
//public:
//	ContextArea(std::string_view context) 
//		: context_fmt(context)
//	{
//	}
//	ContextArea(std::string_view context_fmt, TArgs&&... args)
//		: context_fmt(context_fmt)
//		, args(std::forward<TArgs>(args)...)
//	{
//	}
//
//	~ContextArea()
//	{
//		if (std::uncaught_exceptions() > 0) [[unlikely]]
//		{
//			try {
//				std::rethrow_exception(std::current_exception());
//			} catch(CodeGenException& exception) {
//				auto context = Detail::format_with_tuple_args(context_fmt, args, std::make_index_sequence<sizeof...(TArgs)>{});
//				exception.add_context(std::move(context));
//			}
//		}
//	}
//
//private:
//	std::string_view context_fmt;
//	std::tuple<TArgs...> args;
//};