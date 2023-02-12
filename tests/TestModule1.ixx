module;
#include "Neat/ReflectPrivateMembers.h"
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
	void argumented_function(int i, int);
	int get_42();
	const char* argumented_function2(const char* prefix, int i, float f);
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