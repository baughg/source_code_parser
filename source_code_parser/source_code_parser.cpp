// source_code_parser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SourceTree.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "source_code_parser [source_dir]" << std::endl;
		exit(1);
	}

	std::string source_dir = std::string(argv[1]);
	
	Code::SourceTree tree;
	tree.get_source(source_dir);
	tree.compute_source_distances();
	std::string file_graph;

	if (argc >= 3)
	{
		file_graph = argv[2];
		tree.build_branch_graph(file_graph);
	}
	printf("dot -Grankdir=LR -Tpdf dependency.gv -o dependency.pdf\n");
	return 0;
}

