#ifndef FILE_EDIT_H
#define FILE_EDIT_H

#include <experimental/string_view>
#include <fstream>
#include <boost/filesystem/path.hpp>

using std::experimental::string_view;

struct File_edit {
	File_edit(boost::filesystem::path filename);
	File_edit(File_edit &&) = default;
	File_edit &operator=(File_edit &&) = default;
	~File_edit();
	void modify(int line, string_view old_text, string_view new_text);

	private:
	void read_until(int line);
	std::ifstream input{};
	std::ofstream output{};
	int current_line{1};
	boost::filesystem::path filename;
};

#endif // FILE_EDIT_H