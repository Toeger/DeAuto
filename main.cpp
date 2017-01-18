#include "main.h"
#include "vardeclvisitor.h"

#include <chrono>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <iostream>
#include <llvm/Support/CommandLine.h>

int main(int argc, const char **argv) {
	auto start = std::chrono::high_resolution_clock::now();
	std::ios::sync_with_stdio(false);
	llvm::cl::OptionCategory myToolCategory("my-tool options");
	clang::tooling::CommonOptionsParser optionsParser(argc, argv, myToolCategory);
	clang::tooling::ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());

	DeclarationPrinter declprinter;
	clang::ast_matchers::DeclarationMatcher var_decl_matcher = clang::ast_matchers::varDecl().bind("declaration");

	clang::ast_matchers::MatchFinder finder;
	finder.addMatcher(var_decl_matcher, &declprinter);

	const auto &retval = tool.run(clang::tooling::newFrontendActionFactory(&finder).get());
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000. << "s\n";
	return retval;
}