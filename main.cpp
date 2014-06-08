#include <clang/AST/ASTConsumer.h>
#include <clang/AST/CXXInheritance.h>
#include <clang/AST/DeclCXX.h> // CXXRecordDecl
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendPluginRegistry.h>

// TODO: Figure out which headers we have to include

namespace
{

class Checker
:
	public clang::ASTConsumer
{
public:
	explicit
	Checker(
		clang::DiagnosticsEngine &_diagnostics
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

		clang::CXXFinalOverriderMap final_overriders;

		record->getFinalOverriders(
			final_overriders
		);

		for(
			auto const &element
			:
			final_overriders
		)
		{
			clang::CXXMethodDecl const *method(
				element.first
			);

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
				_instance.getDiagnostics()
			);
	}

	bool
	ParseArgs(
		clang::CompilerInstance const &,
		std::vector<std::string> const &
	)
	override
	{
		return
			true;
	}
};

}

static clang::FrontendPluginRegistry::Add<CheckerAction>
X("fcppt-check", "Print additional warnings for fcppt");
