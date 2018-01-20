#include "SourceTree.h"

using namespace Code;

SourceTree::SourceTree()
{
}


SourceTree::~SourceTree()
{
}


bool SourceTree::get_source(std::string & source_dir)
{
	IO::FileManager::subdirectory_content(source_dir, source_filenames_, true);
	
	source_code_.resize(source_filenames_.size());
	const size_t source_count = source_code_.size();
	std::string dot_filename = "dependency.gv";

	std::ofstream graph_file(dot_filename.c_str());
	graph_file << "digraph G {\n" << std::endl;
	
	uint64_t id;

	for (size_t src = 0; src < source_count; ++src)
	{
		if (source_code_[src].parse(source_dir, source_filenames_[src]))
		{
			id = source_code_[src].get_id();

			if (source_id_map_.find(id) == source_id_map_.end())
			{
				std::vector<Code::Source*> src_ind(1);
				src_ind[0] = &source_code_[src];
				source_id_map_[id] = src_ind;
			}
			else
			{
				std::vector<Code::Source*> src_ind = source_id_map_[id];
				src_ind.push_back(&source_code_[src]);
				source_id_map_[id] = src_ind;
			}
			source_code_[src].get_dependency_graph(graph_file);
		}
	}

	graph_file << "}" << std::endl;
	graph_file.close();

	for (size_t src = 0; src < source_count; ++src)
	{
		source_code_[src].build_source_dependency_tree(source_id_map_);
	}
	return true;
}