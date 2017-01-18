#include "vardeclvisitor.h"

#include <llvm/Support/FormattedStream.h>

llvm::formatted_raw_ostream &DeclarationPrinter::os = llvm::fdbgs();

static llvm::raw_ostream &operator<<(llvm::raw_ostream &os, llvm::formatted_raw_ostream::Colors color) {
	if (color == os.SAVEDCOLOR) {
		return os.resetColor();
	}
	return os.changeColor(color, false, false);
}

void DeclarationPrinter::run(const clang::ast_matchers::MatchFinder::MatchResult &result) {
	auto column = [&result](clang::SourceLocation sl) {
		return result.SourceManager->getColumnNumber(result.SourceManager->getFileID(sl), result.SourceManager->getFileOffset(sl));
	};
	auto line = [&result](clang::SourceLocation sl) {
		return result.SourceManager->getLineNumber(result.SourceManager->getFileID(sl), result.SourceManager->getFileOffset(sl));
	};
	auto line_column = [&result, &column, &line](clang::SourceLocation sl) { return std::to_string(line(sl)) + ":" + std::to_string(column(sl)); };
	auto raw_text = [&result](clang::SourceLocation sl) { return result.SourceManager->getCharacterData(sl); };
	auto string = [&result, &raw_text](clang::SourceLocation start, clang::SourceLocation end) { return std::string{raw_text(start), raw_text(end)}; };
	auto statement = [&result, &raw_text](clang::SourceLocation location, clang::SourceLocation min) {
		auto min_pos = raw_text(min);
		const char *end = strchr(min_pos, ';');
		return std::string{raw_text(location), end};
	};
	auto trim = [](std::string &&s) {
		auto should_be_replaced = [](char c) {
			const auto &ws = " \t\n\r";
			return std::find(std::begin(ws), std::end(ws), c) != std::end(ws);
		};
		while (s.empty() == false && should_be_replaced(s.back())) {
			s.pop_back();
		}
		return s;
	};

	if (const clang::VarDecl *vd = result.Nodes.getNodeAs<clang::VarDecl>("declaration")) {
		const auto &filename = result.SourceManager->getFilename(vd->getLocation());
		if (filename.find("test.cpp") == std::string::npos) {
			return;
		}
		const auto original_expression = statement(vd->getLocStart(), vd->getLocEnd());
		os << os.GREEN << vd->getType().getAsString() << ' ' << vd->getName() << os.SAVEDCOLOR << '\n';
		os << "\tOriginal Expression: " << os.GREEN << original_expression << os.SAVEDCOLOR << " : " << os.YELLOW << line_column(vd->getLocStart())
		   << os.SAVEDCOLOR << '-' << os.YELLOW << line_column(vd->getLocEnd()) << os.SAVEDCOLOR << '\n';

		os << "\tVariable: " << os.GREEN << vd->getName() << os.SAVEDCOLOR << '\n';
		os << "\tOriginal Type: " << os.GREEN << trim(string(vd->getLocStart(), vd->getLocation())) << os.YELLOW << ' ' << line_column(vd->getLocStart())
		   << os.SAVEDCOLOR << '-' << os.YELLOW << line_column(vd->getLocation()) << os.SAVEDCOLOR << '\n';
		os << "\tReal Type: " << os.GREEN << vd->getType().getAsString() << os.SAVEDCOLOR << '\n';
		os << "\tInitializer type: " << os.GREEN << "TODO" << os.SAVEDCOLOR << '\n';
		if (auto init = vd->getInit()) {
			if (const clang::ImplicitCastExpr *ice = clang::dyn_cast<const clang::ImplicitCastExpr>(init)) {
				os << "\tType: " << os.GREEN << ice->getType().getAsString() << os.SAVEDCOLOR << '\n';
				os << "\tCast Type: " << os.GREEN << ice->getCastKindName() << os.SAVEDCOLOR << '\n';
				ice->getSubExpr()->dumpColor();
			}
		}
	}
}
