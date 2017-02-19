#ifndef VARDECLVISITOR_H
#define VARDECLVISITOR_H

#include "file_edit.h"

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <string>

namespace llvm{
	class formatted_raw_ostream;
}

struct DeclarationPrinter : clang::ast_matchers::MatchFinder::MatchCallback {
	DeclarationPrinter(std::string filename, unsigned int line, unsigned int range);
	private:
	std::string filename{};
	unsigned int line{0};
	unsigned int range{0};
	File_edit file_edit;
	static llvm::formatted_raw_ostream &os;
	void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
};

#endif // VARDECLVISITOR_H