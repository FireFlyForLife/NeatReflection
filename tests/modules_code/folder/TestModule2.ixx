module;
#include <array>
export module TestModule2;

export void MyFunc2();

export struct MyBaseStruct2 { int health; };

export struct MyStruct2 : MyBaseStruct2
{
	double damage;
	unsigned int speed;
	std::array<int, 4> four_ints;

	void helper_function();
	void argumented_function(int i, int);
	int get_42();
};

struct NonExportedStruct {};
export struct MyStructWithPrivateType 
{
	NonExportedStruct non_exported_type_field;
};