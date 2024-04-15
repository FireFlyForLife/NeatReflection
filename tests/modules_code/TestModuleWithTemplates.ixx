export module TestModuleWithTemplates;


export struct SomeEmptyStruct
{
	// Make it act like an int a little bit
	SomeEmptyStruct(int i) { (void)i; }
	explicit operator int() const { return 777; }
};

export template<typename T, size_t I>
class TemplatedClass
{
public:
	// Aliases
	using value_type = T;

	// Fields
	int i = I;
	T* t_ptr;

	// Methods
	void use_t(T& t) { (void)t; }
	T get_t_as_42() const { return static_cast<T>(42); }
};

export template<typename T>
class AnotherTemplatedClass
{
public:
	T a_value;
	T b_value{};

	TemplatedClass<T, 42> sub_templated_value;
};

export template<typename T1, typename T2, int I3, typename T4, typename T5>
class QuadrupleTemplatedClass
{
public:
	T1 a;
	T2 b;
	TemplatedClass<T4, I3> c;
	T4 d;
	T5 e;
};

export template<typename T>
struct LittleWrapper
{
	T value;
};

export template<int I> 
struct IntWrapper
{
	const int value = I;
};

template<typename T> struct TransformIntTrait { using type = T; };
template<> struct TransformIntTrait<int> { using type = float; };
template<typename T> struct TransformIntTrait<LittleWrapper<T>> { using type = SomeEmptyStruct; };

template<int I> struct TransformFortyTwoTrait { using type = IntWrapper<I>; };
template<> struct TransformFortyTwoTrait<42> { using type = IntWrapper<0xF00D>; };

export template<typename T>
class LittleQuadrupleWrapperClass
{
public:
	T ripple; // Haha, get it? Because it spells tripple! :D

	QuadrupleTemplatedClass<T, typename TransformIntTrait<T>::type, 42, LittleWrapper<T>, SomeEmptyStruct> tripple;
};

export struct StructWithTemplatedClasses
{
	TemplatedClass<int, 3> member_1;
	AnotherTemplatedClass<double> member_2;
	QuadrupleTemplatedClass<int, double, 42, SomeEmptyStruct, unsigned char> member_3;
	LittleQuadrupleWrapperClass<int> member_4;
	TransformFortyTwoTrait<99>::type member_5;
	TransformFortyTwoTrait<42>::type member_6;
};
