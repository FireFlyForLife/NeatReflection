#pragma once
#include <ostream>
#include <string>
#include <string_view>

#include "ifc/FileFwd.h"
#include "ifc/DeclarationFwd.h"
#include "ifc/NameFwd.h"
#include "ifc/Scope.h"
#include "ifc/TypeFwd.h"


class CodeGenerator
{
public:
	CodeGenerator(ifc::File& file);

	void write_cpp_file(std::ostream& out);

private:
	void scan(ifc::ScopeDescriptor scope_desc);
	void scan(ifc::DeclIndex decl);
	void scan(const ifc::ScopeDeclaration& scope_decl);

	void render(std::string_view type_name, const ifc::ScopeDeclaration& scope_decl);
	struct TypeMembers { std::string fields, methods; };
	TypeMembers render_members(std::string_view object, std::string_view type_variable, ifc::ScopeDescriptor scope_desc);
	
	std::string render_full_typename(ifc::TypeIndex type_index);
	std::string render_full_typename(const ifc::FundamentalType& type);
	std::string render_full_typename(const ifc::TupleType& types);

	std::string render(ifc::Qualifiers qualifiers);

	std::string render_attributes(const ifc::ScopeDeclaration& scope_decl);

private:
	ifc::File& file;
	std::string code;
};

std::string_view get_user_type_name(const ifc::File& file, ifc::NameIndex name);
std::string replace_all_copy(std::string str, std::string_view target, std::string_view replacement);
std::string to_snake_case(std::string_view type_name);
