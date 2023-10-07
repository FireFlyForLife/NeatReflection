#pragma once
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include <format>


class ContextualException : public std::exception
{
public:
	// Constructors & Deconstructors
	ContextualException();
	ContextualException(std::string message);
	ContextualException(std::string message, std::string context);
	~ContextualException();

	ContextualException(const ContextualException&) = default;
	ContextualException(ContextualException&&) = default;
	ContextualException& operator=(const ContextualException&) = default;
	ContextualException& operator=(ContextualException&&) = default;

	// Accessors
	[[nodiscard]] char const* what() const noexcept override; // Note: Not threadsafe

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
	std::format_args args;
};


// Implementation
// ============================================================================

namespace Detail
{
	void context_area_add_context_current_thread(std::string&& context_point);
}

template<typename... TArgs>
ContextArea<TArgs...>::ContextArea(std::string context_fmt, TArgs&&... args)
	: context_fmt_owned(std::move(context_fmt))
	, context_fmt(context_fmt_owned)
	, args(std::make_format_args(args...))
{
}

template<typename... TArgs>
ContextArea<TArgs...>::ContextArea(std::string_view context_fmt, TArgs&&... args)
	: context_fmt(context_fmt)
	, args(std::make_format_args(args...))
{
}

template<typename... TArgs>
ContextArea<TArgs...>::~ContextArea()
{
	if (std::uncaught_exceptions() > 0) [[unlikely]]
	{
		std::string formatted_context = std::vformat(context_fmt, args);
		Detail::context_area_add_context_current_thread(std::move(formatted_context));
	}
}
