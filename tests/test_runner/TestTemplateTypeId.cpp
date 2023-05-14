#include "catch2/catch_all.hpp"
#include "neat/TemplateTypeId.h"

#include <type_traits>


constexpr Neat::TemplateTypeId c_manual_id_number = 5'000'001;

struct SomeStructA {};
struct SomeStructB {};
struct SomeStructC { using ManualId = std::integral_constant<Neat::TemplateTypeId, c_manual_id_number>; };

TEST_CASE("Neat::get_id() returns consistent id's")
{
	const auto a_id = Neat::get_id<SomeStructA>();
	const auto a_id2 = Neat::get_id<SomeStructA>();
	const auto b_id = Neat::get_id<SomeStructB>();
	const auto c_id = Neat::get_id<SomeStructC>();
	const auto b_id2 = Neat::get_id<SomeStructB>();
	const auto c_id2 = Neat::get_id<SomeStructC>();

	CHECK(a_id == a_id2);
	CHECK(b_id == b_id2);
	CHECK(c_id == c_id2);
}

TEST_CASE("Neat::get_id() returns unique id's")
{
	const auto a_id = Neat::get_id<SomeStructA>();
	const auto b_id = Neat::get_id<SomeStructB>();
	const auto c_id = Neat::get_id<SomeStructC>();

	CHECK(a_id != b_id);
	CHECK(b_id != c_id);
}

TEST_CASE("Neat::get_id() with manual id override")
{
	const auto c_id = Neat::get_id<SomeStructC>();
	CHECK(c_id == c_manual_id_number);
}
