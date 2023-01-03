#pragma once

namespace Neat
{
	void reflect_private_members(); // function stub to indicate we expose full type info

	// I'm not sure if I want to have a macro for this
#define REFLECT_PRIVATE_MEMBERS friend void ::Neat::reflect_private_members()
}