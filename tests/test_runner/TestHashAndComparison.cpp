#include "catch2/catch_all.hpp"
#include "neat/Reflection.h"


struct HashTestTypeBase {};
struct HashTestTypeBase2 {};
struct HashTestType : HashTestTypeBase
{
	int field_first;
	double field_second;

	void method_first() {}
	void method_second(int*) {}
};

Neat::Type create_hash_test_base_type()
{
	return Neat::Type::create<HashTestTypeBase>("HashTestTypeBase", Neat::get_id<HashTestTypeBase>(),
		{ },
		{ },
		{ }
	);
}

Neat::Type create_hash_test_type()
{
	return Neat::Type::create<HashTestType>("HashTestType", Neat::get_id<HashTestType>(),
		{ Neat::BaseClass{ Neat::get_id<HashTestTypeBase>(), Neat::Access::Public }, Neat::BaseClass{ Neat::get_id<HashTestTypeBase2>(), Neat::Access::Public } },
		{ Neat::Field::create<HashTestType, int, &HashTestType::field_first>("field_first", Neat::Access::Public), Neat::Field::create<HashTestType, double, &HashTestType::field_second>("field_second", Neat::Access::Public) },
		{ Neat::Method::create<&HashTestType::method_first, HashTestType, void>("method_first", Neat::Access::Public), Neat::Method::create<&HashTestType::method_second, HashTestType, void, int*>("method_second", Neat::Access::Public) }
	);
}

TEST_CASE("Test std::hash<Neat::Type>")
{
	auto type = create_hash_test_type();
	std::hash<Neat::Type> hasher{};

	size_t hash = hasher(type);
	CHECK(hash == type.id);
}

TEST_CASE("Test std::hash<Neat::Field>")
{
	auto type = create_hash_test_type();
	auto& field = type.fields[0];
	std::hash<Neat::Field> hasher{};

	size_t hash = hasher(field);

	// This makes this a bit of a weak test, as it's directly using the code in the implementation. 
	// But it's primarily meant so instantiate the functions
	size_t expected_h = 0;
	Neat::HashUtils::combine(expected_h, field.object_type, field.name);

	CHECK(hash == expected_h);
}

TEST_CASE("Test std::hash<Neat::Method>")
{
	auto type = create_hash_test_type();
	auto& method = type.methods[0];
	std::hash<Neat::Method> hasher{};

	size_t hash = hasher(method);

	// This makes this a bit of a weak test, as it's directly using the code in the implementation. 
	// But it's primarily meant so instantiate the functions
	size_t expected_h = 0;

	for (const auto& argument_type : method.argument_types)
	{
		Neat::HashUtils::combine(expected_h, argument_type);
	}
	Neat::HashUtils::combine(expected_h, method.object_type, method.return_type, method.name);

	CHECK(hash == expected_h);
}

TEST_CASE("Test std::hash<Neat::BaseClass>")
{
	auto type = create_hash_test_type();
	auto& base_class = type.bases[0];
	std::hash<Neat::BaseClass> hasher{};

	size_t hash = hasher(base_class);
	CHECK(hash == base_class.base_id);
}

TEST_CASE("Test Neat::Type operator<=>")
{
	auto type_a = create_hash_test_type();
	auto type_b = create_hash_test_base_type();

	auto a_to_a_cmp = (type_a <=> type_a);
	CHECK(a_to_a_cmp == std::strong_ordering::equal);

	auto a_to_b_cmp = (type_a <=> type_b);
	auto expected_a_to_b = (type_a.id <=> type_b.id);
	CHECK(a_to_b_cmp == expected_a_to_b);

	auto b_to_a_cmp = (type_b <=> type_a);
	auto expected_b_to_a = (type_b.id <=> type_a.id);
	CHECK(b_to_a_cmp == expected_b_to_a);

	auto b_to_b_cmp = (type_b <=> type_b);
	CHECK(b_to_b_cmp == std::strong_ordering::equal);
}

TEST_CASE("Test Neat::Type operator==")
{
	auto type_a = create_hash_test_type();
	auto type_b = create_hash_test_base_type();

	auto a_to_a_cmp = (type_a == type_a);
	CHECK(a_to_a_cmp == true);
	auto a_to_a_not = (type_a != type_a);
	CHECK(a_to_a_not == false);

	auto a_to_b_cmp = (type_a == type_b);
	CHECK(a_to_b_cmp == false);
	auto a_to_b_not = (type_a != type_b);
	CHECK(a_to_b_not == true);

	auto b_to_a_cmp = (type_b == type_a);
	CHECK(b_to_a_cmp == false);
	auto b_to_a_not = (type_b != type_a);
	CHECK(b_to_a_not == true);

	auto b_to_b_cmp = (type_b == type_b);
	CHECK(b_to_b_cmp == true);
	auto b_to_b_not = (type_b != type_b);
	CHECK(b_to_b_not == false);
}

TEST_CASE("Test Neat::BaseClass operator<=>")
{
	auto type_a = create_hash_test_type();
	REQUIRE(type_a.bases.size() == 2);

	const auto& base_class_a = type_a.bases[0];
	const auto& base_class_b = type_a.bases[1];

	auto a_to_a_cmp = (base_class_a <=> base_class_a);
	CHECK(a_to_a_cmp == std::strong_ordering::equal);

	auto a_to_b_cmp = (base_class_a <=> base_class_b);
	auto expected_a_to_b = (base_class_a.base_id <=> base_class_b.base_id);
	CHECK(a_to_b_cmp == expected_a_to_b);

	auto b_to_a_cmp = (base_class_b <=> base_class_a);
	auto expected_b_to_a = (base_class_b.base_id <=> base_class_a.base_id);
	CHECK(b_to_a_cmp == expected_b_to_a);

	auto b_to_b_cmp = (base_class_b <=> base_class_b);
	CHECK(b_to_b_cmp == std::strong_ordering::equal);
}

TEST_CASE("Test Neat::BaseClass operator==")
{
	auto type_a = create_hash_test_type();
	REQUIRE(type_a.bases.size() == 2);

	const auto& base_class_a = type_a.bases[0];
	const auto& base_class_b = type_a.bases[1];

	auto a_to_a_cmp = (base_class_a == base_class_a);
	CHECK(a_to_a_cmp == true);
	auto a_to_a_not = (base_class_a != base_class_a);
	CHECK(a_to_a_not == false);

	auto a_to_b_cmp = (base_class_a == base_class_b);
	CHECK(a_to_b_cmp == false);
	auto a_to_b_not = (base_class_a != base_class_b);
	CHECK(a_to_b_not == true);

	auto b_to_a_cmp = (base_class_b == base_class_a);
	CHECK(b_to_a_cmp == false);
	auto b_to_a_not = (base_class_b != base_class_a);
	CHECK(b_to_a_not == true);

	auto b_to_b_cmp = (base_class_b == base_class_b);
	CHECK(b_to_b_cmp == true);
	auto b_to_b_not = (base_class_b != base_class_b);
	CHECK(b_to_b_not == false);
}

TEST_CASE("Test Neat::Field operator<=>")
{
	auto type_a = create_hash_test_type();
	REQUIRE(type_a.fields.size() == 2);

	const auto& field_a = type_a.fields[0];
	const auto& field_b = type_a.fields[1];

	auto a_to_a_cmp = (field_a <=> field_a);
	CHECK(a_to_a_cmp == std::strong_ordering::equal);

	auto a_to_b_cmp = (field_a <=> field_b);
	auto expected_a_to_b = (field_a.name <=> field_b.name); // It's already the same object.
	CHECK(a_to_b_cmp == expected_a_to_b);

	auto b_to_a_cmp = (field_b <=> field_a);
	auto expected_b_to_a = (field_b.name <=> field_a.name); // It's already the same object.
	CHECK(b_to_a_cmp == expected_b_to_a);

	auto b_to_b_cmp = (field_b <=> field_b);
	CHECK(b_to_b_cmp == std::strong_ordering::equal);
}

TEST_CASE("Test Neat::Field operator==")
{
	auto type_a = create_hash_test_type();
	REQUIRE(type_a.fields.size() == 2);

	const auto& field_a = type_a.fields[0];
	const auto& field_b = type_a.fields[1];

	auto a_to_a_cmp = (field_a == field_a);
	CHECK(a_to_a_cmp == true);
	auto a_to_a_not = (field_a != field_a);
	CHECK(a_to_a_not == false);

	auto a_to_b_cmp = (field_a == field_b);
	CHECK(a_to_b_cmp == false);
	auto a_to_b_not = (field_a != field_b);
	CHECK(a_to_b_not == true);

	auto b_to_a_cmp = (field_b == field_a);
	CHECK(b_to_a_cmp == false);
	auto b_to_a_not = (field_b != field_a);
	CHECK(b_to_a_not == true);

	auto b_to_b_cmp = (field_b == field_b);
	CHECK(b_to_b_cmp == true);
	auto b_to_b_not = (field_b != field_b);
	CHECK(b_to_b_not == false);
}

TEST_CASE("Test Neat::Method operator<=>")
{
	auto type_a = create_hash_test_type();
	REQUIRE(type_a.methods.size() == 2);

	const auto& method_a = type_a.methods[0];
	const auto& method_b = type_a.methods[1];

	auto a_to_a_cmp = (method_a <=> method_a);
	CHECK(a_to_a_cmp == std::strong_ordering::equal);

	auto a_to_b_cmp = (method_a <=> method_b);
	auto expected_a_to_b = (method_a.name <=> method_b.name); // It's already the same object.
	CHECK(a_to_b_cmp == expected_a_to_b);

	auto b_to_a_cmp = (method_b <=> method_a);
	auto expected_b_to_a = (method_b.name <=> method_a.name); // It's already the same object.
	CHECK(b_to_a_cmp == expected_b_to_a);

	auto b_to_b_cmp = (method_b <=> method_b);
	CHECK(b_to_b_cmp == std::strong_ordering::equal);
}

TEST_CASE("Test Neat::Method operator==")
{
	auto type_a = create_hash_test_type();
	REQUIRE(type_a.methods.size() == 2);

	const auto& method_a = type_a.methods[0];
	const auto& method_b = type_a.methods[1];

	auto a_to_a_cmp = (method_a == method_a);
	CHECK(a_to_a_cmp == true);
	auto a_to_a_not = (method_a != method_a);
	CHECK(a_to_a_not == false);

	auto a_to_b_cmp = (method_a == method_b);
	CHECK(a_to_b_cmp == false);
	auto a_to_b_not = (method_a != method_b);
	CHECK(a_to_b_not == true);

	auto b_to_a_cmp = (method_b == method_a);
	CHECK(b_to_a_cmp == false);
	auto b_to_a_not = (method_b != method_a);
	CHECK(b_to_a_not == true);

	auto b_to_b_cmp = (method_b == method_b);
	CHECK(b_to_b_cmp == true);
	auto b_to_b_not = (method_b != method_b);
	CHECK(b_to_b_not == false);
}