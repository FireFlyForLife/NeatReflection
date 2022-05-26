export module TestModule1;

export void MyFunc();

export struct BaseStruct { int health; };

export struct MyStruct : BaseStruct
{
	double damage;

	void helper_function();
};