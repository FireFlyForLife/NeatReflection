#include "ContextualException.h"

#include <format>


ContextualException::ContextualException(std::string message) 
	: message(std::move(message))
{
}

ContextualException::ContextualException(std::string message, std::string context)
	: message(std::move(message))
	, context({ std::move(context) })
{
}

void ContextualException::add_context(std::string context)
{
	this->context.push_back(std::move(context));
}

char const* ContextualException::what() const
{
	std::string flattened_context;
	for (auto context_entry : context)
		flattened_context += "    " +  context_entry + '\n';

	formatted_message = std::format("Error: {0}\nContext: {1}", message, flattened_context);

	return formatted_message.c_str();
}
