#include "main.h"

#include <chrono>
#include <iostream>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_os_ostream.h>

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static llvm::cl::extrahelp MoreHelp("\nMore help text...");

struct DeclarationNamePrinter : clang::ast_matchers::MatchFinder::MatchCallback {
	void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
		if (const clang::DeclStmt *ds = result.Nodes.getNodeAs<clang::DeclStmt>("declaration")) {
			llvm::raw_os_ostream out{std::cout};
			for (auto &decl : ds->decls()) {
				{
					std::string filename = result.SourceManager->getFilename(decl->getLocation());
					if (filename.find("main.cpp") == std::string::npos) {
						continue;
					}
				}
				out.changeColor(out.BLUE);
				decl->print(out, 0, true);
				out.resetColor() << " in ";
				out.changeColor(out.GREEN) << result.SourceManager->getFilename(decl->getLocation());
				out.resetColor() << ":";
				out.changeColor(out.YELLOW) << result.SourceManager->getLineNumber(result.SourceManager->getFileID(decl->getLocation()),
																				   result.SourceManager->getFileOffset(decl->getLocation()));
				out.resetColor() << '\n';
			}
		}
	}
};

int main(int argc, const char **argv) {
	auto start = std::chrono::high_resolution_clock::now();
	std::ios::sync_with_stdio(false);
	clang::tooling::CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
	clang::tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

	DeclarationNamePrinter declprinter;
	clang::ast_matchers::StatementMatcher declmatcher = clang::ast_matchers::declStmt().bind("declaration");

	clang::ast_matchers::MatchFinder Finder;
	Finder.addMatcher(declmatcher, &declprinter);

	auto retval = Tool.run(clang::tooling::newFrontendActionFactory(&Finder).get());
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000. << "s\n";
	return retval;
}