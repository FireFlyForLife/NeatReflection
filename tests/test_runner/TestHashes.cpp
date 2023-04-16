#include "catch2/catch_all.hpp"
#include "Neat/Reflection.h"


struct HashTestTypeBase {};
struct HashTestType : HashTestTypeBase
{
	int field_first;

	void method_first() {}
};

Neat::Type create_hash_test_type()
{
	return Neat::Type{ "HashTestTypes::ForType", Neat::get_id<HashTestType>(),
		{ Neat::BaseClass{ Neat::get_id<HashTestTypeBase>(), Neat::Access::Public } },
		{ Neat::Field::create<HashTestType, int, &HashTestType::field_first>("field_first", Neat::Access::Public) },
		{ Neat::Method::create<&HashTestType::method_first, HashTestType, void>("method_first", Neat::Access::Public) }
	};
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
	Neat::HashUtils::combine(expected_h, field.type, field.name);

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
	Neat::HashUtils::combine(expected_h, method.object_type, method.name);

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