// C++ trick, get a unique and consistent number per template type.
// NOTE: doesn't work for dynamic libraries (.dll .so), since each dll has their own id counter.
#pragma once
#include "neat/Defines.h"

#include <cstdint>


namespace Neat
{
	using TemplateTypeId = uint32_t;

	template<typename T>
	concept ManuallyDefinedTemplateTypeId = requires { typename T::ManualId; };

	REFL_API TemplateTypeId generate_new_type_id();

	template<ManuallyDefinedTemplateTypeId T>
	constexpr TemplateTypeId get_id() 
	{	
		return T::ManualId::value; 
	}

	template<typename T>
	TemplateTypeId get_id()
	{
		static const TemplateTypeId id = generate_new_type_id();
		return id;
	}
}