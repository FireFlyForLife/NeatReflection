#pragma once

#include <string>
#include <string_view>


std::string replace_all_copy(std::string str, std::string_view target, std::string_view replacement);
std::string to_snake_case(std::string_view type_name);
