module;

#include <iostream>

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

int MyStruct::get_42()
{
	return 42;
}
