module;

#include <iostream>
#include <format>

module TestModule1;

void MyFunc()
{
	std::cout << "Hello Module World!\n";
}

void MyStruct::helper_function()
{
	std::cout << "Module Method Called!\n";
}

void MyStruct::argumented_function(int i, int)
{
	std::cout << "Module Argumented Method Called With:" << i << "!\n";
}

const char* MyStruct::argumented_function2(const char* prefix, int i, float f)
{
	static std::string s;
	s = std::format("{}: {} {:.1f}", prefix, i, f);
	return s.c_str();
}

int MyStruct::get_42()
{
	return 42;
}
