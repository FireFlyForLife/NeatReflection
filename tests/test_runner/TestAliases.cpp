#include "catch2/catch_all.hpp"

#include "neat/Reflection.h"

import TestModule1;

TEST_CASE("Test templated class with alias")
{
	auto templated_class_type = Neat::get_type<TemplatedClass<int, 3>>(); // This class is instantiated in `StructWithTemplatedClasses`
	REQUIRE(templated_class_type != nullptr);
	
	CHECK(templated_class_type->name == "TemplatedClass<int, 3>");
	CHECK(templated_class_type->id == Neat::get_id<TemplatedClass<int, 3>>());

	REQUIRE(templated_class_type->fields.size() == 2);
	auto& first_field = templated_class_type->fields[0];
	CHECK(first_field.name == "i");
	CHECK(first_field.type == Neat::get_id<int>());
	CHECK(first_field.access == Neat::Access::Public);

	auto& second_field = templated_class_type->fields[1];
	CHECK(second_field.name == "t_ptr");
	CHECK(second_field.type == Neat::get_id<int*>());
	CHECK(second_field.access == Neat::Access::Public);

	REQUIRE(templated_class_type->member_aliases.size() == 1);
	auto& first_alias = templated_class_type->member_aliases.front();
	CHECK(first_alias.name == "value_type");
	CHECK(first_alias.type == Neat::get_id<int>());
	CHECK(first_alias.access == Neat::Access::Public);

	REQUIRE(templated_class_type->template_arguments.size() == 2);
	auto& first_template_arg = templated_class_type->template_arguments[0];
	auto first_template_arg_type = std::get<Neat::TemplateTypeId>(first_template_arg.type_or_value); // Will throw exception for incorrect variant
	CHECK(first_template_arg_type == Neat::get_id<int>());
	
	auto& second_template_arg = templated_class_type->template_arguments[1];
	auto second_template_arg_value = std::get<Neat::Any>(second_template_arg.type_or_value); // Will throw exception for incorrect variant
	REQUIRE(second_template_arg_value.has_value());
	REQUIRE(second_template_arg_value.type_id() == Neat::get_id<int>()); // TODO: Also cast to the correct integer type, needs to be obtained from the TemplateDecl
	CHECK(second_template_arg_value.value<int>() == 3);
}

TEST_CASE("Struct with aliases")
{
	auto struct_type = Neat::get_type<StructWithAliases>();
	REQUIRE(struct_type != nullptr);

	CHECK(struct_type->name == "StructWithAliases");
	CHECK(struct_type->id == Neat::get_id<StructWithAliases>());

	REQUIRE(struct_type->member_aliases.size() == 4);
	auto& some_int_type_alias = struct_type->member_aliases[0];
	CHECK(some_int_type_alias.name == "SomeIntType");
	CHECK(some_int_type_alias.type == Neat::get_id<int>());
	CHECK(some_int_type_alias.access == Neat::Access::Public);

	auto& my_class_ref_alias = struct_type->member_aliases[1];
	CHECK(my_class_ref_alias.name == "MyClassRef");
	CHECK(my_class_ref_alias.type == Neat::get_id<MyClass&>());
	CHECK(my_class_ref_alias.access == Neat::Access::Public);

	auto& my_class_ptr_alias = struct_type->member_aliases[2];
	CHECK(my_class_ptr_alias.name == "MyClassPtr");
	CHECK(my_class_ptr_alias.type == Neat::get_id<MyClass*>());
	CHECK(my_class_ptr_alias.access == Neat::Access::Public);

	auto& private_double_type_alias = struct_type->member_aliases[3];
	CHECK(private_double_type_alias.name == "PrivateDoubleType");
	CHECK(private_double_type_alias.type == Neat::get_id<double>());
	CHECK(private_double_type_alias.access == Neat::Access::Private);

	REQUIRE(struct_type->fields.size() == 2);
	auto& my_i_field = struct_type->fields[0];
	CHECK(my_i_field.name == "my_i");
	CHECK(my_i_field.type == Neat::get_id<int>());
	CHECK(my_i_field.access == Neat::Access::Public);

	auto& my_class_ref_field = struct_type->fields[1];
	CHECK(my_class_ref_field.name == "my_class_ptr");
	CHECK(my_class_ref_field.type == Neat::get_id<MyClass*>());
	CHECK(my_class_ref_field.access == Neat::Access::Public);
}




TEST_CASE("Test templated class yadayada")
{
	auto uber_struct = Neat::get_type<StructWithTemplatedClasses>();
	REQUIRE(uber_struct != nullptr);
	auto templated_class_type = Neat::get_type(uber_struct->fields[0].type);
	REQUIRE(templated_class_type != nullptr);

	REQUIRE(!templated_class_type->member_aliases.empty());
	auto& first_alias = templated_class_type->member_aliases.front();
	CHECK(first_alias.name == "value_type");
	CHECK(first_alias.type == Neat::get_id<int>());
	CHECK(first_alias.access == Neat::Access::Public);
}
