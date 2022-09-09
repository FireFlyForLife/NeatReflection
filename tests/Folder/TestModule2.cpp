module;

#include <iostream>

module TestModule2;

void MyFunc2()
{
	std::cout << "Hello Module World!\n";
}

void MyStruct2::helper_function()
{
	std::cout << "Module Method Called!\n";
}

void MyStruct2::argumented_function(int i, int)
{
	std::cout << "Module Argumented Method Called With:" << i << "!\n";
}

int MyStruct2::get_42()
{
	return 42;
}
