#pragma once
#include "neat/Reflection.h"

#include "IfcRendering.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>
#include <string>
#include <string_view>
#include <optional>
#include <vector>

#include "reflifc/Chart.h"
#include "reflifc/Expression.h"
#include "reflifc/Module.h"
#include "reflifc/TupleView.h"
#include "reflifc/Type.h"
#include "reflifc/Declaration.h"
#include "reflifc/decl/ClassOrStruct.h"
#include "reflifc/decl/Field.h"
#include "reflifc/decl/Function.h"
#include "reflifc/type/Base.h"
#include "reflifc/type/Function.h"
#include "reflifc/TemplateId.h"
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
		std::string type_name;
		ifc::Access default_access; // public for struct, private for class

		std::vector<reflifc::Field> fields;
		std::vector<reflifc::Method> methods;
		std::vector<reflifc::BaseType> bases;
		std::vector<reflifc::AliasDeclaration> aliases;

		RecursionContext templates_context{};
	};
	struct ReflectableTypes
	{
		std::unordered_map<reflifc::Declaration, ReflectableType> types;
		std::unordered_map<reflifc::TemplateId, ReflectableType> template_types;
		std::unordered_set<reflifc::Type> fundamental_types; // Should be `ifc::FundamentalType*` but reflifc doesn't expose that yet.
	};
	void scan(reflifc::Scope scope_desc, RecursionContext& ctx, ReflectableTypes& out_types);
	void scan(reflifc::Declaration decl, RecursionContext& ctx, ReflectableTypes& out_types);
	void scan(reflifc::ScopeDeclaration scope_decl, reflifc::Declaration decl, RecursionContext& ctx, ReflectableTypes& out_types);
	void scan(reflifc::ClassOrStruct scope_decl, reflifc::Declaration decl, RecursionContext& ctx, ReflectableTypes& out_types);
	void scan(reflifc::Type type, RecursionContext& ctx, ReflectableTypes& out_types);
	void scan(reflifc::Expression expression, RecursionContext& ctx, ReflectableTypes& out_types);
	void scan(reflifc::TemplateId template_id, RecursionContext& ctx, ReflectableTypes& out_types);

	void render(const ifc::FundamentalType& type);
	void render(ReflectableType& type, bool is_templated_type);
	std::string render_field(std::string_view outer_class_type, ifc::Access default_access, const reflifc::Field& field, RecursionContext& ctx) const;
	std::string render_method(std::string_view outer_class_type, ifc::Access default_access, const reflifc::Method& method, RecursionContext& ctx) const;
	std::string render_base_class(std::string_view outer_class_type, ifc::Access default_access, const reflifc::BaseType& base_class, RecursionContext& ctx) const;
	std::string render_member_alias(const reflifc::AliasDeclaration& member_alias, ifc::Access default_access, RecursionContext& ctx) const;
	std::string render_template_argument(const reflifc::Expression& template_arg, RecursionContext& ctx) const;
	
private:
	std::string code;
	const ifc::File* ifc_file;
	ifc::Environment* environment;
};
