#include "catch2/catch_all.hpp"

#include "neat/Reflection.h"

import TestModuleWithTemplates;


TEST_CASE("Test templated class with arguments")
{
	// Query type that contains all the test templated instances
	auto struct_with_template_class = Neat::get_type<StructWithTemplatedClasses>();
	REQUIRE(struct_with_template_class != nullptr);

	CHECK(struct_with_template_class->name == "StructWithTemplatedClasses");
	CHECK(struct_with_template_class->id == Neat::get_id<StructWithTemplatedClasses>());
	REQUIRE(struct_with_template_class->fields.size() == 6);

	{
		// TemplatedClass<int, 3>
		auto templated_class_int_3 = Neat::get_type(struct_with_template_class->fields[0].type);
		REQUIRE(templated_class_int_3 != nullptr);
		CHECK(templated_class_int_3->name == "TemplatedClass<int, 3>");
		CHECK(templated_class_int_3->id == Neat::get_id<TemplatedClass<int, 3>>());

		REQUIRE(templated_class_int_3->template_arguments.size() == 2);
		auto first_template_arg = std::get<Neat::TemplateTypeId>(templated_class_int_3->template_arguments[0].type_or_value); // Will throw exception for incorrect variant
		CHECK(first_template_arg == Neat::get_id<int>());
		auto second_template_arg = std::get<Neat::Any>(templated_class_int_3->template_arguments[1].type_or_value); // Will throw exception for incorrect variant
		CHECK(second_template_arg.value<int>() == 3);
	}

	{
		// AnotherTemplatedClass<double>
		auto another_templated_class_double = Neat::get_type(struct_with_template_class->fields[1].type);
		REQUIRE(another_templated_class_double != nullptr);
		CHECK(another_templated_class_double->name == "AnotherTemplatedClass<double>");
		CHECK(another_templated_class_double->id == Neat::get_id<AnotherTemplatedClass<double>>());

		REQUIRE(another_templated_class_double->template_arguments.size() == 1);
		auto first_template_arg = std::get<Neat::TemplateTypeId>(another_templated_class_double->template_arguments[0].type_or_value); // Will throw exception for incorrect variant
		CHECK(first_template_arg == Neat::get_id<double>());
	}

	{
		// QuadrupleTemplatedClass<int, double, 42, SomeEmptyStruct, unsigned char>
		auto quadruple_templated_class = Neat::get_type(struct_with_template_class->fields[2].type);
		REQUIRE(quadruple_templated_class != nullptr);
		CHECK(quadruple_templated_class->name == "QuadrupleTemplatedClass<int, double, 42, SomeEmptyStruct, unsigned char>");
		CHECK(quadruple_templated_class->id == Neat::get_id<QuadrupleTemplatedClass<int, double, 42, SomeEmptyStruct, unsigned char>>());

		REQUIRE(quadruple_templated_class->template_arguments.size() == 5);
		auto first_template_arg = std::get<Neat::TemplateTypeId>(quadruple_templated_class->template_arguments[0].type_or_value); // Will throw exception for incorrect variant
		CHECK(first_template_arg == Neat::get_id<int>());
		auto second_template_arg = std::get<Neat::TemplateTypeId>(quadruple_templated_class->template_arguments[1].type_or_value); // Will throw exception for incorrect variant
		CHECK(second_template_arg == Neat::get_id<double>());
		auto third_template_arg = std::get<Neat::Any>(quadruple_templated_class->template_arguments[2].type_or_value); // Will throw exception for incorrect variant
		CHECK(third_template_arg.value<int>() == 42);
		auto fourth_template_arg = std::get<Neat::TemplateTypeId>(quadruple_templated_class->template_arguments[3].type_or_value); // Will throw exception for incorrect variant
		CHECK(fourth_template_arg == Neat::get_id<SomeEmptyStruct>());
		auto fifth_template_arg = std::get<Neat::TemplateTypeId>(quadruple_templated_class->template_arguments[4].type_or_value); // Will throw exception for incorrect variant
		CHECK(fifth_template_arg == Neat::get_id<unsigned char>());
	}

	{
		// LittleQuadrupleWrapperClass<int>
		auto little_quadruple_wrapper_class = Neat::get_type(struct_with_template_class->fields[3].type);
		REQUIRE(little_quadruple_wrapper_class != nullptr);
		CHECK(little_quadruple_wrapper_class->name == "LittleQuadrupleWrapperClass<int>");
		CHECK(little_quadruple_wrapper_class->id == Neat::get_id<LittleQuadrupleWrapperClass<int>>());

		REQUIRE(little_quadruple_wrapper_class->template_arguments.size() == 1);
		auto wrapper_first_template_arg = std::get<Neat::TemplateTypeId>(little_quadruple_wrapper_class->template_arguments[0].type_or_value); // Will throw exception for incorrect variant
		CHECK(wrapper_first_template_arg == Neat::get_id<int>());

		REQUIRE(little_quadruple_wrapper_class->fields.size() == 2);
		CHECK(little_quadruple_wrapper_class->fields[0].name == "ripple");
		CHECK(little_quadruple_wrapper_class->fields[0].type == Neat::get_id<int>());

		{
			// QuadrupleTemplatedClass<T, typename TransformIntTrait<T>::type, 42, LittleWrapper<T>, SomeEmptyStruct>
			// (Turns into: QuadrupleTemplatedClass<int, float, 42, LittleWrapper<int>, SomeEmptyStruct>)
			auto quadruple_templated_class = Neat::get_type(little_quadruple_wrapper_class->fields[1].type);
			REQUIRE(quadruple_templated_class != nullptr);
			CHECK(quadruple_templated_class->name == "QuadrupleTemplatedClass<int, float, 42, LittleWrapper<int>, SomeEmptyStruct>");
			CHECK(quadruple_templated_class->id == Neat::get_id<QuadrupleTemplatedClass<int, float, 42, LittleWrapper<int>, SomeEmptyStruct>>());
		
			REQUIRE(quadruple_templated_class->template_arguments.size() == 5);
			auto first_template_arg = std::get<Neat::TemplateTypeId>(quadruple_templated_class->template_arguments[0].type_or_value); // Will throw exception for incorrect variant
			CHECK(first_template_arg == Neat::get_id<int>());
			auto second_template_arg = std::get<Neat::TemplateTypeId>(quadruple_templated_class->template_arguments[1].type_or_value); // Will throw exception for incorrect variant
			CHECK(second_template_arg == Neat::get_id<float>());
			auto third_template_arg = std::get<Neat::Any>(quadruple_templated_class->template_arguments[2].type_or_value); // Will throw exception for incorrect variant
			CHECK(third_template_arg.value<int>() == 42);
			auto fourth_template_arg = std::get<Neat::TemplateTypeId>(quadruple_templated_class->template_arguments[3].type_or_value); // Will throw exception for incorrect variant
			CHECK(fourth_template_arg == Neat::get_id<LittleWrapper<int>>());
			auto fifth_template_arg = std::get<Neat::TemplateTypeId>(quadruple_templated_class->template_arguments[4].type_or_value); // Will throw exception for incorrect variant
			CHECK(fifth_template_arg == Neat::get_id<SomeEmptyStruct>());
		}
	}

	{
		// TransformForthTwoTrait<99>::type
		// (Turns into: IntWrapper<99>
		auto int_wrapper_99 = Neat::get_type(struct_with_template_class->fields[4].type);
		REQUIRE(int_wrapper_99 != nullptr);
		CHECK(int_wrapper_99->name == "IntWrapper<99>");
		CHECK(int_wrapper_99->id == Neat::get_id<IntWrapper<99>>());
	}

	{
		// TransformFortyTwoTrait<42>::type
		// (Turns into: IntWrapper<61453>
		auto int_wrapper_42 = Neat::get_type(struct_with_template_class->fields[5].type);
		REQUIRE(int_wrapper_42 != nullptr);
		CHECK(int_wrapper_42->name == "IntWrapper<61453>");
		CHECK(int_wrapper_42->id == Neat::get_id<IntWrapper<61453>>());
	}
}
