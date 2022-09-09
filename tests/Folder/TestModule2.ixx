export module TestModule2;

export void MyFunc2();

export struct MyBaseStruct2 { int health; };

export struct MyStruct2 : MyBaseStruct2
{
	double damage;

	void helper_function();
	void argumented_function(int i, int);
	int get_42();
};