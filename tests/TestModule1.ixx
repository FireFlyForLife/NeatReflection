module;
#include "Neat/ReflectPrivateMembers.h"
export module TestModule1;

export void MyFunc();

export struct MyBaseStruct { int health; };

export struct MyStruct : MyBaseStruct
{
	double damage;

	void helper_function();
	void argumented_function(int i, int);
	int get_42();
};

export class MyClass 
{
public:
	MyClass() : i(42)/*, d(5.0)*/ {}

	int i;

private:
	double d;

	REFLECT_PRIVATE_MEMBERS;
};

class NonExportedClass {};
