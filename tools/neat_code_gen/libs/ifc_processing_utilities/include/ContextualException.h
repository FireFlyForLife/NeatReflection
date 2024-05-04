#pragma once
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include <format>
#include <source_location>


// Throws a ContextualException if the statement is false
void verify(bool success, std::source_location source_location = std::source_location::current());
void verify(bool success, std::string_view additional_message, std::source_location source_location = std::source_location::current());

// Exception that carries context information about what happened earlier in the stack, via ContextArea's
class ContextualException : public std::exception
{
public:
	// Constructors & Deconstructors
	ContextualException();
	ContextualException(std::string message);
	ContextualException(std::string message, std::string context);
	~ContextualException();

	ContextualException(const ContextualException&) = default;
	ContextualException(ContextualException&&) noexcept = default;
	ContextualException& operator=(const ContextualException&) = default;
	ContextualException& operator=(ContextualException&&) noexcept = default;

	// Accessors
	[[nodiscard]] char const* what() const noexcept override; // Note: Not threadsafe

private:
	std::string message;
	std::vector<std::string> context;

	mutable std::string formatted_message;
};

// RAII class that adds more context information for when ContextualException is thrown
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

inline void verify(bool success, std::source_location source_location)
{
	if (!success) [[unlikely]] {
		throw ContextualException(std::format("Failed to verify statement at {}:{},{} in '{}'.",
			source_location.function_name(), source_location.line(), source_location.column(), source_location.file_name()));
	}
}

inline void verify(bool success, std::string_view additional_message, std::source_location source_location)
{
	if (!success) [[unlikely]] {
		throw ContextualException(std::format("Failed to verify statement, message: \"{}\".\nStatement at{}:{}, {} in '{}'.",
			additional_message, source_location.function_name(), source_location.line(), source_location.column(), source_location.file_name()));
	}
}

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
