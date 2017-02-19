#include "file_edit.h"

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

static std::string get_temp_filename(string_view original) {
	//TODO: make this work on all platforms with relative and absolute paths
	original.remove_prefix(original.rfind('/') + 1);
	return "/tmp/" + original.to_string();
}

File_edit::File_edit(std::string filename)
	: input{filename}
	, output(get_temp_filename(filename))
	, filename(std::move(filename)) {}

File_edit::~File_edit() {
	for (std::string text_line; std::getline(input, text_line);) {
		output << text_line << '\n';
	}
	//TODO: move the file get_temp_filename(filename) to filename
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
