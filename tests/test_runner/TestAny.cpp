#include "catch2/catch_all.hpp"

#include "neat/Any.h"
#include "neat/TemplateTypeId.h"

#include <string>

import TestModule1;


TEST_CASE("Any Constructor (from value)")
{
	using namespace std::string_literals;

	// Default constructor
	Neat::Any empty_any{};
	CHECK(!empty_any.has_value());

	// SBO
	Neat::Any value_int{ 5 };
	REQUIRE(value_int.has_value());
	REQUIRE(value_int.type_id() == Neat::get_id<int>());
	CHECK(value_int.value<int>() == 5);

	// On heap
	Neat::Any value_nontrivial{ NonTrivialClass{ 7 } };
	REQUIRE(value_nontrivial.has_value());
	REQUIRE(value_nontrivial.type_id() == Neat::get_id<NonTrivialClass>());
	CHECK(value_nontrivial.value<NonTrivialClass>().get() == 7);
}

TEST_CASE("Any Assignment Operator (from value)")
{
	SECTION("Same storage mode")
	{
		// Construct initial any's
		Neat::Any empty_any{};
		Neat::Any value_int{ 15 };
		Neat::Any value_nontrivial{ NonTrivialClass{ 17 } };

		// Use move constructor
		value_int = 5;
		REQUIRE(value_int.has_value());
		REQUIRE(value_int.type_id() == Neat::get_id<int>());
		CHECK(value_int.value<int>() == 5);

		value_nontrivial = NonTrivialClass{7};
		REQUIRE(value_nontrivial.has_value());
		REQUIRE(value_nontrivial.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(value_nontrivial.value<NonTrivialClass>().get() == 7);
	}

	SECTION("Different storage mode (permutation 1)")
	{
		// Construct initial any's
		Neat::Any empty_any{};
		Neat::Any value_int{ 15 };
		Neat::Any value_nontrivial{ NonTrivialClass{ 17 } };

		// Use move constructor
		empty_any = 5;
		REQUIRE(empty_any.has_value());
		REQUIRE(empty_any.type_id() == Neat::get_id<int>());
		CHECK(empty_any.value<int>() == 5);

		value_int = NonTrivialClass{ 7 };
		REQUIRE(value_int.has_value());
		REQUIRE(value_int.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(value_int.value<NonTrivialClass>().get() == 7);
	}

	SECTION("Different storage mode (permutation 2)")
	{
		// Construct initial any's
		Neat::Any empty_any{};
		Neat::Any value_int{ 15 };
		Neat::Any value_nontrivial{ NonTrivialClass{ 17 } };

		// Use move constructor
		empty_any = NonTrivialClass{7};
		REQUIRE(empty_any.has_value());
		REQUIRE(empty_any.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(empty_any.value<NonTrivialClass>().get() == 7);

		value_nontrivial = 5;
		REQUIRE(value_nontrivial.has_value());
		REQUIRE(value_nontrivial.type_id() == Neat::get_id<int>());
		CHECK(value_nontrivial.value<int>() == 5);
	}
}

TEST_CASE("Any Copy Constructor")
{
	// Construct initial any's
	Neat::Any empty_any{};
	Neat::Any value_int{ 5 };
	Neat::Any value_nontrivial{ NonTrivialClass{ 7 } };

	// Use copy constructor
	Neat::Any copy_empty_any{ empty_any };
	CHECK(!copy_empty_any.has_value());

	Neat::Any copy_value_int{ value_int };
	REQUIRE(copy_value_int.has_value());
	REQUIRE(copy_value_int.type_id() == Neat::get_id<int>());
	CHECK(copy_value_int.value<int>() == 5);

	Neat::Any copy_value_nontrivial{ value_nontrivial };
	REQUIRE(copy_value_nontrivial.has_value());
	REQUIRE(copy_value_nontrivial.type_id() == Neat::get_id<NonTrivialClass>());
	CHECK(copy_value_nontrivial.value<NonTrivialClass>().get() == 7);
}

TEST_CASE("Any Move Constructor")
{
	// Construct initial any's
	Neat::Any empty_any{};
	Neat::Any value_int{ 5 };
	Neat::Any value_nontrivial{ NonTrivialClass{ 7 } };

	// Use move constructor
	Neat::Any moved_empty_any{ std::move(empty_any) };
	CHECK(!empty_any.has_value());
	CHECK(empty_any.type_id() == Neat::c_empty_type_id);
	CHECK(!moved_empty_any.has_value());
	CHECK(moved_empty_any.type_id() == Neat::c_empty_type_id);

	Neat::Any moved_value_int{ std::move(value_int) };
	CHECK(!value_int.has_value());
	CHECK(value_int.type_id() == Neat::c_empty_type_id);
	REQUIRE(moved_value_int.has_value());
	REQUIRE(moved_value_int.type_id() == Neat::get_id<int>());
	CHECK(moved_value_int.value<int>() == 5);

	Neat::Any moved_value_nontrivial{ std::move(value_nontrivial) };
	CHECK(!value_nontrivial.has_value());
	CHECK(value_nontrivial.type_id() == Neat::c_empty_type_id);
	REQUIRE(moved_value_nontrivial.has_value());
	REQUIRE(moved_value_nontrivial.type_id() == Neat::get_id<NonTrivialClass>());
	CHECK(moved_value_nontrivial.value<NonTrivialClass>().get() == 7);
}

TEST_CASE("Any Copy Assignment Operator")
{
	SECTION("Same storage mode")
	{
		// Construct initial any's
		Neat::Any from_empty_any{};
		Neat::Any from_value_int{ 5 };
		Neat::Any from_value_nontrivial{ NonTrivialClass{ 7 } };

		Neat::Any to_empty_any{};
		Neat::Any to_value_int{ 15 };
		Neat::Any to_value_nontrivial{ NonTrivialClass{ 17 } };

		// Use move constructor
		to_empty_any = from_empty_any;
		CHECK(!from_empty_any.has_value());
		CHECK(from_empty_any.type_id() == Neat::c_empty_type_id);
		CHECK(!to_empty_any.has_value());
		CHECK(to_empty_any.type_id() == Neat::c_empty_type_id);

		to_value_int = from_value_int;
		REQUIRE(from_value_int.has_value());
		REQUIRE(from_value_int.type_id() == Neat::get_id<int>());
		CHECK(from_value_int.value<int>() == 5);
		REQUIRE(to_value_int.has_value());
		REQUIRE(to_value_int.type_id() == Neat::get_id<int>());
		CHECK(to_value_int.value<int>() == 5);

		to_value_nontrivial = from_value_nontrivial;
		REQUIRE(from_value_nontrivial.has_value());
		REQUIRE(from_value_nontrivial.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(from_value_nontrivial.value<NonTrivialClass>().get() == 7);
		REQUIRE(to_value_nontrivial.has_value());
		REQUIRE(to_value_nontrivial.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(to_value_nontrivial.value<NonTrivialClass>().get() == 7);
	}

	SECTION("Different storage mode (permutation 1)")
	{
		// Construct initial any's
		Neat::Any from_empty_any{};
		Neat::Any from_value_int{ 5 };
		Neat::Any from_value_nontrivial{ NonTrivialClass{ 7 } };

		Neat::Any to_empty_any{};
		Neat::Any to_value_int{ 15 };
		Neat::Any to_value_nontrivial{ NonTrivialClass{ 17 } };

		// Use move constructor
		to_empty_any = from_value_int;
		REQUIRE(from_value_int.has_value());
		REQUIRE(from_value_int.type_id() == Neat::get_id<int>());
		CHECK(from_value_int.value<int>() == 5);
		REQUIRE(to_empty_any.has_value());
		REQUIRE(to_empty_any.type_id() == Neat::get_id<int>());
		CHECK(to_empty_any.value<int>() == 5);

		to_value_int = from_value_nontrivial;
		REQUIRE(from_value_nontrivial.has_value());
		REQUIRE(from_value_nontrivial.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(from_value_nontrivial.value<NonTrivialClass>().get() == 7);
		REQUIRE(to_value_int.has_value());
		REQUIRE(to_value_int.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(to_value_int.value<NonTrivialClass>().get() == 7);

		to_value_nontrivial = from_empty_any;
		CHECK(!from_empty_any.has_value());
		CHECK(from_empty_any.type_id() == Neat::c_empty_type_id);
		CHECK(!to_value_nontrivial.has_value());
		CHECK(to_value_nontrivial.type_id() == Neat::c_empty_type_id);
	}

	SECTION("Different storage mode (permutation 2)")
	{
		// Construct initial any's
		Neat::Any from_empty_any{};
		Neat::Any from_value_int{ 5 };
		Neat::Any from_value_nontrivial{ NonTrivialClass{ 7 } };

		Neat::Any to_empty_any{};
		Neat::Any to_value_int{ 15 };
		Neat::Any to_value_nontrivial{ NonTrivialClass{ 17 } };

		// Use move constructor
		to_empty_any = from_value_nontrivial;
		REQUIRE(from_value_nontrivial.has_value());
		REQUIRE(from_value_nontrivial.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(from_value_nontrivial.value<NonTrivialClass>().get() == 7);
		REQUIRE(to_empty_any.has_value());
		REQUIRE(to_empty_any.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(to_empty_any.value<NonTrivialClass>().get() == 7);

		to_value_int = from_empty_any;
		CHECK(!from_empty_any.has_value());
		CHECK(from_empty_any.type_id() == Neat::c_empty_type_id);
		CHECK(!to_value_int.has_value());
		CHECK(to_value_int.type_id() == Neat::c_empty_type_id);

		to_value_nontrivial = from_value_int;
		REQUIRE(from_value_int.has_value());
		REQUIRE(from_value_int.type_id() == Neat::get_id<int>());
		CHECK(from_value_int.value<int>() == 5);
		REQUIRE(to_value_nontrivial.has_value());
		REQUIRE(to_value_nontrivial.type_id() == Neat::get_id<int>());
		CHECK(to_value_nontrivial.value<int>() == 5);
	}
}

TEST_CASE("Any Move Assignment Operator")
{
	SECTION("Same storage mode")
	{
		// Construct initial any's
		Neat::Any from_empty_any{};
		Neat::Any from_value_int{ 5 };
		Neat::Any from_value_nontrivial{ NonTrivialClass{ 7 } };

		Neat::Any to_empty_any{};
		Neat::Any to_value_int{ 15 };
		Neat::Any to_value_nontrivial{ NonTrivialClass{ 17 } };

		// Use move constructor
		to_empty_any = std::move(from_empty_any);
		CHECK(!from_empty_any.has_value());
		CHECK(from_empty_any.type_id() == Neat::c_empty_type_id);
		CHECK(!to_empty_any.has_value());
		CHECK(to_empty_any.type_id() == Neat::c_empty_type_id);

		to_value_int = std::move(from_value_int);
		CHECK(!from_value_int.has_value());
		CHECK(from_value_int.type_id() == Neat::c_empty_type_id);
		REQUIRE(to_value_int.has_value());
		REQUIRE(to_value_int.type_id() == Neat::get_id<int>());
		CHECK(to_value_int.value<int>() == 5);

		to_value_nontrivial = std::move(from_value_nontrivial);
		CHECK(!from_value_nontrivial.has_value());
		CHECK(from_value_nontrivial.type_id() == Neat::c_empty_type_id);
		REQUIRE(to_value_nontrivial.has_value());
		REQUIRE(to_value_nontrivial.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(to_value_nontrivial.value<NonTrivialClass>().get() == 7);
	}

	SECTION("Different storage mode (permutation 1)")
	{
		// Construct initial any's
		Neat::Any from_empty_any{};
		Neat::Any from_value_int{ 5 };
		Neat::Any from_value_nontrivial{ NonTrivialClass{ 7 } };

		Neat::Any to_empty_any{};
		Neat::Any to_value_int{ 15 };
		Neat::Any to_value_nontrivial{ NonTrivialClass{ 17 } };

		// Use move constructor
		to_empty_any = std::move(from_value_int);
		CHECK(!from_value_int.has_value());
		CHECK(from_value_int.type_id() == Neat::c_empty_type_id);
		REQUIRE(to_empty_any.has_value());
		REQUIRE(to_empty_any.type_id() == Neat::get_id<int>());
		CHECK(to_empty_any.value<int>() == 5);

		to_value_int = std::move(from_value_nontrivial);
		CHECK(!from_value_nontrivial.has_value());
		CHECK(from_value_nontrivial.type_id() == Neat::c_empty_type_id);
		REQUIRE(to_value_int.has_value());
		REQUIRE(to_value_int.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(to_value_int.value<NonTrivialClass>().get() == 7);

		to_value_nontrivial = std::move(from_empty_any);
		CHECK(!from_empty_any.has_value());
		CHECK(from_empty_any.type_id() == Neat::c_empty_type_id);
		CHECK(!to_value_nontrivial.has_value());
		CHECK(to_value_nontrivial.type_id() == Neat::c_empty_type_id);
	}

	SECTION("Different storage mode (permutation 2)")
	{
		// Construct initial any's
		Neat::Any from_empty_any{};
		Neat::Any from_value_int{ 5 };
		Neat::Any from_value_nontrivial{ NonTrivialClass{ 7 } };

		Neat::Any to_empty_any{};
		Neat::Any to_value_int{ 15 };
		Neat::Any to_value_nontrivial{ NonTrivialClass{ 17 } };

		// Use move constructor
		to_empty_any = std::move(from_value_nontrivial);
		CHECK(!from_value_nontrivial.has_value());
		CHECK(from_value_nontrivial.type_id() == Neat::c_empty_type_id);
		REQUIRE(to_empty_any.has_value());
		REQUIRE(to_empty_any.type_id() == Neat::get_id<NonTrivialClass>());
		CHECK(to_empty_any.value<NonTrivialClass>().get() == 7);

		to_value_int = std::move(from_empty_any);
		CHECK(!from_empty_any.has_value());
		CHECK(from_empty_any.type_id() == Neat::c_empty_type_id);
		CHECK(!to_value_int.has_value());
		CHECK(to_value_int.type_id() == Neat::c_empty_type_id);

		to_value_nontrivial = std::move(from_value_int);
		CHECK(!from_value_int.has_value());
		CHECK(from_value_int.type_id() == Neat::c_empty_type_id);
		REQUIRE(to_value_nontrivial.has_value());
		REQUIRE(to_value_nontrivial.type_id() == Neat::get_id<int>());
		CHECK(to_value_nontrivial.value<int>() == 5);
	}
}
