#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclCXX.h> // CXXRecordDecl
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <algorithm>
#include <string>
#include <vector>

// TODO: Figure out which headers we have to include

namespace
{

typedef
std::vector<
	std::string
>
namespace_vector;

clang::NamespaceDecl &
get_outermost_namespace(
	clang::NamespaceDecl *_decl
)
{
	while(
		clang::NamespaceDecl *next =
			clang::dyn_cast<
				clang::NamespaceDecl
			>(
				_decl->getDeclContext()
			)
	)
		_decl = next;

	return
		*_decl;
}

class Checker
:
	public clang::ASTConsumer
{
public:
	Checker(
		clang::DiagnosticsEngine &_diagnostics,
		namespace_vector const &_namespaces
	)
	:
		diagnostics_(
			_diagnostics
		),
		no_override_(
			_diagnostics.getCustomDiagID(
				clang::DiagnosticsEngine::Warning,
				"No override"
			)
		),
		namespaces_(
			_namespaces
		)
	{
	}

	void
	HandleTagDeclDefinition(
		clang::TagDecl *_decl
	)
	override
	{
		// TODO: Do we need this?
		clang::TagDecl *definition(
			_decl->getDefinition()
		);

		if(
			definition
			==
			nullptr
		)
			return;

		clang::CXXRecordDecl *const record(
			clang::dyn_cast<
				clang::CXXRecordDecl
			>(
				definition
			)
		);

		if(
			record
			==
			nullptr
		)
			return;

		clang::NamespaceDecl *decl_context(
			clang::dyn_cast<
				clang::NamespaceDecl
			>(
				record->getDeclContext()
			)
		);

		if(
			decl_context
			==
			nullptr
		)
			return;

		if(
			!decl_context->isAnonymousNamespace()
			&&
			std::find(
				namespaces_.begin(),
				namespaces_.end(),
				get_outermost_namespace(
					decl_context
				).getNameAsString()
			)
			==
			namespaces_.end()
		)
			return;

		for(
			clang::CXXMethodDecl const *method
			:
			record->methods()
		)
		{
			if(
				method->size_overridden_methods()
				==
				0u
				||
				!method->isUserProvided()
			)
				continue;

			if(
				!method->hasAttr<
					clang::OverrideAttr
				>()
			)
				diagnostics_.Report(
					method->getLocation(),
					no_override_
				);
		}
	}
private:
	clang::DiagnosticsEngine &diagnostics_;

	unsigned const no_override_;

	namespace_vector const namespaces_;
};

class CheckerAction
:
	public clang::PluginASTAction
{
protected:
	clang::ASTConsumer *
	CreateASTConsumer(
		clang::CompilerInstance &_instance,
		llvm::StringRef
	)
	override
	{
		return
			new Checker(
				_instance.getDiagnostics(),
				namespaces_
			);
	}

	bool
	ParseArgs(
		clang::CompilerInstance const &,
		std::vector<std::string> const &_args
	)
	override
	{
		namespaces_ = _args;

		return
			true;
	}

	namespace_vector namespaces_;
};

}

static clang::FrontendPluginRegistry::Add<CheckerAction>
X("fcppt-check", "Print additional warnings for fcppt");
