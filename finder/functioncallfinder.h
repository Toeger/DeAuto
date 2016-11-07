#ifndef FUNCTIONCALL_FINDER_HPP
#define FUNCTIONCALL_FINDER_HPP

#include "finder.h"

#include <string>
#include <vector>

namespace clang {
	class ASTContext;
}

class FunctionCallFinder : public Finder {
	private:
	std::vector<std::string> functions;

	public:
	explicit FunctionCallFinder(clang::ASTContext &context);

	void startSearch() override;
	void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
	void print(clang::raw_ostream &stream) override;
};

#endif
