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

export struct SomeStruct1 {};
export struct SomeStruct2 {};
export struct SomeStruct3 {};

export struct SomeStruct4 {};

export struct SomeStruct5 {};
