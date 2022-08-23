#include "CodeGenExceptions.h"

#include <utility>
#include <format>


CodeGenException::CodeGenException(std::string message)
	: message(std::move(message))
{
}

CodeGenException::CodeGenException(std::string message, std::string context)
	: message(std::move(message))
	, context({std::move(context)})
{
}

void CodeGenException::add_context(std::string context)
{
	this->context.push_back(std::move(context));
}

char const* CodeGenException::what() const
{
	formatted_message.reserve(16 + message.size() + context.size() * 32);

	formatted_message = std::format("CodeGenException: {}\nContext:\n", message);

	for (const auto& single_context : context) {
		formatted_message += "    "; 
		formatted_message += single_context;
	}

	return formatted_message.c_str();
}
