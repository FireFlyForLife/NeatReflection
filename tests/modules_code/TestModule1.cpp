module;

#include <iostream>
#include <format>
#include <string>

module TestModule1;

void MyFunc()
{
	std::cout << "Hello Module World!\n";
}

void MyStruct::helper_function()
{
	std::cout << "Module Method Called!\n";
}

void MyStruct::argumented_function(int i, int j)
{
	std::cout << "Module Argumented Method Called With: " << i << " and " << j << "!\n";
}

int MyStruct::get_42()
{
	return 42;
}

void MethodTester::TestMethod2(int i, int j)
{
	test_method_2_args = std::format("{},{}", i, j);
}

int MethodTester::TestMethodReturn()
{
	return test_method_return_value;
}

int MethodTester::TestMethodConstReturn() const
{
	return test_method_const_return_value;
}

NonTrivialClass::NonTrivialClass()
	: data(new int(0))
{
}

NonTrivialClass::NonTrivialClass(int i)
	: data(new int(i))
{

}

NonTrivialClass::NonTrivialClass(const NonTrivialClass& other)
	: data(new int{ *other.data })
{
}

NonTrivialClass::NonTrivialClass(NonTrivialClass&& other)
	: data(other.data)
{
	other.data = nullptr;
}

NonTrivialClass& NonTrivialClass::operator=(const NonTrivialClass& other)
{
	if (&other == this) {
		return *this;
	}

	delete data;
	data = new int(*other.data);
	return *this;
}

NonTrivialClass& NonTrivialClass::operator=(NonTrivialClass&& other)
{
	if (&other == this) {
		return *this;
	}

	delete data;
	data = other.data;
	other.data = nullptr;
	return *this;
}

NonTrivialClass::~NonTrivialClass()
{
	delete data;
}