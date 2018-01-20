#ifndef SOURCE_TREE_H
#define SOURCE_TREE_H
#include "FileManager.h"
#include "SourceCode.h"
#include <fstream>
#include <map>

namespace Code
{
	class SourceTree
	{
	public:
		SourceTree();
		~SourceTree();
		bool get_source(std::string & root_dir);
	private:
		IO::string_list source_filenames_;
		std::vector<Code::Source> source_code_;
		std::map<uint64_t, std::vector<Code::Source*>> source_id_map_;
	};
}
#endif

