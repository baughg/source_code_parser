#ifndef SOURCE_CODE_H
#define SOURCE_CODE_H
#include <string>
#include <stdint.h>
#include <fstream>
#include "FileManager.h"


namespace Code
{
	class Source
	{
	public:
		Source();
		~Source();
		bool parse(const std::string &root_dir, std::string name);
		void to_lowercase(std::string &s);
		void get_dependency_graph(std::ofstream &graph);
		uint64_t get_id();
	private:
		void just_filename(std::string &name);
		std::string name_;
		std::string name_full_;
		std::string node_;
		uint64_t id_;
		IO::string_list include_list_;
		IO::string_list include_list_full_;
		std::vector<uint64_t> include_ids_;
		IO::string_list include_nodes_;
	};
}
#endif

