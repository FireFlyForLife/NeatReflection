#pragma once

namespace Neat::Experimental
{
	// Builtin attributes
	struct DoNotSerialise {};

#define EXTRA_ATTRIBUTE(field) using ___NeatReflectionExtraAttribute_1_##field
	EXTRA_ATTRIBUTE(f) = DoNotSerialise;

	template<auto PtrToMember, typename Attribute>
	struct MemberAttribute 
	{
		static constexpr auto member = PtrToMember;
		Attribute attribute;
	};



	// What should it look like?

	class SomeClass
	{
		// ...
	public:
		float f;

	private:
		int i;
	};

	constexpr MemberAttribute<&SomeClass::f, DoNotSerialise> _attr_some_class_f{};
}
