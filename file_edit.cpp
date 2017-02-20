#include "file_edit.h"
#include "utility.h"

#include <fstream>
#include <iostream>

static void replace(std::string &original, string_view old_text, string_view new_text) {
	//TODO: avoid using string_view::to_string
	auto pos = original.find(old_text.to_string());
	if (pos == std::string::npos) {
		return;
	}
	original.replace(pos, old_text.size(), new_text.to_string());
}

static std::string get_temp_filename(boost::filesystem::path original) {
	return std::string{} + "/tmp/" + original.filename().c_str();
}

File_edit::File_edit(boost::filesystem::path filename)
	: input{filename.c_str()}
	, output(get_temp_filename(filename.c_str()))
	, filename(std::move(filename)) {}

File_edit::~File_edit() {
	for (std::string text_line; std::getline(input, text_line);) {
		output << text_line << '\n';
	}
	output.close();
	Utility::copy_file(get_temp_filename(filename), filename);
}

void File_edit::modify(int line, string_view old_text, string_view new_text) {
	read_until(line);
	std::string text_line;
	std::getline(input, text_line);
	current_line++;
	replace(text_line, old_text, new_text);
	output << text_line << '\n';
}

void File_edit::read_until(int line) {
	std::string text_line;
	while (current_line < line) {
		if (!std::getline(input, text_line)) {
			return;
		}
		output << text_line << '\n';
		current_line++;
	}
}
