#include "SourceTree.h"
#include "FileManager.h"

#include <algorithm>

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

bool sort_by_level(const Code::source_depth &lhs, const Code::source_depth &rhs)
{
	return lhs.level < rhs.level;
}

void SourceTree::compute_source_distances()
{
	const size_t source_count = source_code_.size();
	bool iterate = true;
	source_depth_list_.resize(source_count);
	uint32_t interations = 0;

	while (iterate) {
		iterate = false;

		for (size_t src = 0; src < source_count; ++src)
		{
			iterate |= source_code_[src].find_tree_level();
		}

		interations++;
	}

	for (size_t src = 0; src < source_count; ++src)
	{
		source_depth_list_[src].level = source_code_[src].level();
		source_depth_list_[src].p_source = &source_code_[src];
	}

	std::sort(source_depth_list_.begin(), source_depth_list_.end(), sort_by_level);
	report_dependency();
	build_branch_graphs();
}

void SourceTree::report_dependency()
{
	const size_t sources = source_depth_list_.size();
	uint32_t index = 0;

	for (size_t src = 0; src < sources; ++src)
	{		
		if (source_depth_list_[src].p_source->report())
		{
			index++;
		}
	}

	printf("\n%04u source code files.\n", index);
}

void SourceTree::build_branch_graphs()
{
	const size_t sources = source_depth_list_.size();
	uint32_t enable_id = 1;
	bool enable_update = true;
	char extension[16];
	IO::FileManager::make_directory("graphs");
	IO::FileManager::make_directory("pdf");

	std::ofstream build_pdfs("pdf_build.bat");
	build_pdfs << "del /Y pdf\\*.*" << std::endl;
	

	for (size_t src = 0; src < sources; ++src)
	{
		if (source_depth_list_[src].p_source->level() == 1)
		{
			source_depth_list_[src].p_source->set_enable_id(enable_id);

			enable_update = true;

			while (enable_update)
			{
				enable_update = false;

				for (size_t s = 0; s < sources; ++s)
				{
					enable_update |= source_depth_list_[s].p_source->enable_update(enable_id);
				}
			}

			std::string dot_filename = "graphs\\" + source_depth_list_[src].p_source->get_name();

			sprintf(extension, "_%04u.gv", enable_id);
			dot_filename.append(std::string(extension));

			std::ofstream graph_file(dot_filename.c_str());
			graph_file << "digraph G {\n" << std::endl;

			build_pdfs << "dot -Tpdf " << dot_filename << " -o pdf\\" << source_depth_list_[src].p_source->get_name() << extension << ".pdf" << std::endl;

			uint64_t id;

			for (size_t s = 0; s < sources; ++s)
			{
				source_code_[s].get_dependency_graph_filtered(graph_file, enable_id);
			}

			graph_file << "}" << std::endl;
			graph_file.close();

			enable_id++;
		}
		else
			break;
	}

	build_pdfs.close();
}