#ifndef VARDECLVISITOR_H
#define VARDECLVISITOR_H

#include <clang/ASTMatchers/ASTMatchFinder.h>

namespace llvm{
	class formatted_raw_ostream;
}

struct DeclarationPrinter : clang::ast_matchers::MatchFinder::MatchCallback {
	static llvm::formatted_raw_ostream &os;
	void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
};

#endif // VARDECLVISITOR_H