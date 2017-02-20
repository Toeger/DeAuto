#include "vardeclvisitor.h"
#include "assume.h"
#include "file_edit.h"

#include <experimental/string_view>
#include <fstream>
#include <llvm/Support/FormattedStream.h>

llvm::formatted_raw_ostream &DeclarationPrinter::os = llvm::fdbgs();

static llvm::raw_ostream &operator<<(llvm::raw_ostream &os, llvm::formatted_raw_ostream::Colors color) {
	if (color == os.SAVEDCOLOR) {
		return os.resetColor();
	}
	return os.changeColor(color, false, false);
}

static std::string to_string(const clang::QualType &t) {
	std::string name;
	if (t.isConstQualified()) {
		name = "const ";
	}
	if (t.isVolatileQualified()) {
		name += "volatile ";
	}
	name += t.getAsString();
	return name;
}

static bool contains_full_word(string_view text, string_view word, string_view separators) {
	const auto contains = [](string_view text, std::size_t pos, string_view characters) {
		if (pos >= text.size()) {
			return true;
		}
		return characters.find(text[pos]) != string_view::npos;
	};
	for (std::size_t pos = text.find(word, 0); pos != string_view::npos; pos = text.find(word, pos)) {
		if (contains(text, pos - 1, separators) || contains(text, pos + word.size(), separators)) {
			return true;
		}
	}
	return false;
}

DeclarationPrinter::DeclarationPrinter(boost::filesystem::path filename)
	: DeclarationPrinter(std::move(filename), 1, -1) {}

DeclarationPrinter::DeclarationPrinter(boost::filesystem::path filename, unsigned int line)
	: DeclarationPrinter(std::move(filename), line, 1) {}

DeclarationPrinter::DeclarationPrinter(boost::filesystem::path filename, unsigned int line, unsigned int range)
	: filename(std::move(filename))
	, line(line)
	, range(range)
	, file_edit(this->filename) {}

void DeclarationPrinter::run(const clang::ast_matchers::MatchFinder::MatchResult &result) {
	auto get_column = [&result](clang::SourceLocation sl) {
		return result.SourceManager->getColumnNumber(result.SourceManager->getFileID(sl), result.SourceManager->getFileOffset(sl));
	};
	auto get_line = [&result](clang::SourceLocation sl) {
		return result.SourceManager->getLineNumber(result.SourceManager->getFileID(sl), result.SourceManager->getFileOffset(sl));
	};
	auto get_line_column_string = [&result, &get_column, &get_line](clang::SourceLocation sl) {
		return std::to_string(get_line(sl)) + ":" + std::to_string(get_column(sl));
	};
	auto get_raw_text = [&result](clang::SourceLocation sl) { return result.SourceManager->getCharacterData(sl); };
	auto get_string = [&result, &get_raw_text](clang::SourceLocation start, clang::SourceLocation end) {
		return std::string{get_raw_text(start), get_raw_text(end)};
	};
	auto get_statement = [&result, &get_raw_text](clang::SourceLocation location, clang::SourceLocation min) {
		auto min_pos = get_raw_text(min);
		const char *end = strchr(min_pos, ';');
		return std::string{get_raw_text(location), end};
	};
	const auto &ws = " \t\n\r";
	auto trim = [](std::string &&s) {
		auto should_be_replaced = [](char c) { return std::find(std::begin(ws), std::end(ws), c) != std::end(ws); };
		while (s.empty() == false && should_be_replaced(s.back())) {
			s.pop_back();
		}
		return s;
	};

	if (const clang::VarDecl *vd = result.Nodes.getNodeAs<clang::VarDecl>("declaration")) {
		if (result.SourceManager->getFilename(vd->getLocation()) == filename) {
			return;
		}
		auto current_line = get_line(vd->getLocation());
		if (current_line < line || current_line - line >= range) {
			return;
		}
		const auto original_expression = get_statement(vd->getLocStart(), vd->getLocEnd());
		const auto original_type = trim(get_string(vd->getLocStart(), vd->getLocation()));
		const auto real_type = to_string(vd->getType());

		if (!contains_full_word(original_type, "auto", ws)) {
			return;
		}

		os << os.GREEN << vd->getType().getAsString() << ' ' << vd->getName() << os.SAVEDCOLOR << '\n';
		os << "\tOriginal Expression: " << os.GREEN << original_expression << os.SAVEDCOLOR << " : " << os.YELLOW << get_line_column_string(vd->getLocStart())
		   << os.SAVEDCOLOR << '-' << os.YELLOW << get_line_column_string(vd->getLocEnd()) << os.SAVEDCOLOR << '\n';

		os << "\tVariable: " << os.GREEN << vd->getName() << os.SAVEDCOLOR << '\n';
		os << "\tOriginal Type: " << os.GREEN << original_type << os.YELLOW << ' ' << get_line_column_string(vd->getLocStart()) << os.SAVEDCOLOR << '-'
		   << os.YELLOW << get_line_column_string(vd->getLocation()) << os.SAVEDCOLOR << '\n';
		os << "\tReal Type: " << os.GREEN << real_type << os.SAVEDCOLOR << '\n';
		if (auto init = vd->getInit()) {
			if (const clang::ImplicitCastExpr *ice = clang::dyn_cast<const clang::ImplicitCastExpr>(init)) {
				os << "\tType: " << os.GREEN << to_string(ice->getType()) << os.SAVEDCOLOR << '\n';
				if (const auto expr = ice->getSubExpr()) {
					os << "\tInitializer type: " << os.GREEN << expr->getType().getAsString() << os.SAVEDCOLOR << '\n';
				}
			} else {
				file_edit.modify(current_line, original_type, real_type);
			}
		}
	}
}
