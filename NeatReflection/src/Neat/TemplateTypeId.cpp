#include "Neat/TemplateTypeId.h"

namespace Neat
{
	TemplateTypeId generate_new_type_id()
	{
		static TemplateTypeId id_counter = 0;
		return ++id_counter;
	}
}