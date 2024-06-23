#include "ContextualException.h"

#include <format>
#include <cassert>


static thread_local std::vector<IContextArea*> context_area_stack;


ContextualException::ContextualException(std::string_view message, std::string_view context)
{
	// Prepare arguments
	// Copy context area stack to prevent it from being modified during iteration
	std::vector<IContextArea*> copy_context_area_stack = context_area_stack;
	formatted_message.reserve(message.size() + copy_context_area_stack.size() * 32);
	const auto indentation = "    ";

	// Start with the message
	formatted_message += "Error:\n";
	formatted_message += indentation;
	formatted_message += message;
	formatted_message += "\nContext:\n";

	// Optionally add the exception's context
	if (!context.empty()) {
		formatted_message += indentation;
		formatted_message += context;
		formatted_message += '\n';
	}

	// Unroll the context stack in reverse
	for (auto context_area_it = copy_context_area_stack.rbegin(); context_area_it != copy_context_area_stack.rend(); ++context_area_it) {
		formatted_message += indentation;
		formatted_message += (*context_area_it)->get_formatted_context();
		formatted_message += '\n';
	}
}

char const* ContextualException::what() const noexcept
{
	return formatted_message.c_str();
}

namespace Detail
{
	void context_area_push(IContextArea& context_area) noexcept
	{
		context_area_stack.push_back(&context_area);
	}

	void context_area_pop(IContextArea& context_area) noexcept
	{
		assert(context_area_stack.back() == &context_area && "Context area's are supposed to be destructed in reverse order (like a stack).");
		context_area_stack.pop_back();
	}
}
