#include "FileManager.h"
#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <fstream>

using namespace IO;

FileManager::FileManager()
{
}


FileManager::~FileManager()
{
}



void FileManager::directory_content(char* dir, string_list &file_list, string_list &dir_list)
{
	char originalDirectory[_MAX_PATH];

	// Get the current directory so we can return to it
	_getcwd(originalDirectory, _MAX_PATH);

	_chdir(dir);  // Change to the working directory
	_finddata_t fileinfo;

	// This will grab the first file in the directory
	// "*" can be changed if you only want to look for specific files
	intptr_t handle = _findfirst("*", &fileinfo);

	if (handle == -1)  // No files or directories found
	{
		perror("Error searching for file");
		exit(1);
	}

	do
	{
		if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0)
			continue;
		if (fileinfo.attrib & _A_SUBDIR) {

			char sub_dir[5120];

			sprintf(sub_dir, "%s%s\\", dir, fileinfo.name);
			dir_list.push_back(std::string(sub_dir));
			//cout << "This is a directory. " << sub_dir << endl; 
			//list(sub_dir);
		}
		else {
			char sub_dir[5120];

			sprintf(sub_dir, "%s%s", dir, fileinfo.name);
			file_list.push_back(std::string(sub_dir));
			//cout << sub_dir << endl;
		}
	} while (_findnext(handle, &fileinfo) == 0);

	_findclose(handle); // Close the stream

	_chdir(originalDirectory);
}

void FileManager::subdirectory_content(std::string _dir, string_list &file_list, bool alloc)
{
	file_list.clear();
	//std::vector<std::string> file_list;
	string_list dir_list;
	string_list dir_list_sec;
	directory_content((char*)_dir.c_str(), file_list, dir_list);

	if (alloc) {
		file_list.reserve(1000000);
		dir_list_sec.reserve(10000);
	}
	else
		dir_list_sec.reserve(10);


	char dir[1000];

	while (dir_list.size() > 0) {
		for (size_t d = 0; d < dir_list.size(); d++) {
			sprintf(dir, "%s", dir_list[d].c_str());
			directory_content(dir, file_list, dir_list_sec);
		}

		dir_list.clear();
		dir_list = dir_list_sec;
		dir_list_sec.clear();
	}
}

bool FileManager::get_text_from_file(const std::string &text_filename, string_list &lines)
{
	std::ifstream text_file(text_filename.c_str());

	std::string line_in;
	
	

	if (!text_file.good())
		return false;

	while (!text_file.eof()) {
		getline(text_file, line_in);
		lines.push_back(line_in);		
	}

	text_file.close();

	if (!lines.size())
		return false;

	
	return true;
}