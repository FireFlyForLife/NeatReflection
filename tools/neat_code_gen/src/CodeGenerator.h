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
	struct ReflectableField
	{
		reflifc::Field ifc_field;
		bool is_reflectable;
		std::string failure_reason;
	};

	struct ReflectableMethod
	{
		reflifc::Method ifc_method;
		bool is_reflectable;
		std::string failure_reason;
	};

	struct ReflectableAlias
	{
		reflifc::AliasDeclaration ifc_alias;
		bool is_reflectable;
		std::string failure_reason;
	};

	struct ReflectableBaseClass
	{
		reflifc::BaseType ifc_base;
		bool is_reflectable;
		std::string failure_reason;
	};

	struct ReflectableType
	{
		std::string type_name;
		ifc::Access default_access = ifc::Access::None; // public for struct, private for class

		std::vector<ReflectableBaseClass> bases;

		std::vector<ReflectableField> fields;
		std::vector<ReflectableMethod> methods;
		std::vector<ReflectableAlias> aliases;

		RecursionContext::TemplateArgumentSets templates_context{};
	};

	struct ReflectableTypes
	{
		std::unordered_map<reflifc::Declaration, ReflectableType> types;
		std::unordered_map<reflifc::TemplateId, ReflectableType> template_types;
		std::unordered_set<reflifc::Type> fundamental_types; // Should be `ifc::FundamentalType*` but reflifc doesn't expose that yet.
	};

	void scan(reflifc::Scope scope_desc, RecursionContextArg ctx, ReflectableTypes& out_types);
	void scan(reflifc::Declaration decl, RecursionContextArg ctx, ReflectableTypes& out_types);
	void scan(reflifc::ScopeDeclaration scope_decl, reflifc::Declaration decl, RecursionContextArg ctx, ReflectableTypes& out_types);
	void scan(reflifc::ClassOrStruct scope_decl, reflifc::Declaration decl, RecursionContextArg ctx, ReflectableTypes& out_types);
	void scan(reflifc::Type type, RecursionContextArg ctx, ReflectableTypes& out_types);
	void scan(reflifc::Expression expression, RecursionContextArg ctx, ReflectableTypes& out_types);
	void scan(reflifc::TemplateId template_id, RecursionContextArg ctx, ReflectableTypes& out_types);
	void collect_class_members(reflifc::Declaration decl, reflifc::ClassOrStruct scope_decl, RecursionContextArg ctx, ReflectableType& inout_type, ReflectableTypes& out_other_types);

	void render(const ifc::FundamentalType& type);
	void render(ReflectableType& type, bool is_templated_type);
	std::string render_field(std::string_view outer_class_type, ifc::Access default_access, const reflifc::Field& field, RecursionContextArg ctx) const;
	std::string render_method(std::string_view outer_class_type, ifc::Access default_access, const reflifc::Method& method, RecursionContextArg ctx) const;
	std::string render_base_class(std::string_view outer_class_type, ifc::Access default_access, const reflifc::BaseType& base_class, RecursionContextArg ctx) const;
	std::string render_member_alias(const reflifc::AliasDeclaration& member_alias, ifc::Access default_access, RecursionContextArg ctx) const;
	std::string render_template_argument(const reflifc::Expression& template_arg, RecursionContextArg ctx) const;
	
private:
	std::string code;
	const ifc::File* ifc_file;
	ifc::Environment* environment;
};
