#include "IfcConversion.h"

#include "ContextualException.h"

#include "ifc/Declaration.h"
#include "ifc/Expression.h"
#include "ifc/Type.h"


std::optional<Neat::Access> convert_access_enum(ifc::Access ifc_access)
{
	switch (ifc_access)
	{
	case ifc::Access::None: return std::nullopt;
	case ifc::Access::Private: return Neat::Access::Private;
	case ifc::Access::Protected: return Neat::Access::Protected;
	case ifc::Access::Public: return Neat::Access::Public;
	default:
		throw ContextualException{ std::format("Invalid ifc::Access enum value: {}", (int)ifc_access) };
	}
}

ifc::Access convert_access_enum(Neat::Access neat_access)
{
	switch (neat_access)
	{
	case Neat::Access::Private: return ifc::Access::Private;
	case Neat::Access::Protected: return ifc::Access::Protected;
	case Neat::Access::Public: return ifc::Access::Public;
	default:
		throw ContextualException{ std::format("Invalid Neat::Access enum value: {}", (int)neat_access) };
	}
}

std::string_view decl_sort_to_string(ifc::DeclSort sort)
{
	switch (sort) 
	{
	case ifc::DeclSort::VendorExtension: return "VendorExtension";
	case ifc::DeclSort::Enumerator: return "Enumerator";
	case ifc::DeclSort::Variable: return "Variable";
	case ifc::DeclSort::Parameter: return "Parameter";
	case ifc::DeclSort::Field: return "Field";
	case ifc::DeclSort::Bitfield: return "Bitfield";
	case ifc::DeclSort::Scope: return "Scope";
	case ifc::DeclSort::Enumeration: return "Enumeration";
	case ifc::DeclSort::Alias: return "Alias";
	case ifc::DeclSort::Temploid: return "Temploid";
	case ifc::DeclSort::Template: return "Template";
	case ifc::DeclSort::PartialSpecialization: return "PartialSpecialization";
	case ifc::DeclSort::Specialization: return "Specialization";
	case ifc::DeclSort::DefaultArgument: return "DefaultArgument";
	case ifc::DeclSort::Concept: return "Concept";
	case ifc::DeclSort::Function: return "Function";
	case ifc::DeclSort::Method: return "Method";
	case ifc::DeclSort::Constructor: return "Constructor";
	case ifc::DeclSort::InheritedConstructor: return "InheritedConstructor";
	case ifc::DeclSort::Destructor: return "Destructor";
	case ifc::DeclSort::Reference: return "Reference";
	case ifc::DeclSort::UsingDeclaration: return "UsingDeclaration";
	case ifc::DeclSort::UsingDirective: return "UsingDirective";
	case ifc::DeclSort::Friend: return "Friend";
	case ifc::DeclSort::Expansion: return "Expansion";
	case ifc::DeclSort::DeductionGuide: return "DeductionGuide";
	case ifc::DeclSort::Barren: return "Barren";
	case ifc::DeclSort::Tuple: return "Tuple";
	case ifc::DeclSort::SyntaxTree: return "SyntaxTree";
	case ifc::DeclSort::Intrinsic: return "Intrinsic";
	case ifc::DeclSort::Property: return "Property";
	case ifc::DeclSort::OutputSegment: return "OutputSegment";
	default:
		throw ContextualException{ std::format("Invalid DeclSort enum value: {}", (int)sort) };
	}
}

std::string_view type_sort_to_string(ifc::TypeSort sort)
{
	switch(sort)
	{
	case ifc::TypeSort::VendorExtension: return "VendorExtension";
	case ifc::TypeSort::Fundamental: return "Fundamental";
	case ifc::TypeSort::Designated: return "Designated";
	case ifc::TypeSort::Tor: return "Tor";
	case ifc::TypeSort::Syntactic: return "Syntactic";
	case ifc::TypeSort::Expansion: return "Expansion";
	case ifc::TypeSort::Pointer: return "Pointer";
	case ifc::TypeSort::PointerToMember: return "PointerToMember";
	case ifc::TypeSort::LvalueReference: return "LvalueReference";
	case ifc::TypeSort::RvalueReference: return "RvalueReference";
	case ifc::TypeSort::Function: return "Function";
	case ifc::TypeSort::Method: return "Method";
	case ifc::TypeSort::Array: return "Array";
	case ifc::TypeSort::Typename: return "Typename";
	case ifc::TypeSort::Qualified: return "Qualified";
	case ifc::TypeSort::Base: return "Base";
	case ifc::TypeSort::Decltype: return "Decltype";
	case ifc::TypeSort::Placeholder: return "Placeholder";
	case ifc::TypeSort::Tuple: return "Tuple";
	case ifc::TypeSort::Forall: return "Forall";
	case ifc::TypeSort::Unaligned: return "Unaligned";
	case ifc::TypeSort::SyntaxTree: return "SyntaxTree";
	default:
		throw ContextualException{ std::format("Invalid TypeSort enum value: {}", (int)sort) };
	}
}

std::string_view expr_sort_to_string(ifc::ExprSort sort)
{
	switch(sort)
	{
	case ifc::ExprSort::VendorExtension: return "VendorExtension";
	case ifc::ExprSort::Empty: return "Empty";
	case ifc::ExprSort::Literal: return "Literal";
	case ifc::ExprSort::Lambda: return "Lambda";
	case ifc::ExprSort::Type: return "Type";
	case ifc::ExprSort::NamedDecl: return "NamedDecl";
	case ifc::ExprSort::UnresolvedId: return "UnresolvedId";
	case ifc::ExprSort::TemplateId: return "TemplateId";
	case ifc::ExprSort::UnqualifiedId: return "UnqualifiedId";
	case ifc::ExprSort::SimpleIdentifier: return "SimpleIdentifier";
	case ifc::ExprSort::Pointer: return "Pointer";
	case ifc::ExprSort::QualifiedName: return "QualifiedName";
	case ifc::ExprSort::Path: return "Path";
	case ifc::ExprSort::Read: return "Read";
	case ifc::ExprSort::Monad: return "Monad";
	case ifc::ExprSort::Dyad: return "Dyad";
	case ifc::ExprSort::Triad: return "Triad";
	case ifc::ExprSort::String: return "String";
	case ifc::ExprSort::Temporary: return "Temporary";
	case ifc::ExprSort::Call: return "Call";
	case ifc::ExprSort::MemberInitializer: return "MemberInitializer";
	case ifc::ExprSort::MemberAccess: return "MemberAccess";
	case ifc::ExprSort::InheritancePath: return "InheritancePath";
	case ifc::ExprSort::InitializerList: return "InitializerList";
	case ifc::ExprSort::Cast: return "Cast";
	case ifc::ExprSort::Condition: return "Condition";
	case ifc::ExprSort::ExpressionList: return "ExpressionList";
	case ifc::ExprSort::SizeofType: return "SizeofType";
	case ifc::ExprSort::Alignof: return "Alignof";
	case ifc::ExprSort::New: return "New";
	case ifc::ExprSort::Delete: return "Delete";
	case ifc::ExprSort::Typeid: return "Typeid";
	case ifc::ExprSort::DestructorCall: return "DestructorCall";
	case ifc::ExprSort::SyntaxTree: return "SyntaxTree";
	case ifc::ExprSort::FunctionString: return "FunctionString";
	case ifc::ExprSort::CompoundString: return "CompoundString";
	case ifc::ExprSort::StringSequence: return "StringSequence";
	case ifc::ExprSort::Initializer: return "Initializer";
	case ifc::ExprSort::Requires: return "Requires";
	case ifc::ExprSort::UnaryFold: return "UnaryFold";
	case ifc::ExprSort::BinaryFold: return "BinaryFold";
	case ifc::ExprSort::HierarchyConversion: return "HierarchyConversion";
	case ifc::ExprSort::ProductTypeValue: return "ProductTypeValue";
	case ifc::ExprSort::SumTypeValue: return "SumTypeValue";
	case ifc::ExprSort::SubobjectValue: return "SubobjectValue";
	case ifc::ExprSort::ArrayValue: return "ArrayValue";
	case ifc::ExprSort::DynamicDispatch: return "DynamicDispatch";
	case ifc::ExprSort::VirtualFunctionConversion: return "VirtualFunctionConversion";
	case ifc::ExprSort::Placeholder: return "Placeholder";
	case ifc::ExprSort::Expansion: return "Expansion";
	case ifc::ExprSort::Generic: return "Generic";
	case ifc::ExprSort::Tuple: return "Tuple";
	case ifc::ExprSort::Nullptr: return "Nullptr";
	case ifc::ExprSort::This: return "This";
	case ifc::ExprSort::TemplateReference: return "TemplateReference";
	case ifc::ExprSort::PushState: return "PushState";
	case ifc::ExprSort::TypeTraitIntrinsic: return "TypeTraitIntrinsic";
	case ifc::ExprSort::DesignatedInitializer: return "DesignatedInitializer";
	case ifc::ExprSort::PackedTemplateArguments: return "PackedTemplateArguments";
	case ifc::ExprSort::Tokens: return "Tokens";
	case ifc::ExprSort::AssignInitializer: return "AssignInitializer";
	default:
		throw ContextualException{ std::format("Invalid ExprSort enum value: {}", (int)sort) };
	}
}

std::string_view type_basis_to_string(ifc::TypeBasis type_basis)
{
	switch(type_basis)
	{
	case ifc::TypeBasis::Void: return "Void";
	case ifc::TypeBasis::Bool: return "Bool";
	case ifc::TypeBasis::Char: return "Char";
	case ifc::TypeBasis::Wchar_t: return "Wchar_t";
	case ifc::TypeBasis::Int: return "Int";
	case ifc::TypeBasis::Float: return "Float";
	case ifc::TypeBasis::Double: return "Double";
	case ifc::TypeBasis::Nullptr: return "Nullptr";
	case ifc::TypeBasis::Ellipsis: return "Ellipsis";
	case ifc::TypeBasis::SegmentType: return "SegmentType";
	case ifc::TypeBasis::Class: return "Class";
	case ifc::TypeBasis::Struct: return "Struct";
	case ifc::TypeBasis::Union: return "Union";
	case ifc::TypeBasis::Enum: return "Enum";
	case ifc::TypeBasis::Typename: return "Typename";
	case ifc::TypeBasis::Namespace: return "Namespace";
	case ifc::TypeBasis::Interface: return "Interface";
	case ifc::TypeBasis::Function: return "Function";
	case ifc::TypeBasis::Empty: return "Empty";
	case ifc::TypeBasis::VariableTemplate: return "VariableTemplate";
	case ifc::TypeBasis::Concept: return "Concept";
	case ifc::TypeBasis::Auto: return "Auto";
	case ifc::TypeBasis::DecltypeAuto: return "DecltypeAuto";
	case ifc::TypeBasis::Overload: return "Overload";
	default:
		throw ContextualException{ std::format("Invalid TypeBasis enum value: {}", (int)type_basis) };
	}
}

std::string_view type_precicion_to_string(ifc::TypePrecision type_precision)
{
	switch(type_precision)
	{
	case ifc::TypePrecision::Default: return "Default";
	case ifc::TypePrecision::Short: return "Short";
	case ifc::TypePrecision::Long: return "Long";
	case ifc::TypePrecision::Bit8: return "Bit8";
	case ifc::TypePrecision::Bit16: return "Bit16";
	case ifc::TypePrecision::Bit32: return "Bit32";
	case ifc::TypePrecision::Bit64: return "Bit64";
	case ifc::TypePrecision::Bit128: return "Bit128";
	default:
		throw ContextualException{ std::format("Invalid TypePrecision enum value: {}", (int)type_precision) };
	}
}

std::string_view type_sign_to_string(ifc::TypeSign type_sign)
{
	switch(type_sign)
	{
	case ifc::TypeSign::Plain: return "Plain";
	case ifc::TypeSign::Signed: return "Signed";
	case ifc::TypeSign::Unsigned: return "Unsigned";
	default:
		throw ContextualException{ std::format("Invalid TypeSign enum value: {}", (int)type_sign) };
	}
}
