#ifndef SOURCE_CODE_H
#define SOURCE_CODE_H
#include <string>
#include <stdint.h>
#include <fstream>
#include "FileManager.h"
#include <vector>
#include <map>

namespace Code
{
	typedef struct include_info{
		include_info()
		{
			is_library = 0;
			is_external = 0;
		}
		int32_t is_library;
		int32_t is_external;
	} include_info;
	class Source
	{
	public:
		Source();
		~Source();
		bool parse(const std::string &root_dir, std::string name);
		void to_lowercase(std::string &s);
		void get_dependency_graph(std::ofstream &graph);
		uint64_t get_id();
		void build_source_dependency_tree(std::map<uint64_t, std::vector<Code::Source*>> &source_id_map);
		bool find_tree_level();
		uint32_t level();
		bool report();
	private:
		void add_include_me(Code::Source &src);
		void just_filename(std::string &name);
		std::string name_;
		std::string name_full_;
		std::string node_;
		uint64_t id_;
		uint32_t level_;
		IO::string_list include_list_;
		IO::string_list include_list_full_;
		std::vector<uint64_t> include_ids_;
		IO::string_list include_nodes_;
		std::vector<Code::Source*> src_include_me_;
		std::vector<Code::Source*> src_i_include_;
		std::vector<uint32_t> source_to_include_mapping_;
		std::vector<include_info> include_info_;
		Code::Source* p_longest_path_src_;
	};
}
#endif

