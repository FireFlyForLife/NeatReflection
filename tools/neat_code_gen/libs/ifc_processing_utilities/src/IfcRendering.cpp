#include "IfcRendering.h"

#include "ContextualException.h"
#include "IfcConversion.h"
#include "IfcVisitor.h"

#include "ifc/Type.h"
#include "ifc/Expression.h"
#include "ifc/Declaration.h"
#include "reflifc/Expression.h"
#include "reflifc/expr/Read.h"
#include "reflifc/expr/Path.h"
#include "reflifc/expr/UnqualifiedId.h"
#include "reflifc/expr/QualifiedName.h"
#include "reflifc/TemplateId.h"
#include "reflifc/decl/AliasDeclaration.h"
#include "reflifc/decl/ClassOrStruct.h"
#include "reflifc/decl/Concept.h"
#include "reflifc/decl/DeclarationReference.h"
#include "reflifc/decl/Enumeration.h"
#include "reflifc/decl/Function.h"
#include "reflifc/decl/Intrinsic.h"
#include "reflifc/decl/Namespace.h"
#include "reflifc/decl/Parameter.h"
#include "reflifc/decl/Specialization.h"
#include "reflifc/decl/TemplateDeclaration.h"
#include "reflifc/decl/UsingDeclaration.h"
#include "reflifc/type/Array.h"
#include "reflifc/type/Expansion.h"
#include "reflifc/type/Function.h"
#include "reflifc/type/Pointer.h"
#include "reflifc/type/Placeholder.h"
#include "reflifc/type/Qualified.h"
#include "reflifc/type/Reference.h"
#include "reflifc/type/Forall.h"

#include <algorithm>
#include <format>
#include <concepts>

using namespace std::string_literals;
using namespace std::string_view_literals;


std::pair<reflifc::Expression, RecursionContext> RecursionContext::get_template_parameter(reflifc::Parameter parameter) const
{
	verify(parameter.sort() == ifc::ParameterSort::Type || parameter.sort() == ifc::ParameterSort::NonType);

	auto level_index = template_argument_sets.size() - 1 - ((uint32_t)parameter.level() - 1);
	auto parameter_index = (uint32_t)parameter.position() - 1;
	verify(level_index < template_argument_sets.size());
	verify(parameter_index < template_argument_sets[level_index].size());
	reflifc::Expression argument = template_argument_sets[level_index][parameter_index];
	
	RecursionContext relative_ctx{ *this };
	relative_ctx.template_argument_sets.erase(
		relative_ctx.template_argument_sets.begin() + level_index, 
		relative_ctx.template_argument_sets.end()
	);

	return { argument, relative_ctx };
}

std::string render_full_typename(reflifc::Type type, RecursionContextArg ctx)
{
	switch (type.sort())
	{
	case ifc::TypeSort::Fundamental:
		return render_full_typename(type.as_fundamental());
	case ifc::TypeSort::Designated:
	{
		const auto designated_declaration = type.designation();
		return render_namespace(designated_declaration, ctx) + render_refered_declaration(designated_declaration, ctx);
	}
	case ifc::TypeSort::Pointer:
		return render_full_typename(type.as_pointer().pointee, ctx) + "*";
	case ifc::TypeSort::LvalueReference:
		return render_full_typename(type.as_lvalue_reference().referee, ctx) + "&";
	case ifc::TypeSort::RvalueReference:
		return render_full_typename(type.as_rvalue_reference().referee, ctx) + "&&";
	case ifc::TypeSort::Qualified:
		return render_full_typename(type.as_qualified().unqualified(), ctx)
			+ " "
			+ render_qualifiers(type.as_qualified().qualifiers());
	case ifc::TypeSort::Placeholder:
		if (auto elaborated_type = type.as_placeholder().elaboration())
		{
			return render_full_typename(elaborated_type, ctx);
		}
		throw ContextualException("IFC Doesn't contain deduced type for placeholder to be rendered with render_full_typename()");
	case ifc::TypeSort::Function: // U (*)(Args...);
		return render_full_typename(type.as_function(), ctx);
	case ifc::TypeSort::Syntactic:
		return render_full_typename(type.as_syntactic(), ctx);
	case ifc::TypeSort::Typename:
		if (auto resolved_type = resolve_type(type.typename_path(), ctx)) {
			return render_full_typename(resolved_type, ctx);
		} else {
			// If we could not resolve the type, return the typename path as string.
			return "typename " + render_full_typename(type.typename_path().scope(), ctx) + "::" + render_full_typename(type.typename_path().member(), ctx);
		}

		// Currently unsupported
	case ifc::TypeSort::Expansion: // variadic pack expansion (...)
	case ifc::TypeSort::PointerToMember: // U (T::*);
	case ifc::TypeSort::Method: // U (T::*)(Args...);
		// case ifc::TypeSort::Array: // T t[N]; Not implemented yet in ifc-reader
	case ifc::TypeSort::Decltype: // Seems to complicated to support
	case ifc::TypeSort::Forall: // Template declaration. Not used yet in MSVC
	case ifc::TypeSort::Unaligned: // __unaligned T; Partition not implemented yet in ifc-reader

		// Not planned to be supported
	case ifc::TypeSort::VendorExtension:
	case ifc::TypeSort::Tor: // Compiler generated constructor
	case ifc::TypeSort::SyntaxTree: // General parse tree representation. Seems to complicated to support

	default:
		return std::format("<UNSUPPORTED_TYPE {}>", type_sort_to_string(type.sort()));
	}
}

std::string render_full_typename(reflifc::FunctionType function_type, RecursionContextArg ctx) // U (*)(Args...);
{
	auto return_type = render_full_typename(function_type.return_type(), ctx);
	std::string parameter_types;
	auto params = function_type.parameters();
	parameter_types.reserve(params.size() * 8);
	bool first = true;
	for (auto param : params)
	{
		if (first)
			first = false;
		else
			parameter_types += ", ";
		parameter_types += render_full_typename(param, ctx);
	}
	return return_type + " (" + parameter_types + ")";
}

std::string render_full_typename(const ifc::FundamentalType& type)
{
	std::string rendered;
	rendered.reserve(16);

	if (type.sign == ifc::TypeSign::Unsigned)
	{
		rendered += "unsigned ";
	}

	switch (type.precision)
	{
	case ifc::TypePrecision::Default:
		break;
	case ifc::TypePrecision::Short:
		verify(type.basis == ifc::TypeBasis::Int);
		return "short";
	case ifc::TypePrecision::Long:
		rendered += "long ";
		break;
	case ifc::TypePrecision::Bit64:
		verify(type.basis == ifc::TypeBasis::Int);
		return "long long";
	case ifc::TypePrecision::Bit8:
		if (type.basis == ifc::TypeBasis::Char)
		{
			return "char8_t";
		}
	case ifc::TypePrecision::Bit16:
		if (type.basis == ifc::TypeBasis::Char)
		{
			return "char16_t";
		}
	case ifc::TypePrecision::Bit32:
		if (type.basis == ifc::TypeBasis::Char)
		{
			return "char32_t";
		}
	default:
		throw ContextualException{ std::format("Unexpected bitness '{}'. While rendering fundamental type.",
			type_precicion_to_string(type.precision)) };
	}

	switch (type.basis)
	{
	case ifc::TypeBasis::Void:
		rendered += "void";
		break;
	case ifc::TypeBasis::Bool:
		rendered += "bool";
		break;
	case ifc::TypeBasis::Char:
		rendered += "char";
		break;
	case ifc::TypeBasis::Wchar_t:
		rendered += "wchar_t";
		break;
	case ifc::TypeBasis::Int:
		rendered += "int";
		break;
	case ifc::TypeBasis::Float:
		rendered += "float";
		break;
	case ifc::TypeBasis::Double:
		rendered += "double";
		break;
	default:
		throw ContextualException{ std::format("Unexpected basis '{}'. While rendering fundamental type.",
			type_basis_to_string(type.basis)) };
	}

	return rendered;
}


std::string render_full_typename(reflifc::TemplateId template_id, RecursionContextArg ctx)
{
	auto template_arguments = template_id.arguments();
	RecursionContext new_ctx{ ctx };
	new_ctx.template_argument_sets.emplace_back(template_arguments.begin(), template_arguments.end());

	return std::format("{}<{}>",
		render_full_typename(template_id.primary(), new_ctx),
		render_full_typename(template_id.arguments(), ctx));
}

std::string render_full_typename(reflifc::Expression expr, RecursionContextArg ctx)
{
	switch (expr.sort())
	{
	case ifc::ExprSort::NamedDecl:
		return render_full_typename(expr.referenced_decl(), ctx);
	case ifc::ExprSort::Type:
		return render_full_typename(expr.as_type(), ctx);
	case ifc::ExprSort::TemplateId:
		return render_full_typename(expr.as_template_id(), ctx);
	case ifc::ExprSort::Literal:
		return render_full_typename(expr.as_literal());
	case ifc::ExprSort::Read:
		return render_full_typename(expr.as_read().address(), ctx);
	case ifc::ExprSort::UnqualifiedId:
		return render_name(expr.as_unqualified_id().name(), ctx); // TODO: Should this prepend `::` ?
	case ifc::ExprSort::Empty:
		return "";

		// This function is only for rendering a typename, most expression sorts are not used for that so will be ignored:
	default:
		throw ContextualException{ std::format("Unexpected expression while rendering typename. ExprSort is: {}",
			expr_sort_to_string(expr.sort())) };
	}
}

std::string render_full_typename(reflifc::Literal literal)
{
	if (literal.is_null())
	{
		return "";
	}

	switch (literal.sort())
	{
	case ifc::LiteralSort::Immediate: // uint32
		return std::to_string(literal.int_value());
	case ifc::LiteralSort::Integer: // uint64
		//return std::to_string(literal.int64_value());
	case ifc::LiteralSort::FloatingPoint: // double (8 byte)
		//return std::to_string(literal.float_value());
	default:
		throw ContextualException{ std::format("Unknown literal sort value: {}", (int)literal.sort()) };
	}
}

std::string render_full_typename(reflifc::TupleExpressionView tuple, RecursionContextArg ctx)
{
	std::string rendered;
	rendered.reserve(tuple.size() * 8); // Preallocate a reasonable amount

	bool first = true;
	for (auto expression : tuple)
	{
		if (!first)
		{
			rendered += ", ";
		}

		rendered += render_full_typename(expression, ctx);

		first = false;
	}
	return rendered;
}

std::string render_full_typename(reflifc::Declaration decl, RecursionContextArg ctx)
{
	return render_namespace(decl, ctx) + render_refered_declaration(decl, ctx);
}


std::string render_method_pointer(reflifc::MethodType type, std::string_view outer_class_type, RecursionContextArg ctx)
{
	const auto return_type = render_full_typename(type.return_type(), ctx);
	const auto parameter_types = render_full_typename_list(type.parameters(), ctx);
	const auto method_traits = render_function_type_traits(type.traits());

	return std::format("{0} ({1}::*)({2}) {3}", return_type, outer_class_type, parameter_types, method_traits);
}


std::string render_full_typename_list(reflifc::TupleTypeView types, RecursionContextArg ctx)
{
	auto joined_types = std::string{};
	joined_types.reserve(types.size() * 8);
	for (auto type_it = types.begin(); type_it != types.end(); ++type_it) {
		if (type_it != types.begin()) {
			joined_types += ", ";
		}
		joined_types += render_full_typename(*type_it, ctx);
	}
	return joined_types;
}


std::string render_qualifiers(ifc::Qualifiers qualifiers)
{
	constexpr auto const_rendered = "const "sv;
	constexpr auto volatile_rendered = "volatile "sv;
	constexpr auto longest_size = const_rendered.size() + volatile_rendered.size();

	if (qualifiers == ifc::Qualifiers::None) {
		// Early out to avoid allocation.
		return ""s;
	}

	std::string rendered;
	rendered.reserve(longest_size);

	if (ifc::has_qualifier(qualifiers, ifc::Qualifiers::Const)) {
		rendered += const_rendered;
	}
	if (ifc::has_qualifier(qualifiers, ifc::Qualifiers::Volatile)) {
		rendered += volatile_rendered;
	}
	if (ifc::has_qualifier(qualifiers, ifc::Qualifiers::Restrict)) {
		// Ignored
	}

	return rendered;
}

std::string render_function_type_traits(ifc::FunctionTypeTraits traits)
{
	constexpr auto const_rendered = "const "sv;
	constexpr auto volatile_rendered = "volatile "sv;
	constexpr auto lvalue_rendered = "& "sv;
	constexpr auto rvalue_rendered = "&& "sv;
	constexpr auto longest_size = const_rendered.size() + volatile_rendered.size() + std::max(lvalue_rendered.size(), rvalue_rendered.size());

	if (traits == ifc::FunctionTypeTraits::None) {
		// Early out to avoid allocation.
		return ""s;
	}

	std::string rendered;
	rendered.reserve(longest_size);

	if (ifc::has_trait(traits, ifc::FunctionTypeTraits::Const)) {
		rendered += const_rendered;
	}
	if (ifc::has_trait(traits, ifc::FunctionTypeTraits::Volatile)) {
		rendered += volatile_rendered;
	}

	if (ifc::has_trait(traits, ifc::FunctionTypeTraits::Lvalue)) {
		rendered += lvalue_rendered;
	} else if (ifc::has_trait(traits, ifc::FunctionTypeTraits::Rvalue)) {
		rendered += rvalue_rendered;
	}

	return rendered;
}


std::string render_name(reflifc::Name name, RecursionContextArg ctx)
{
	if (!name) {
		return "";
	}

	switch (name.sort()) {
	case ifc::NameSort::Identifier: return name.as_identifier();
	case ifc::NameSort::Operator: return "operator "s + name.operator_name();
	case ifc::NameSort::Literal: return name.as_literal();
	case ifc::NameSort::Specialization: return render_name(name.as_specialization(), ctx);
		
		// Not implemented in reflifc yet
	case ifc::NameSort::Conversion:
	case ifc::NameSort::Template:
	case ifc::NameSort::SourceFile:
	case ifc::NameSort::Guide:
	default:
		throw ContextualException(std::format("Could not render reflifc::Name, unsupported type: {}", 
			name_sort_to_string(name.sort())));
	}
}

std::string render_name(reflifc::SpecializationName name, RecursionContextArg ctx)
{
	auto primary = render_name(name.primary(), ctx);
	auto args_rendered = render_full_typename(name.template_arguments(), ctx);
	return primary + '<' + args_rendered + '>';
}

std::string render_refered_declaration(reflifc::Declaration decl, RecursionContextArg ctx)
{
	switch (const auto kind = decl.sort())
	{
	case ifc::DeclSort::Parameter:
	{
		auto aaaa = decl.as_parameter().sort();
		auto nameee = decl.as_parameter().name();
		auto[argument, new_ctx] = ctx.get_template_parameter(decl.as_parameter());
		return render_full_typename(argument, new_ctx);
	}
	case ifc::DeclSort::Scope:
	{
		reflifc::ScopeDeclaration scope = decl.as_scope();
		return scope.name().as_identifier();
	}
	case ifc::DeclSort::Template:
	{
		return render_name(decl.as_template().name(), ctx);
	}
	case ifc::DeclSort::Function:
	{
		reflifc::Function function = decl.as_function();
		return function.name().as_identifier();
	}
	case ifc::DeclSort::Reference:
	{
		auto referenced_declaration = decl.as_reference().referenced_declaration(*ctx.environment);
		return render_refered_declaration(referenced_declaration, ctx);
	}
	case ifc::DeclSort::Enumeration:
	{
		auto enumeration = decl.as_enumeration();
		return enumeration.name();
	}
	default:
		throw ContextualException{ std::format("Cannot render a refered declaration with unsupported DeclSort: {}.",
			decl_sort_to_string(kind)) };
	}
}

std::string render_namespace(reflifc::Declaration decl, RecursionContextArg ctx)
{
	reflifc::Declaration home_scope = get_home_scope(decl, ctx);

	if (!home_scope) {
		return "";
	}

	// Recursive call
	auto current_namespace = render_refered_declaration(home_scope, ctx);
	auto full_namespace = render_namespace(home_scope, ctx) + current_namespace;

	if (full_namespace.empty()) {
		return "";
	}

	return full_namespace + "::";
}

std::string_view render_as_neat_access_enum(ifc::Access access, ifc::Access value_for_none)
{
	if (access == ifc::Access::None)
		access = value_for_none;

	return render_as_neat_access_enum(access);
}

std::string_view render_as_neat_access_enum(ifc::Access access, std::string_view value_for_none)
{
	switch (access)
	{
	case ifc::Access::None: return value_for_none;
	case ifc::Access::Private: return "Neat::Access::Private";
	case ifc::Access::Protected: return "Neat::Access::Protected";
	case ifc::Access::Public: return "Neat::Access::Public";
	}

	throw ContextualException("Invalid access value.",
		std::format("Expected 0 to 3 (inclusive). While {0} was given.", static_cast<uint8_t>(access)));
}

bool is_member_publicly_accessible(reflifc::Field field_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, RecursionContextArg ctx)
{
	if (!is_type_visible_from_module(field_declaration.type(), root_module, ctx))
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
		throw ContextualException(std::format("Expected a member variable (Field) to be part of a Class or a Struct, but it's part of a {} instead.",
			type_basis_to_string(type)));
	}

	const Neat::Access member_access = convert_access_enum(field_declaration.access()).value_or(default_access);

	return (member_access == Neat::Access::Public || reflects_private_members);
}

bool is_member_publicly_accessible(reflifc::Method method_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, RecursionContextArg ctx)
{
	if (!is_type_visible_from_module(method_declaration.type(), root_module, ctx))
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
		throw ContextualException(std::format("Expected a member function (Method) to be part of a Class or a Struct, but it's part of a {} instead.",
			type_basis_to_string(type)));
	}

	const Neat::Access member_access = convert_access_enum(method_declaration.access()).value_or(default_access);

	return (member_access == Neat::Access::Public || reflects_private_members);
}

bool is_member_publicly_accessible(reflifc::AliasDeclaration alias_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, RecursionContextArg ctx)
{
	if (!is_type_visible_from_module(alias_declaration.aliasee(), root_module, ctx)) {
		return false;
	}

	Neat::Access default_access;
	switch (type) {
	case ifc::TypeBasis::Class:
		default_access = Neat::Access::Private;
		break;
	case ifc::TypeBasis::Struct:
		default_access = Neat::Access::Public;
		break;
	default:
		throw ContextualException(std::format("Expected a member function (Method) to be part of a Class or a Struct, but it's part of a {} instead.",
			type_basis_to_string(type)));
	}

	const Neat::Access member_access = convert_access_enum(alias_declaration.access()).value_or(default_access);

	return (member_access == Neat::Access::Public || reflects_private_members);
}

bool can_reflect_private_members(reflifc::Declaration type_decl, reflifc::Module root_module, RecursionContextArg ctx)
{
	for (auto expression : type_decl.friends())
	{
		if(expression.sort() == ifc::ExprSort::NamedDecl)
		{
			// If the friend declaration is a named function.
			auto named_decl = expression.referenced_decl();
			if (!named_decl.is_function()) {
				return false;
			}
			auto function_decl = named_decl.as_function();
			
			// Now check if the friend is equal to: `void Neat::reflect_types_and_members()`.

			// Check if the return type is `void`.
			auto return_type = function_decl.type().return_type();
			if (!return_type.is_fundamental()) {
				return false;
			}
			auto return_type_basis = return_type.as_fundamental().basis;

			if (return_type_basis != ifc::TypeBasis::Void) {
				return false;
			}

			// Check the parameters ().
			auto parameter_types = function_decl.type().parameters();
			if (!parameter_types.empty()) {
				return false;
			}

			// Check if the name is "reflect_types_and_members".
			auto name = function_decl.name();
			if (!name.is_identifier()) {
				return false;
			}
			auto name_str = name.as_identifier();
			if (name_str != "reflect_types_and_members"sv) {
				return false;
			}

			// Check if the namespace is "Neat::".
			auto friend_name = render_namespace(named_decl, ctx) + render_refered_declaration(named_decl, ctx);
			auto rendered_type = render_full_typename(named_decl.as_function().type(), ctx);

			auto home_scope = get_home_scope(named_decl, ctx);
			if (!home_scope || !home_scope.is_scope() || !home_scope.as_scope().is_namespace()) {
				return false;
			}
			auto namespace_ = home_scope.as_scope().as_namespace();
			auto namespace_str = namespace_.name().as_identifier();
			
			if (namespace_str != "Neat"sv) {
				return false;
			}

			auto home_scope_of_home_scope = get_home_scope(home_scope, ctx);
			if (home_scope_of_home_scope) { // No more parent namespaces are expected.
				return false;
			}

			// The declaration has a friend expression to the `void Neat::reflect_types_and_members()` functions.
			return true;
		}
	}

	// No friend expression to `Neat::reflect_types_and_members()`.
	return false;
}

bool is_type_visible_from_module(reflifc::Type type, reflifc::Module root_module, RecursionContextArg ctx)
{
	switch (type.sort())
	{
	case ifc::TypeSort::VendorExtension: // Unspecified compiler specific
		return false;
	case ifc::TypeSort::Fundamental:
		return true; // Builtin types are always accesible
	case ifc::TypeSort::Designated:
		return is_type_visible_from_module(type.designation(), root_module, ctx);
	case ifc::TypeSort::Syntactic:
		return is_type_visible_from_module(type.as_syntactic(), root_module, ctx);
	case ifc::TypeSort::Expansion: // Pack expansion
		return is_type_visible_from_module(type.as_expansion().pack(), root_module, ctx);
	case ifc::TypeSort::Pointer:
		return is_type_visible_from_module(type.as_pointer().pointee, root_module, ctx);
	case ifc::TypeSort::Function:
		return is_type_visible_from_module(type.as_function(), root_module, ctx);
	case ifc::TypeSort::Method:
		return is_type_visible_from_module(type.as_method(), root_module, ctx);
	case ifc::TypeSort::Qualified:
		return is_type_visible_from_module(type.as_qualified().unqualified(), root_module, ctx);
	case ifc::TypeSort::LvalueReference:
		return is_type_visible_from_module(type.as_lvalue_reference().referee, root_module, ctx);
	case ifc::TypeSort::RvalueReference:
		return is_type_visible_from_module(type.as_rvalue_reference().referee, root_module, ctx);
	case ifc::TypeSort::Array:
		return is_type_visible_from_module(type.as_array().element(), root_module, ctx);
	case ifc::TypeSort::Typename: 
		return is_type_visible_from_module(type.typename_path(), root_module, ctx);
	case ifc::TypeSort::Base:
		return is_type_visible_from_module(type.as_base().type, root_module, ctx);
	case ifc::TypeSort::Decltype:
		return false; // Too complicated to support
	case ifc::TypeSort::Placeholder: // Elaboration on a `auto` or `decltype(auto)` placeholder type
		return is_type_visible_from_module(type.as_placeholder().elaboration(), root_module, ctx); 
	case ifc::TypeSort::Forall: // A template template type
		return is_type_visible_from_module(type.as_forall().subject(), root_module, ctx);
	case ifc::TypeSort::Tor: // Auto generated constructor type
		return false; // I don't see a situation where this makes sense to reflect

	// Unsupported:
	case ifc::TypeSort::PointerToMember: // Not implemented by reflifc yet
	case ifc::TypeSort::Tuple: // A list of types, not accesible from `Type` in reflifc (yet?)
	case ifc::TypeSort::Unaligned: // __unaligned msvc specifier, not implemented yet by reflifc
	case ifc::TypeSort::SyntaxTree: // An old token style syntax tree type. Not going to be supported and also not supported by reflifc

	default:
		throw ContextualException{ std::format("Unexpected type while checking if the type was exported. type sort: {}",
			type_sort_to_string(type.sort())) };
	}
}

bool is_type_visible_from_module(reflifc::MethodType method, reflifc::Module root_module, RecursionContextArg ctx)
{
	return is_type_visible_from_module(method.return_type(), root_module, ctx) &&
		std::ranges::all_of(method.parameters(), [root_module, &ctx](reflifc::Type type) { return is_type_visible_from_module(type, root_module, ctx); });
}

bool is_type_visible_from_module(reflifc::FunctionType function, reflifc::Module root_module, RecursionContextArg ctx)
{
	return is_type_visible_from_module(function.return_type(), root_module, ctx) &&
		std::ranges::all_of(function.parameters(), [root_module, &ctx](reflifc::Type type) { return is_type_visible_from_module(type, root_module, ctx); });
}

bool is_type_visible_from_module(reflifc::Declaration decl, reflifc::Module root_module, RecursionContextArg ctx)
{
	ContextArea area_{ "While checking if Declaration '{}' is visible from module 'TODO Fill module name in'."sv, 
		decl_sort_to_string(decl.sort()), 
		root_module.unit().name() 
	};

	auto decl_module = reflifc::Module{ decl.containing_file() };

	const bool is_decl_in_module_import_chain = is_module_imported_in_module(decl_module, root_module, *ctx.environment);
	const bool is_decl_in_direct_module = (decl_module == root_module);

	auto specifiers = get_basic_specifiers(decl, ctx);

	using enum ifc::BasicSpecifiers;

	if (is_decl_in_direct_module) {
		return 0 == ((uint8_t)specifiers & (uint8_t)IsMemberOfGlobalModule);
	} else if(is_decl_in_module_import_chain) {
		return 0 == ((uint8_t)specifiers & ((uint8_t)IsMemberOfGlobalModule | (uint8_t)NonExported));
	} else {
		return false;
	}
}

bool is_type_visible_from_module(reflifc::Expression expr, reflifc::Module root_module, RecursionContextArg ctx)
{
	switch (expr.sort())
	{
	case ifc::ExprSort::NamedDecl:
		return is_type_visible_from_module(expr.referenced_decl(), root_module, ctx);
	case ifc::ExprSort::Type:
		return is_type_visible_from_module(expr.as_type(), root_module, ctx);
	case ifc::ExprSort::TemplateId:
		return is_type_visible_from_module(expr.as_template_id(), root_module, ctx);
	case ifc::ExprSort::Literal:
		return true; // ifc::LiteralSort only has the types uint32, uint64 or double. Each of these types are always visible. 
	case ifc::ExprSort::Empty:
		return false;
	case ifc::ExprSort::Read:
		return is_type_visible_from_module(expr.as_read().address(), root_module, ctx);
	case ifc::ExprSort::UnqualifiedId:
		//return is_type_visible_from_module(expr.as_unqualified_id().scope(), root_module, ctx);

		// Not supported currently, will need more investigation.
	case ifc::ExprSort::Lambda:
	case ifc::ExprSort::UnresolvedId:
	case ifc::ExprSort::SimpleIdentifier:

	case ifc::ExprSort::QualifiedName:
	case ifc::ExprSort::Path:

		// The rest is not supported, these expressions are for internals similar to a syntax tree.
	default:
		throw ContextualException(std::format("Unexpected expression while checking if the type can be exported. ExprSort is: {}",
			expr_sort_to_string(expr.sort())));
	}
}

bool is_type_visible_from_module(reflifc::TemplateId template_id, reflifc::Module root_module, RecursionContextArg ctx)
{
	RecursionContext new_ctx{ ctx };
	auto arguments = template_id.arguments();
	new_ctx.template_argument_sets.emplace_back(arguments.begin(), arguments.end());

	return is_type_visible_from_module(template_id.primary(), root_module, new_ctx) &&
		std::ranges::all_of(template_id.arguments(), [root_module , &ctx](reflifc::Expression arg)
		{
			return is_type_visible_from_module(arg, root_module, ctx);
		});
}

bool is_type_visible_from_module(reflifc::PathExpression path, reflifc::Module root_module, RecursionContextArg ctx)
{
	try {
		auto resolved_type = resolve_type(path, ctx);
		return is_type_visible_from_module(resolved_type, root_module, ctx);
	} catch (ContextualException& e) {
		return false;
	}
}

static bool is_module_imported_in_exported_module(reflifc::Module to_check, reflifc::Module root_module, ifc::Environment& environment)
{
	if (to_check == root_module) {
		return true;
	}

	for (auto exported_module : root_module.exported_modules(environment)) {
		if (is_module_imported_in_exported_module(to_check, exported_module, environment)) {
			return true;
		}
	}

	return false;
}

bool is_module_imported_in_module(reflifc::Module to_check, reflifc::Module root_module, ifc::Environment& environment)
{
	if (to_check == root_module) {
		return true;
	}

	// Only check imported modules on the root module.
	for (auto imported_module : root_module.imported_modules(environment)) {
		if (to_check == imported_module) {
			return true;
		}
	}

	// Recursively check 
	if (is_module_imported_in_exported_module(to_check, root_module, environment)) {
		return true;
	}

	return false;
}


bool is_reference_type(reflifc::Declaration decl, RecursionContextArg ctx)
{
	if (decl.is_reference()) {
		return is_reference_type(decl.as_reference().referenced_declaration(*ctx.environment), ctx);
	}
	if (decl.is_parameter()) {
		auto [argument, new_ctx] = ctx.get_template_parameter(decl.as_parameter());
		if (argument.is_type()) {
			return false;
		}
		return is_reference_type(argument.as_type(), new_ctx);
	}

	return false;
}

bool is_reference_type(reflifc::Type type, RecursionContextArg ctx)
{
	ContextArea area_{ "While checking if the type is a reference type."sv };

	switch (type.sort()) {
	case ifc::TypeSort::LvalueReference:
	case ifc::TypeSort::RvalueReference:
		return true;

	case ifc::TypeSort::Designated:
		return is_reference_type(type.designation(), ctx);
	case ifc::TypeSort::Syntactic:
		return is_reference_type(type.as_syntactic(), ctx);
	case ifc::TypeSort::Typename:
		return is_reference_type(resolve_type(type.typename_path(), ctx), ctx);
	case ifc::TypeSort::Qualified:
		return is_reference_type(type.as_qualified().unqualified(), ctx);

	case ifc::TypeSort::Expansion:
	case ifc::TypeSort::Tuple:
		throw ContextualException("Multiple types are not supported yet for checking if it is a reference type.");

	case ifc::TypeSort::VendorExtension:
	case ifc::TypeSort::Fundamental:
	case ifc::TypeSort::Tor:
	case ifc::TypeSort::Pointer:
	case ifc::TypeSort::PointerToMember:
	case ifc::TypeSort::Function:
	case ifc::TypeSort::Method:
	case ifc::TypeSort::Array:
	case ifc::TypeSort::Base:
	case ifc::TypeSort::Decltype:
	case ifc::TypeSort::Placeholder:
	case ifc::TypeSort::Forall:
	case ifc::TypeSort::Unaligned:
	case ifc::TypeSort::SyntaxTree:
		return false;

	default:
		throw ContextualException(std::format("Unexpected type sort while checking reference type: {}.", 
			type_sort_to_string(type.sort())));
	}
}

bool is_reference_type(reflifc::Expression expr, RecursionContextArg ctx)
{
	switch (expr.sort()) {
	case ifc::ExprSort::Type:
		return is_reference_type(expr.as_type(), ctx);
	case ifc::ExprSort::NamedDecl:
		return is_reference_type(expr.referenced_decl(), ctx);

	case ifc::ExprSort::QualifiedName:
	case ifc::ExprSort::UnqualifiedId:
	case ifc::ExprSort::Path:
		throw ContextualException(std::format("Dependant typenames etc are not supported yet for checking reference type. ExprSort: {}",
			expr_sort_to_string(expr.sort())));

	default:
		return false;
	}
}


// Get home scope
template<typename T>
concept HasHomeScope = requires(T t) {
	{ t.home_scope() } -> std::same_as<reflifc::Declaration>;
};

struct GetHomeScope
{
	reflifc::Declaration operator()(HasHomeScope auto const& t) 
	{ 
		return t.home_scope();
	}

	reflifc::Declaration operator()(reflifc::Parameter const& parameter)
	{
		if (parameter.sort() == ifc::ParameterSort::Type) {
			auto [argument, new_ctx] = ctx->get_template_parameter(parameter);
			if (argument.as_type().is_designated()) {
				return get_home_scope(argument.as_type().designation(), new_ctx);
			}
		}

		// TODO: Why is there no default constructor?
		return reflifc::Declaration{ nullptr, {} };
	}

	const RecursionContext* ctx;
};

reflifc::Declaration get_home_scope(const reflifc::Declaration& decl, RecursionContextArg ctx)
{
	ContextArea area_{ "While getting the home scope."sv };

	return visit_declaration<reflifc::Declaration>(decl, *ctx.environment, 
		GetHomeScope{ &ctx }
	);
}


// Get basic specifiers
template<typename T>
concept HasBasicSpecifiers = requires(T t) {
	{ t.specifiers() } -> std::same_as<ifc::BasicSpecifiers>;
};

struct GetBasicSpecifiers
{
	ifc::BasicSpecifiers operator()(HasBasicSpecifiers auto const& t)
	{
		return t.specifiers();
	}

	ifc::BasicSpecifiers operator()(reflifc::Parameter const& parameter)
	{
		if (parameter.sort() == ifc::ParameterSort::Type) {
			auto[argument, new_ctx] = ctx->get_template_parameter(parameter);
			if (argument.as_type().is_designated()) {
				return get_basic_specifiers(argument.as_type().designation(), new_ctx);
			}
		}

		return ifc::BasicSpecifiers{};
	}

	const RecursionContext* ctx;
};

ifc::BasicSpecifiers get_basic_specifiers(reflifc::Declaration decl, RecursionContextArg ctx)
{
	ContextArea area_{ "While getting the basic specifiers."sv };

	return visit_declaration<ifc::BasicSpecifiers>(decl, *ctx.environment,
		GetBasicSpecifiers{ &ctx }
	);
}

// Get declaration name
template<typename T>
concept HasDeclarationName = requires(T t) {
	{ t.name() } -> std::same_as<const char*>;
};

struct GetDeclarationName
{
	std::optional<std::string_view> operator()(HasDeclarationName auto const& t)
	{
		return t.name();
	}

	std::optional<std::string_view> operator()(reflifc::Parameter const& parameter)
	{
		if (parameter.sort() == ifc::ParameterSort::Type) {
			auto [argument, new_ctx] = ctx->get_template_parameter(parameter);
			if (argument.as_type().is_designated()) {
				return get_declaration_name(argument.as_type().designation(), new_ctx);
			}
		}

		return std::nullopt;
	}

	std::optional<std::string_view> operator()(...)
	{
		return std::nullopt;
	}

	const RecursionContext* ctx;
};

std::optional<std::string_view> get_declaration_name(reflifc::Declaration decl, RecursionContextArg ctx)
{
	ContextArea area_{ "While getting the declaration name."sv };

	return visit_declaration<std::optional<std::string_view>>(decl, *ctx.environment,
		GetDeclarationName{ &ctx }
	);
}

reflifc::Type resolve_type(reflifc::PathExpression path, RecursionContextArg ctx)
{
	reflifc::Name dependant_name{ nullptr, {} };
	switch (path.member().sort()) {
	case ifc::ExprSort::UnresolvedId:
		throw ContextualException("UnresolvedId is not implemented yet in reflifc.");
	case ifc::ExprSort::UnqualifiedId:
		dependant_name = path.member().as_unqualified_id().name();
		break;
	default:
		throw ContextualException("Unexpected member sort while checking if the type can be exported.");
	}

	std::string_view dependant_name_str;
	switch (dependant_name.sort()) {
	case ifc::NameSort::Identifier:
		dependant_name_str = dependant_name.as_identifier();
		break;
	case ifc::NameSort::Operator:
		throw ContextualException("Not handled yet by CodeGenerator");
	case ifc::NameSort::Conversion:
		throw ContextualException("Not implemented yet by reflifc");
	case ifc::NameSort::Literal:
		throw ContextualException("A string literal is not expected to be encountered for a dependant typename.");
	case ifc::NameSort::Template:
		throw ContextualException("Not implemented yet by reflifc");
	case ifc::NameSort::Specialization:
		throw ContextualException("Not handled yet by CodeGenerator");
	case ifc::NameSort::SourceFile:
		throw ContextualException("A source file name is not expected to be encountered for a dependant typename.");
	case ifc::NameSort::Guide:
		throw ContextualException("A source file name is not expected to be encountered for a dependant typename.");
	}

	return resolve_type(path.scope(), dependant_name_str, ctx);
}

reflifc::Type resolve_type(reflifc::Expression scope, std::string_view dependant_name, RecursionContextArg ctx)
{
	switch (scope.sort()) {
	case ifc::ExprSort::NamedDecl:
		return resolve_type(scope.referenced_decl(), dependant_name, ctx);
	case ifc::ExprSort::Type:
		throw ContextualException("Type is not implemented yet for dependant typename lookup.");
	case ifc::ExprSort::QualifiedName:
		// TODO: This probably needs to be resolved recursively.
		throw ContextualException("QualifiedName is not implemented yet by CodeGenerator.");
	case ifc::ExprSort::TemplateId:
	{
		RecursionContext new_ctx{ ctx };
		auto arguments = scope.as_template_id().arguments();
		new_ctx.template_argument_sets.emplace_back(arguments.begin(), arguments.end());
		return resolve_type(scope.as_template_id().primary(), dependant_name, new_ctx);
	}
	default:
		throw ContextualException("Unexpected expression sort while resolving a type.");
	}
}

reflifc::Type resolve_type(reflifc::Declaration scope, std::string_view dependant_name, RecursionContextArg ctx)
{
	switch (scope.sort()) {
	case ifc::DeclSort::Scope:
	{
		auto scope_decl = scope.as_scope();
		if (scope_decl.is_class_or_struct()) {
			auto class_or_struct = scope_decl.as_class_or_struct();
			
			for (auto member : class_or_struct.members()) {
				if (get_declaration_name(member, ctx) == dependant_name && member.is_alias()) {
					return member.as_alias().aliasee();
				}
			}
		}
		break;
	}
	case ifc::DeclSort::Template:
	{
		reflifc::Declaration template_entity = resolve_template_entity(scope.as_template(), ctx);
		return resolve_type(template_entity, dependant_name, ctx);
	}
	default:
		break;
	}

	throw ContextualException("Could not resolve type.");
}

// Deal with template specializations
reflifc::Declaration resolve_template_entity(reflifc::TemplateDeclaration template_decl, RecursionContextArg ctx)
{
	reflifc::Declaration template_scope = template_decl.entity();

	for (auto specialization : template_decl.template_specializations()) {
		switch (specialization.sort()) {
		case ifc::DeclSort::Specialization:
			if (does_specialization_fit(specialization.as_specialization(), ctx)) {
				template_scope = specialization.as_specialization().entity();
			}
			break;
		case ifc::DeclSort::PartialSpecialization:
			if (does_specialization_fit(specialization.as_partial_specialization(), ctx)) {
				template_scope = specialization.as_partial_specialization().entity();
			}
			break;
		default:
			throw ContextualException(std::format("Unexpected specialization declaration sort: {}",
				decl_sort_to_string(specialization.sort())));
		}
	}

	return template_scope;
}

bool does_specialization_fit(reflifc::Specialization specialization, RecursionContextArg ctx)
{
	auto specialization_arguments = specialization.form().arguments();
	auto& template_arguments = ctx.template_argument_sets.back();

	if (template_arguments.size() != specialization_arguments.size()) {
		return false;
	}

	for (size_t i = 0; i < template_arguments.size(); ++i) {
		auto template_argument_rendered = render_full_typename(template_arguments[i], ctx);
		auto specialization_argument_rendered = render_full_typename(specialization_arguments[i], ctx);
		if (template_argument_rendered != specialization_argument_rendered) {
			return false;
		}
	}

	return true;
}

bool does_specialization_fit(reflifc::PartialSpecialization specialization, RecursionContextArg ctx)
{
	// TODO: Implement partial specialization
	return false;
}
