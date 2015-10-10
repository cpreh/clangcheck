#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclCXX.h> // CXXRecordDecl
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <algorithm>
#include <memory>
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

clang::NamespaceDecl const &
get_outermost_namespace(
	clang::NamespaceDecl const *_decl
)
{
	while(
		clang::NamespaceDecl const *next =
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
		const_return_type_(
			_diagnostics.getCustomDiagID(
				clang::DiagnosticsEngine::Warning,
				"Return type is const which prevents moving and RVO"
			)
		),
		namespaces_(
			_namespaces
		)
	{
	}

	bool
	HandleTopLevelDecl(
		clang::DeclGroupRef const _group
	)
	override
	{
		for(
			clang::Decl const *decl
			:
			_group
		)
		{
			if(
				this->filter(
					*decl
				)
			)
				continue;

			clang::FunctionDecl const *const func(
				clang::dyn_cast<
					clang::FunctionDecl
				>(
					decl
				)
			);

			if(
				func
				!=
				nullptr
			)
				this->check_function(
					*func
				);
		}

		return
			true;
	}

	void
	HandleTagDeclDefinition(
		clang::TagDecl *_decl
	)
	override
	{
		if(
			this->filter(
				*_decl
			)
		)
			return;

		clang::CXXRecordDecl const *const record(
			clang::dyn_cast<
				clang::CXXRecordDecl
			>(
				_decl
			)
		);

		if(
			record
			==
			nullptr
		)
			return;

		for(
			clang::CXXMethodDecl const *const method
			:
			record->methods()
		)
			this->check_function(
				*method
			);
	}
private:
	bool
	filter(
		clang::Decl const &_decl
	) const
	{
		clang::NamespaceDecl const *decl_context(
			clang::dyn_cast<
				clang::NamespaceDecl
			>(
				_decl.getDeclContext()
			)
		);

		if(
			decl_context
			==
			nullptr
		)
			return
				true;

		return
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
			namespaces_.end();
	}

	void
	check_function(
		clang::FunctionDecl const &_func
	)
	{
		if(
			_func.getReturnType().isConstQualified()
		)
			diagnostics_.Report(
				_func.getLocation(),
				const_return_type_
			);
	}

	clang::DiagnosticsEngine &diagnostics_;

	unsigned const const_return_type_;

	namespace_vector const namespaces_;
};

class CheckerAction
:
	public clang::PluginASTAction
{
protected:
	std::unique_ptr<
		clang::ASTConsumer
	>
	CreateASTConsumer(
		clang::CompilerInstance &_instance,
		llvm::StringRef
	)
	override
	{
		return
			std::unique_ptr<
				clang::ASTConsumer
			>(
				new Checker(
					_instance.getDiagnostics(),
					namespaces_
				)
			);
	}

	bool
	ParseArgs(
		clang::CompilerInstance const &,
		std::vector<
			std::string
		> const &_args
	)
	override
	{
		namespaces_ =
			_args;

		return
			true;
	}

	namespace_vector namespaces_;
};

}

static clang::FrontendPluginRegistry::Add<CheckerAction>
X("fcppt-check", "Print additional warnings for fcppt");
