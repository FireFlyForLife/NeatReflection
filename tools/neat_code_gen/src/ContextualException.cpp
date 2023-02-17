#include "ContextualException.h"

#include <format>


static thread_local std::vector<std::string> unrolling_context;

ContextualException::ContextualException()
{
	unrolling_context.clear();
}

ContextualException::ContextualException(std::string message) 
	: message(std::move(message))
{
	unrolling_context.clear();
}

ContextualException::ContextualException(std::string message, std::string context)
	: message(std::move(message))
	, context({ std::move(context) })
{
	unrolling_context.clear();
}

ContextualException::~ContextualException()
{
	unrolling_context.clear();
}

char const* ContextualException::what() const noexcept
{
	auto indentation = "    ";

	std::string flattened_context;
	for (auto context_entry : context)
		flattened_context += indentation + context_entry + '\n';
	for (auto context_entry : unrolling_context)
		flattened_context += indentation + context_entry + '\n';

	formatted_message = std::format("Error: \n{0}{1}\nContext: \n{2}", indentation, message, flattened_context);

	return formatted_message.c_str();
}

namespace Detail
{
	void context_area_add_context_current_thread(std::string&& context_point)
	{
		unrolling_context.push_back(context_point);
	}
}
