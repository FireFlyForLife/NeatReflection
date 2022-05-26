#include "catch2/catch_all.hpp"
#include "Neat/Reflection.h"

import TestModule1;

TEST_CASE("struct members")
{
	REQUIRE_FALSE(false);
	REQUIRE(true);

	auto type = Neat::get_type<MyStruct>();
	REQUIRE(type != nullptr);
}