#pragma once
#include "neat/Reflection.h"

#include <vector>
#include <unordered_map>
#include <ostream>
#include <string>
#include <string_view>
#include <optional>

#include "reflifc/Module.h"
#include "reflifc/TupleView.h"
#include "reflifc/Type.h"
#include "ifc/Type.h"

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
	struct ReflectableType
	{
		reflifc::Declaration decl;
		reflifc::ScopeDeclaration scope_decl;

		std::vector<reflifc::Field> fields;
		std::vector<reflifc::Method> method;
	};
	struct ReflectableTypes
	{
		// TODO: Support multiple environments
		std::unordered_map<ifc::TypeIndex, ReflectableType> types;
	};
	void scan(reflifc::Scope scope_desc, ReflectableTypes& out_types);
	void scan(reflifc::Declaration decl, ReflectableTypes& out_types);
	void scan(reflifc::ScopeDeclaration scope_decl, reflifc::Declaration decl, ReflectableTypes& out_types);

	void render(reflifc::ClassOrStruct scope_decl, reflifc::Declaration decl);
	struct TypeMembers { std::string fields, methods; };
	TypeMembers render_members(std::string_view object, std::string_view type_variable, reflifc::ClassOrStruct scope_decl, bool reflect_private_members);
	std::string render_bases(reflifc::ClassOrStruct scope_decl);

private:
	std::string code;
	const ifc::File* ifc_file;
	ifc::Environment* environment;
};
