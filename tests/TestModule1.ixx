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