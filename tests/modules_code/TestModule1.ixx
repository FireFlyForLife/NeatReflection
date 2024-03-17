module;
#include "neat/ReflectPrivateMembers.h"
#include <string>
#include <unordered_map>
export module TestModule1;

export void MyFunc();

namespace Namespace {
	export void NamespacedFunction() {}
}

export struct MyBaseStruct { int health; };

export struct MyStruct : MyBaseStruct
{
	double damage;
	std::unordered_map<int, int> hash_map;

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
	NonExportedClass unexported_class;
	NonExportedClass return_unexported() { return NonExportedClass{}; }
	void unexported_param(NonExportedClass) {}
	void unexported_params(NonExportedClass a, NonExportedClass b) { (void)a; (void)b; }
	void half_unexported_params(int a, NonExportedClass b) { (void)a; (void)b; }

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

export class NonTrivialClass
{
public:
	NonTrivialClass();
	NonTrivialClass(int i);
	NonTrivialClass(const NonTrivialClass& other);
	NonTrivialClass(NonTrivialClass&& other);
	NonTrivialClass& operator=(const NonTrivialClass& other);
	NonTrivialClass& operator=(NonTrivialClass&& other);
	~NonTrivialClass();

	int get() const { return *data; };
	void set(int i) { *data = i; };

private:
	int* data = nullptr;
};

export struct StructWithAliases
{
	REFLECT_PRIVATE_MEMBERS;

	using SomeIntType = int;
	using MyClassRef = MyClass&;
	using MyClassPtr = MyClass*;

private:
	using PrivateDoubleType = double;

public:
	SomeIntType my_i;
	MyClassRef my_class_ref; // This field won't be reflected because you can't get a pointer to member if the type is a reference.
	MyClassPtr my_class_ptr;
};

export template<typename T, size_t I>
class TemplatedClass
{
public:
	using value_type = T;

	int i = I;
	T* t_ptr;


	void use_t(T& t) { (void)t; }
	T get_t_as_42() const { return static_cast<T>(42); }
};

// TODO: Support this, see issue #12
//export template<typename T>
//class AnotherTemplatedClass
//{
//public:
//	T a_value;
//	T b_value{};
//
//	TemplatedClass<T, 42> sub_templated_value;
//};

export struct StructWithTemplatedClasses
{
	TemplatedClass<int, 3> member_1;
	//AnotherTemplatedClass<double> member_2;
};
