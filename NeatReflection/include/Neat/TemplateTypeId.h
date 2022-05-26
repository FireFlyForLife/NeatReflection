#pragma once
#include "Neat/DllMacro.h"

#include <cstdint>


namespace Neat
{
	using TemplateTypeId = uint32_t;

	// NOTE: Not thread-safe 
	REFL_API TemplateTypeId generate_new_type_id();

	template<typename T>
	TemplateTypeId get_id()
	{
		static TemplateTypeId id = generate_new_type_id();
		return id;
	}
}