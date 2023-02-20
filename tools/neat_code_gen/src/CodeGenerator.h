#pragma once
#include "Neat/Reflection.h"

#include <ostream>
#include <string>
#include <string_view>
#include <optional>

#include "reflifc/Module.h"
#include "reflifc/TupleView.h"
#include "reflifc/Type.h"

class CodeGenerator
{
public:
	CodeGenerator();

	void write_cpp_file(reflifc::Module module, std::ostream& out);

private:
	void scan(reflifc::Scope scope_desc);
	void scan(reflifc::Declaration decl);
	void scan(reflifc::ScopeDeclaration scope_decl, reflifc::Declaration decl);

	void render(reflifc::ClassOrStruct scope_decl, reflifc::Declaration decl);
	struct TypeMembers { std::string fields, methods; };
	TypeMembers render_members(std::string_view object, std::string_view type_variable, reflifc::ClassOrStruct scope_decl, bool reflect_private_members);
	std::string render_bases(reflifc::ClassOrStruct scope_decl);
	
	std::string render_full_typename(reflifc::Type type);
	std::string render_full_typename(reflifc::FunctionType type);
	std::string render_full_typename(const ifc::FundamentalType& type);
	std::string render_full_typename(reflifc::Expression expr);
	std::string render_full_typename(reflifc::TupleExpressionView tuple);
	std::string render_full_typename(reflifc::TemplateId template_id);
	std::string render_full_typename(reflifc::Declaration decl);

	std::string render_refered_declaration(reflifc::Declaration decl);

	std::string render_namespace(reflifc::Declaration decl);
	std::string render_namespace_of_decl(auto decl);

	std::string render(ifc::Qualifiers qualifiers);
	std::string_view render(Neat::Access access);
	std::string render_as_neat_access_enum(ifc::Access access, std::string_view value_for_none = "");
	std::string render_neat_access_enum(Neat::Access access);

	// Expected to be `ifc::FieldDeclaration` or `ifc::MethodDeclaration`
	template<typename T>
	bool is_member_publicly_accessible(T member_declaration, ifc::TypeBasis type, bool reflects_private_members);
	bool reflects_private_members(reflifc::Declaration type_decl);
	bool is_type_exported(reflifc::Type type);
	bool is_type_exported(reflifc::MethodType method);
	bool is_type_exported(reflifc::Declaration decl);
	bool is_type_exported(reflifc::Expression expr);
	bool is_type_exported(reflifc::TemplateId template_id);

private:
	std::string code;
};

std::optional<Neat::Access> convert(ifc::Access);
std::string replace_all_copy(std::string str, std::string_view target, std::string_view replacement);
std::string to_snake_case(std::string_view type_name);

// Inline Implementation
#include "ContextualException.h"

#include <format>
#include <cstdint>
#include <iostream>

#include "ifc/Type.h"
#include "magic_enum.hpp"

template<typename T>
bool CodeGenerator::is_member_publicly_accessible(T member_declaration, ifc::TypeBasis type, bool reflects_private_members)
{
	if (!is_type_exported(member_declaration.type()))
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

	const Neat::Access member_access = convert(member_declaration.access()).value_or(default_access);

	return (member_access == Neat::Access::Public || reflects_private_members);
}