#include "catch2/catch_all.hpp"
#include "Neat/Reflection.h"

#include <string_view>
#include <string>
#include <vector>
#include <array>

import TestModule1;


static Neat::Method* find_method(Neat::Type& type, std::string_view name)
{
	for (auto& method : type.methods)
	{
		if (method.name == name)
		{
			return &method;
		}
	}

	return nullptr;
}

TEST_CASE("Invoke method2")
{
	MethodTester method_tester{};

	Neat::Type* type = Neat::get_type<MethodTester>();
	REQUIRE(type != nullptr);

	auto method = find_method(*type, "TestMethod2");
	REQUIRE(method != nullptr);

	std::array<std::any, 2> args{ 4, 5 };
	method->invoke(&method_tester, args);

	CHECK(method_tester.test_method_2_args == "4,5");
}

TEST_CASE("Invoke method return")
{
	MethodTester method_tester{};

	Neat::Type* type = Neat::get_type<MethodTester>();
	REQUIRE(type != nullptr);

	auto method = find_method(*type, "TestMethodReturn");
	REQUIRE(method != nullptr);

	auto result = method->invoke(&method_tester, {});

	REQUIRE(result.has_value());
	auto result_int = std::any_cast<int>(&result);
	REQUIRE(result_int != nullptr);
	REQUIRE(*result_int == MethodTester::test_method_return_value);
}