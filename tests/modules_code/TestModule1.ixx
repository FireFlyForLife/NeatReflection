module;
#include "neat/ReflectPrivateMembers.h"
#include <string>
export module TestModule1;

export void MyFunc();

namespace Namespace {
	export void NamespacedFunction() {}
}

export struct MyBaseStruct { int health; };

export struct MyStruct : MyBaseStruct
{
	double damage;

	void helper_function();
	void argumented_function(int i, int j);
	int get_42();
};

class NonExportedClass {};

export class MyClass 
{
public:
	MyClass() : i(42), d(5.0) {}

	int i;
	NonExportedClass ptr_to_unexported;
	NonExportedClass return_unexpected() { return NonExportedClass{}; }
	void unexpected_param(NonExportedClass) {}
	void unexpected_params(NonExportedClass a, NonExportedClass b) { (void)a; (void)b; }
	void half_unexpected_params(int a, NonExportedClass b) { (void)a; (void)b; }

private:
	void modify_d() { d = 0.42; }
	double d;

	REFLECT_PRIVATE_MEMBERS;
};

export class MethodTester
{
public:
	std::string test_method_2_args;
	void TestMethod2(int i, int j);

	constexpr static int test_method_return_value = 50;
	int TestMethodReturn();
	constexpr static int test_method_const_return_value = 51;
	int TestMethodConstReturn() const;
};

export class ClassWithUnreflectedPrivates
{
	double private_d;
	int private_i;
};

export namespace ExportedNamespace
{
	class StillExportedClass {};
}

namespace NormalNamespace
{
	export class ExplicitlyExportedClass {};

	class NotExportedClass {};
}