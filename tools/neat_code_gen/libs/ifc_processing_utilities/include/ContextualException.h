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
	ContextualException(std::string_view message, std::string_view context = std::string_view{});

	ContextualException(const ContextualException&) = default;
	ContextualException(ContextualException&&) noexcept = default;
	ContextualException& operator=(const ContextualException&) = default;
	ContextualException& operator=(ContextualException&&) noexcept = default;

	// Accessors
	[[nodiscard]] char const* what() const noexcept override;

private:
	std::string formatted_message;
};

// Interface for a class that can add context information for when ContextualException is thrown
class IContextArea
{
public:
	virtual ~IContextArea() = default;

	virtual std::string get_formatted_context() const noexcept = 0;
};

// RAII class that adds more context information for when ContextualException is thrown
template<typename... TArgs>
class ContextArea : IContextArea
{
public:
	// Construction & Deconstruction
	ContextArea(std::string context_fmt, TArgs&&... args);
	ContextArea(std::string_view context_fmt, TArgs&&... args);
	~ContextArea();

	// No move or copy
	ContextArea(const ContextArea&) = delete;
	ContextArea(ContextArea&&) = delete;
	ContextArea& operator=(const ContextArea&) = delete;
	ContextArea& operator=(ContextArea&&) = delete;

	// Stack only
	void* operator new (std::size_t) = delete;
	void operator delete(void*) = delete;

	// IContextArea interface
	std::string get_formatted_context() const noexcept override;

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
	void context_area_push(IContextArea& context_area) noexcept;
	void context_area_pop(IContextArea& context_area) noexcept;
}

template<typename... TArgs>
ContextArea<TArgs...>::ContextArea(std::string context_fmt, TArgs&&... args)
	: context_fmt_owned(std::move(context_fmt))
	, context_fmt(context_fmt_owned)
	, args(std::make_format_args(args...))
{
	Detail::context_area_push(*this);
}

template<typename... TArgs>
ContextArea<TArgs...>::ContextArea(std::string_view context_fmt, TArgs&&... args)
	: context_fmt(context_fmt)
	, args(std::make_format_args(args...))
{
	Detail::context_area_push(*this);
}

template<typename... TArgs>
ContextArea<TArgs...>::~ContextArea()
{
	Detail::context_area_pop(*this);
}

template<typename... TArgs>
std::string ContextArea<TArgs...>::get_formatted_context() const noexcept
{
	return std::vformat(context_fmt, args);
}
