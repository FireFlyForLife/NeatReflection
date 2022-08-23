#include "ManualAttributeParser.h"

#include "CodeGenExceptions.h"

#include <format>
#include <fstream>
#include <cerrno>
#include <cstring>

#include <iostream>

namespace Experimental
{
	std::vector<ParsedAttribute> parse_attributes_from_source(const std::filesystem::path& source, ifc::SourceLocation from, ifc::SourceLocation to)
	{
		// Load file
		if (!std::filesystem::exists(source))
		{
			throw CodeGenException("Failed to parse attributes from source file", 
				std::format("File at path: '{}' does not exist.", source.string()));
		}

		std::ifstream source_file{ source.string() };
		if (!source_file.good())
		{
			throw CodeGenException(std::format("Could not open file for reading. Reason: {}", std::strerror(errno)),
				std::format("Trying to read file: '{}'", source.string()));
		}

		std::vector<std::string> lines; // TODO: Pre-reserve size, only read the data we need

		std::string line;
		while (std::getline(source_file, line))
		{
			lines.push_back(line);
		}

		// Trunctuate to what we are interested in
		std::string to_be_parsed;
		for (auto c = (uint32_t)from.line-1; c <= (uint32_t)to.line-1; ++c)
		{
			uint32_t start = 0;
			uint32_t end = lines[c].size()-1;

			if (c == (uint32_t)from.line-1)
			{
				start = (uint32_t)from.column;
			}
			if (c == (uint32_t)to.line-1)
			{
				end = (uint32_t)to.column;
			}

			to_be_parsed += lines[c].substr(start, end - start);
		}

		std::cout << "To be parsed: " << to_be_parsed;

		// Parse 
		std::vector<ParsedAttribute> attributes;

		

		return attributes;
	}

}