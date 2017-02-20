#include "main.h"
#include "vardeclvisitor.h"

#include <boost/core/demangle.hpp>
#include <chrono>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <experimental/string_view>
#include <iostream>
#include <llvm/Support/CommandLine.h>
#include <sstream>
#include <typeinfo>
#include <utility>
#include <vector>

std::vector<const char *> get_fake_args() {
	//TODO: load this from a config file or something
	auto buffer =
		"--\0"
		"--std=c++1z\0"
		"-D__STDC_CONSTANT_MACROS\0"
		"-D__STDC_LIMIT_MACROS\0"
		"-I.\0"
		"-I/usr/lib/llvm-3.9/include\0"
		"-I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-clang\0"
		"-I/usr/lib/llvm-3.9/lib/clang/3.9.1/include\0";
	std::vector<const char *> argv{buffer};
	for (auto p = buffer;; p++) {
		if (!*p) {
			p++;
			if (!*p) {
				return argv;
			}
			argv.push_back(p);
		}
	}
}

template <class T, class String_t>
T from_string(const String_t &s) {
	T t{};
	std::stringstream ss{s};
	ss >> t;
	if (!ss) {
		throw std::runtime_error("Failed converting string \"" + std::string{s} + "\" to type \"" + boost::core::demangle(typeid(T).name()) + "\"");
	}
	return t;
}

int main(int argc, const char **argv) {
	for (int i = 0; i < argc; i++) {
		std::cout << argv[i] << '\n';
	}
	if (argc < 3) {
		return -1;
	}
	auto filename = argv[1];
	auto line = from_string<unsigned int>(argv[2]);
	auto range = argc >= 3 ? from_string<unsigned int>(argv[3]) : 1u;

	auto fake_args = get_fake_args();
	fake_args.insert(std::begin(fake_args), filename);
	fake_args.insert(std::begin(fake_args), argv[0]);
	int fake_argc = static_cast<int>(fake_args.size());
	const char **fake_argv = fake_args.data();

	auto start = std::chrono::high_resolution_clock::now();
	std::ios::sync_with_stdio(false);
	llvm::cl::OptionCategory myToolCategory("my-tool options");
	clang::tooling::CommonOptionsParser optionsParser(fake_argc, fake_argv, myToolCategory);
	clang::tooling::ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());

	DeclarationPrinter declprinter{filename, line, range};
	clang::ast_matchers::DeclarationMatcher var_decl_matcher = clang::ast_matchers::varDecl().bind("declaration");

	clang::ast_matchers::MatchFinder finder;
	finder.addMatcher(var_decl_matcher, &declprinter);

	const auto &retval = tool.run(clang::tooling::newFrontendActionFactory(&finder).get());
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000. << "s\n";
	return retval;
}
