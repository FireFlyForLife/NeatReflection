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
