#pragma once
#include "neat/DllExportMacro.h"

#include <cstdint>


namespace Neat
{
	using TemplateTypeId = uint32_t;

	template<typename T>
	concept ManuallyDefinedTemplateTypeId = requires { typename T::ManualId; };

	REFL_API TemplateTypeId generate_new_type_id();

	template<ManuallyDefinedTemplateTypeId T>
	TemplateTypeId get_id()
	{
		return T::ManualId::value;
	}

	template<typename T>
	TemplateTypeId get_id()
	{
		static TemplateTypeId id = generate_new_type_id();
		return id;
	}
}