#include "catch2/catch_all.hpp"
#include "neat/Reflection.h"

import DependantModule;


TEST_CASE("Struct with member which type is externally defined")
{
	auto composite_struct_type = Neat::get_type<CompositeStruct>();
	REQUIRE(composite_struct_type != nullptr);

	REQUIRE(composite_struct_type->fields.size() >= 1);
	const auto& field_0 = composite_struct_type->fields[0];
	
	auto field_type = Neat::get_type(field_0.type);
	CHECK(field_type->name == "MyStruct");
}
