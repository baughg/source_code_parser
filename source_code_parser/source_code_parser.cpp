// source_code_parser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SourceTree.h"

int main()
{
	std::string source_dir = "D:\\movidius\\development\\ospray\\ospray\\";
	
	Code::SourceTree tree;
	tree.get_source(source_dir);

	printf("dot -Grankdir=LR -Tpdf dependency.gv -o dependency.pdf\n");
	return 0;
}

