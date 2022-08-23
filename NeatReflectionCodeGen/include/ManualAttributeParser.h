#pragma once
#include <ifc/SourceLocation.h>

#include <string>
#include <vector>
#include <filesystem>

// EXPERIMENTAL: Purpose of this is to test if parsing C++ attributes is feasable given a small enough domain (between 2 symbols from the .ifc file)
namespace Experimental
{
	struct ParsedAttribute
	{
		std::string contents;
		// Only the internal string contents are provided atm
	};

	// In the future we should run the pre-processor on the file
	std::vector<ParsedAttribute> parse_attributes_from_source(const std::filesystem::path& source, ifc::SourceLocation from, ifc::SourceLocation to);
}