#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include <string>
#include <vector>

namespace IO {
	typedef std::vector<std::string> string_list;
	class FileManager
	{
	public:
		FileManager();
		~FileManager();
		static void subdirectory_content(std::string _dir, string_list &file_list, bool alloc = true);
		static void directory_content(char* dir, string_list &file_list, string_list &dir_list);
		static bool get_text_from_file(const std::string &text_filename, string_list &lines);
		static bool make_directory(std::string dir_name);
	private:

	};
}
#endif

