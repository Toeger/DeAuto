#include "main.h"

#include <chrono>
#include <iostream>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FormattedStream.h>

struct DeclarationPrinter : clang::ast_matchers::MatchFinder::MatchCallback {
	static llvm::formatted_raw_ostream &os;
	void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
		if (const clang::VarDecl *vd = result.Nodes.getNodeAs<clang::VarDecl>("declaration")) {
			const auto &filename = result.SourceManager->getFilename(vd->getLocation());
			if (filename.find("main.cpp") == std::string::npos) {
				return;
			}
			os.changeColor(os.GREEN, false, false) << vd->getType().getAsString() << ' ' << *vd;
			os.resetColor() << " in ";
			os.changeColor(os.YELLOW, false, false) << filename;
			os.resetColor() << ':';
			os.changeColor(os.YELLOW, false, false) << result.SourceManager->getLineNumber(result.SourceManager->getFileID(vd->getLocation()),
																						   result.SourceManager->getFileOffset(vd->getLocation()));
			os.resetColor() << '\n';
		}
	}
};

llvm::formatted_raw_ostream &DeclarationPrinter::os = llvm::fdbgs();

int main(int argc, const char **argv) {
	auto start = std::chrono::high_resolution_clock::now();
	std::ios::sync_with_stdio(false);
	llvm::cl::OptionCategory myToolCategory("my-tool options");
	clang::tooling::CommonOptionsParser optionsParser(argc, argv, myToolCategory);
	clang::tooling::ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());

	DeclarationPrinter declprinter;
	clang::ast_matchers::DeclarationMatcher declmatcher = clang::ast_matchers::varDecl().bind("declaration");

	clang::ast_matchers::MatchFinder finder;
	finder.addMatcher(declmatcher, &declprinter);

	auto retval = tool.run(clang::tooling::newFrontendActionFactory(&finder).get());
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000. << "s\n";
	return retval;
}