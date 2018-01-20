// source_code_parser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileManager.h"
#include "SourceCode.h"
#include <fstream>
#include <map>

int main()
{
	std::string source_dir = "D:\\movidius\\development\\ospray\\ospray\\";
	IO::string_list source_filenames;
	IO::FileManager::subdirectory_content(source_dir, source_filenames, true);
	std::vector<Code::Source> source_code(source_filenames.size());

	const size_t source_count = source_code.size();
	std::string dot_filename = "dependency.gv";

	std::ofstream graph_file(dot_filename.c_str());
	graph_file << "digraph G {\n" << std::endl;
	std::map<uint64_t, std::vector<uint32_t>> source_id_map;
	uint64_t id;

	for (size_t src = 0; src < source_count; ++src)
	{
		if (source_code[src].parse(source_dir, source_filenames[src]))
		{
			id = source_code[src].get_id();

			if (source_id_map.find(id) == source_id_map.end())
			{
				std::vector<uint32_t> src_ind(1);
				src_ind[0] = src;
				source_id_map[id] = src_ind;
			}
			else
			{
				std::vector<uint32_t> src_ind = source_id_map[id];
				src_ind.push_back(src);
				source_id_map[id] = src_ind;
			}
			source_code[src].get_dependency_graph(graph_file);
		}
	}

	graph_file << "}" << std::endl;
	graph_file.close();

	printf("dot -Grankdir=LR -Tpdf dependency.gv -o dependency.pdf\n");
	return 0;
}

