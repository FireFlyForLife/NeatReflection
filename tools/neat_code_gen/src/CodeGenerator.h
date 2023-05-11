#pragma once
#include "neat/Reflection.h"

#include <ostream>
#include <string>
#include <string_view>
#include <optional>

#include "reflifc/Module.h"
#include "reflifc/TupleView.h"
#include "reflifc/Type.h"

namespace ifc
{
	class File;
	class Environment;
}


class CodeGenerator
{
public:
	CodeGenerator(const ifc::File& ifc_file, ifc::Environment& environment);

	void write_cpp_file(reflifc::Module module, std::ostream& out);

private:
	void scan(reflifc::Scope scope_desc);
	void scan(reflifc::Declaration decl);
	void scan(reflifc::ScopeDeclaration scope_decl, reflifc::Declaration decl);

	void render(reflifc::ClassOrStruct scope_decl, reflifc::Declaration decl);
	struct TypeMembers { std::string fields, methods; };
	TypeMembers render_members(std::string_view object, std::string_view type_variable, reflifc::ClassOrStruct scope_decl, bool reflect_private_members);
	std::string render_bases(reflifc::ClassOrStruct scope_decl);

private:
	std::string code;
	const ifc::File* ifc_file;
	ifc::Environment* environment;
};
