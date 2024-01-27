// C++ trick, get a unique and consistent number per template type.
// NOTE: Automatic type id generation doesn't work for dynamic libraries (.dll .so), since each dll has their own id counter.
#pragma once
#include "neat/Defines.h"

#include <cstdint>


namespace Neat
{
	// TemplateTypeId definition
	using TemplateTypeId = uint32_t;

	inline constexpr TemplateTypeId c_empty_type_id = 0;

	// Automatic type id generation
	REFL_API TemplateTypeId generate_new_type_id();

	template<typename T>
	TemplateTypeId get_id()
	{
		static const TemplateTypeId id = generate_new_type_id();
		return id;
	}

	// Manual type id override
	template<typename T>
	concept ManuallyDefinedTemplateTypeId = requires { typename T::ManualId; };

	template<ManuallyDefinedTemplateTypeId T>
	constexpr TemplateTypeId get_id()
	{
		return T::ManualId::value;
	}
}
