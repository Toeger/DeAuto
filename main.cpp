#include "main.h"

#include <iostream>

// Declares clang::SyntaxOnlyAction.
//#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "clang/ASTMatchers/ASTMatchFinder.h"
//#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

class DeclarationNamePrinter : public MatchFinder::MatchCallback {
	public:
	void run(const MatchFinder::MatchResult &result) override {
		if (const DeclStmt *ds = result.Nodes.getNodeAs<clang::DeclStmt>("declaration")) {
			std::string output;
			llvm::raw_string_ostream out(output);
			for (auto &decl : ds->decls()) {
				{
					std::string filename = result.SourceManager->getFilename(decl->getLocation());
					if (filename.find("main.cpp") == std::string::npos) {
						continue;
					}
				}
				decl->print(out, 0, false);
				out << '\n';
			}
			out.flush();
			std::cout << output;
		}
	}
};

int main(int argc, const char **argv) {
	CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
	ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

	DeclarationNamePrinter declprinter;
	StatementMatcher declmatcher = declStmt().bind("declaration");

	MatchFinder Finder;
	Finder.addMatcher(declmatcher, &declprinter);

	return Tool.run(newFrontendActionFactory(&Finder).get());
}