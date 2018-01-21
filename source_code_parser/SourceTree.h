#ifndef SOURCE_TREE_H
#define SOURCE_TREE_H
#include "FileManager.h"
#include "SourceCode.h"
#include <fstream>
#include <map>

namespace Code
{
	typedef struct source_depth
	{
		source_depth()
		{
			p_source = nullptr;
			level = 0;
		}

		Code::Source * p_source;
		uint32_t level;
	}source_depth;

	class SourceTree
	{
	public:
		SourceTree();
		~SourceTree();
		bool get_source(std::string & root_dir);
		void compute_source_distances();
		void report_dependency();
		void build_branch_graphs();
	private:
		IO::string_list source_filenames_;
		std::vector<Code::Source> source_code_;
		std::vector<source_depth> source_depth_list_;
		std::map<uint64_t, std::vector<Code::Source*>> source_id_map_;
	};
}
#endif

