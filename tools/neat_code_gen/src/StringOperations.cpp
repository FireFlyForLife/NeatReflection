#include "StringOperations.h"

#include <cctype>


std::string replace_all_copy(std::string str, std::string_view target, std::string_view replacement)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(target, start_pos)) != std::string::npos) {
		str.replace(start_pos, target.length(), replacement);
		start_pos += replacement.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

std::string to_snake_case(std::string_view type_name)
{
	std::string snake_case;
	snake_case.reserve(type_name.size() + 10);

	bool previous_uppercase = true;
	for (auto c : type_name)
	{
		if (!previous_uppercase && std::isupper(c))
		{
			snake_case.push_back('_');
		}

		if (std::isalnum(c))
		{
			snake_case.push_back(std::tolower(c));
		}
		else
		{
			snake_case.push_back('_'); // Fallback
		}

		previous_uppercase = std::isupper(c);
	}

	return snake_case;
}