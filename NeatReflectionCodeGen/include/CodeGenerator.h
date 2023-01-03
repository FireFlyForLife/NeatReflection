#pragma once
#include "Neat/Reflection.h"

#include <ostream>
#include <string>
#include <string_view>
#include <optional>

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
	void scan(ifc::Sequence scope_desc);
	void scan(ifc::DeclIndex decl);
	void scan(const ifc::ScopeDeclaration& scope_decl, ifc::DeclIndex index);

	void render(const ifc::ScopeDeclaration& scope_decl, ifc::DeclIndex index);
	struct TypeMembers { std::string fields, methods; };
	TypeMembers render_members(std::string_view object, std::string_view type_variable, const ifc::ScopeDeclaration& scope_decl, bool reflect_private_members);
	std::string render_bases(const ifc::ScopeDeclaration& scope_decl);
	
	std::string render_full_typename(ifc::TypeIndex type_index);
	std::string render_full_typename(const ifc::FundamentalType& type);
	std::string render_full_typename(const ifc::TupleType& types);

	std::string render_refered_declaration(const ifc::DeclIndex& decl_index);

	std::string render_namespace(ifc::DeclIndex index);

	std::string render(ifc::Qualifiers qualifiers);
	std::string_view render(Neat::Access access);
	std::string render_as_neat_access_enum(ifc::Access access, std::string_view value_for_none = "");
	std::string render_neat_access_enum(Neat::Access access);

	// Expected to be `ifc::FieldDeclaration` or `ifc::MethodDeclaration`
	template<typename T>
	bool is_member_publicly_accessible(const T& member_declaration, ifc::TypeBasis type, bool reflects_private_members);
	bool reflects_private_members(ifc::DeclIndex type_decl_index);
	bool is_type_exported(ifc::TypeIndex);
	bool is_type_exported(ifc::DeclIndex);

private:
	ifc::File& file;
	std::string code;
};

std::optional<Neat::Access> convert(ifc::Access);
std::string_view get_user_type_name(const ifc::File& file, ifc::NameIndex name);
std::string replace_all_copy(std::string str, std::string_view target, std::string_view replacement);
std::string to_snake_case(std::string_view type_name);

// Inline Implementation
#include "ContextualException.h"

#include <format>
#include <cstdint>
#include <iostream>

#include "ifc/Type.h"
#include "ifc/Declaration.h"
#include "ifc/File.h"
#include "magic_enum.hpp"

template<typename T>
bool CodeGenerator::is_member_publicly_accessible(const T& member_declaration, ifc::TypeBasis type, bool reflects_private_members)
{
	if (!is_type_exported(member_declaration.type))
	{
		return false;
	}

	Neat::Access default_access;
	switch (type)
	{
	case ifc::TypeBasis::Class: 
		default_access = Neat::Access::Private;
		break;
	case ifc::TypeBasis::Struct:
		default_access = Neat::Access::Public;
		break;
	default:
		default_access = Neat::Access::Public;
		//throw ContextualException(std::format("Expected a member to be part of a Class or a Struct, but it's part of a {} instead.",
		//	magic_enum::enum_name(type));
	}

	const Neat::Access member_access = convert(member_declaration.access).value_or(default_access);

	return (member_access == Neat::Access::Public || reflects_private_members);
}