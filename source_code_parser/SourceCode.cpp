#include "SourceCode.h"
#include "MyHash.h"
#include "FileManager.h"
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>
#include <sstream>

using namespace Code;

Source::Source()
	: id_(0ULL),
	level_(1),
	p_longest_path_src_(nullptr),
	enabled_id_(0)
{
}


Source::~Source()
{
}

// trim from start
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

void Source::to_lowercase(std::string &s) {
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) -> unsigned char { return std::tolower(c); });
}

uint64_t Source::get_id()
{
	return id_;
}

std::vector<uint64_t> &Source::get_id_list()
{
  return id_list_;
}

void Source::string_split(std::string &content, IO::string_list &split)
{
  std::istringstream ss(content);
  std::string token;

  while (std::getline(ss, token, '/')) {
    token = trim(token);

    if (token.length())
      split.push_back(token);
  }
}

void Source::get_dependency_graph(std::ofstream &graph_file)
{
	const size_t include_count = include_list_.size();

	if (!include_count)
		return;

	std::string parent_node = "node";
	std::string child_node = "node";
	parent_node.append(node_);

	graph_file
		<< parent_node
		<< " [shape=box]"
		<< std::endl;

	graph_file
		<< parent_node
		<< " [label=\""
		<< name_
		<< "\"]"
		<< std::endl;

	for (size_t i = 0; i < include_count; ++i)
	{		
		child_node = "node";
		child_node.append(include_nodes_[i]);

		graph_file
			<< child_node
			<< " [label=\""
			<< include_list_[i]
			<< "\"]"
			<< std::endl;

		graph_file << parent_node
			<< " -> " << child_node << std::endl;
	}
}

std::string Source::get_name()
{
	return name_;
}

std::string Source::get_full_name()
{
	return name_full_;
}

void Source::get_dependency_graph_filtered(std::ofstream &graph_file, const uint32_t &enable_id)
{
	const size_t include_count = src_i_include_.size();

	if (!include_count || enabled_id_ != enable_id)
		return;

	std::string parent_node = "node";
	std::string child_node = "node";
	parent_node.append(node_);

	graph_file
		<< parent_node
		<< " [shape=box]"
		<< std::endl;

	graph_file
		<< parent_node
		<< " [label=\""
		<< name_full_
		<< "\"]"
		<< std::endl;

	for (size_t i = 0; i < include_count; ++i)
	{
		if (src_i_include_[i]->enabled_id_ != enable_id)
			continue;

		child_node = "node";
		child_node.append(src_i_include_[i]->node_);

		graph_file
			<< child_node
			<< " [label=\""
			<< src_i_include_[i]->name_full_
			<< "\"]"
			<< std::endl;

		graph_file << parent_node
			<< " -> " << child_node << std::endl;
	}
}

void Source::just_filename(std::string &name)
{
	std::replace(name.begin(), name.end(), '\\', '/');

	size_t dir_loc = name.find_last_of("/\\");

	if (dir_loc != std::string::npos)
		name = name.substr(dir_loc + 1);
}

void Source::generate_id_list(std::string &name)
{
  IO::string_list split;

  string_split(name, split);

  const int parts = (int)split.size() - 1;
  std::string sub_name = split[parts];
  id_list_.resize(split.size());
  
  for (int s = parts; s >= 0; s--)
  {
    MyHash::string_hash(sub_name, id_list_[s]);

    if (s)
    {
      sub_name = split[s-1] + "/" + sub_name;
    }
  }
}

bool Source::parse(const std::string &root_dir, std::string name)
{
	name_ = name.substr(root_dir.length(), name.length() - root_dir.length());

	if (!name_.length())
		return false;
	
	std::replace(name_.begin(), name_.end(), '\\', '/');

	name_full_ = name_;

  generate_id_list(name_full_);

	just_filename(name_);
	//MyHash::string_hash(name_, id_);
  id_ = id_list_[0];
	node_ = MyHash::hexstring64(id_);

	IO::string_list lines;
	IO::FileManager::get_text_from_file(name, lines);

	const size_t line_count = lines.size();

	if (!line_count)
		return false;

	size_t include_loc = 0;
	size_t comment_loc1 = 0;
	size_t comment_loc2 = 0;
	char* p_line = nullptr;
	char* p_line_end = nullptr;
	char include_file[256];
	char* p_include = nullptr;
	uint32_t state = 0;
	include_list_.reserve(16);
	include_ids_.reserve(16);
	include_nodes_.reserve(16);
	include_list_full_.reserve(16);
	uint64_t id = 0ULL;
	std::string inc_filename;
  std::string inc_filename_full;

	for (size_t l = 0; l < line_count; ++l)
	{
		include_loc = lines[l].rfind("#include");

		if (include_loc != std::string::npos)
		{
			comment_loc1 = lines[l].rfind("//");
			comment_loc2 = lines[l].rfind("/*");

			if (include_loc < comment_loc1 && include_loc < comment_loc2)
			{
				p_line = (char*)lines[l].c_str();
				p_line_end = p_line + lines[l].length();
				p_line += (include_loc + 8);
				memset(include_file, 0, sizeof(include_file));
				state = 0;
				p_include = include_file;
				while (p_line <= p_line_end) {
					switch (state)
					{
					case 0:
						if (*p_line == '"' || *p_line == '<')
						{
							state++;
						}
						break;
					case 1:
						if (*p_line == '"' || *p_line == '>')
						{
							state++;
						}
						else
							*p_include++ = *p_line;
						break;
					}

					if (state == 2)
					{
						inc_filename = std::string(include_file);
						trim(inc_filename);
						std::replace(inc_filename.begin(), inc_filename.end(), '\\', '/');
            inc_filename_full = inc_filename;
						include_list_full_.push_back(inc_filename);
						just_filename(inc_filename);						
						include_list_.push_back(inc_filename);
						MyHash::string_hash(inc_filename_full, id);
						include_ids_.push_back(id);
						include_nodes_.push_back(MyHash::hexstring64(id));						
						break;
					}
					p_line++;
				}

			}
		}
	}
	return true;
}


void Source::add_include_me(Code::Source &src)
{
	src_include_me_.push_back(&src);
}

bool Source::find_tree_level()
{
	const size_t include_count = src_i_include_.size();

	if (!include_count) {
		level_ = src_include_me_.size() == 0 ? ~0 : 1;
		return false;
	}

	uint32_t current_level_ = level_;
	uint32_t new_level = 0;

	for (size_t inc = 0; inc < include_count; ++inc)
	{
		if (src_i_include_[inc]->p_longest_path_src_ == this)
			continue;

		new_level = src_i_include_[inc]->level_ + 1;

		if (new_level > level_)
		{
			level_ = new_level;
			p_longest_path_src_ = src_i_include_[inc];
		}
	}

	return level_ != current_level_;
}

void Source::build_source_dependency_tree(std::map<uint64_t, std::vector<Code::Source*>> &source_id_map)
{
	const size_t include_count = include_list_.size();

	if (!include_count)
		return;

	uint64_t id = 0ULL;
	src_i_include_.reserve(include_count<<2);
	source_to_include_mapping_.reserve(include_count << 2);
	std::vector<Code::Source*> source_list;
	include_info_.resize(include_count);
	std::string sub_path;
	std::string include_path;
	size_t sub_str_loc = 0;
	for (size_t inc = 0; inc < include_count; ++inc)
	{
		id = include_ids_[inc];
		include_info_[inc].is_external = 1;

		if (source_id_map.find(id) != source_id_map.end())
		{
			source_list = source_id_map[id];

			for (size_t sl = 0; sl < source_list.size(); ++sl)
			{
				sub_path = source_list[sl]->name_full_;
				include_path = include_list_full_[inc];

				std::replace(sub_path.begin(), sub_path.end(), '\\', '/');
				std::replace(include_path.begin(), include_path.end(), '\\', '/');

				size_t rel_path = include_path.rfind("../");

				while (rel_path != std::string::npos)
				{
					include_path = include_path.substr(rel_path+3);
					rel_path = include_path.rfind("../");
				}

				sub_str_loc = sub_path.rfind(include_path);
				
				if (sub_str_loc != std::string::npos) {
					sub_str_loc = sub_path.length() - sub_str_loc;

					if (sub_str_loc == include_path.length())
					{
						src_i_include_.push_back(source_list[sl]);
						source_to_include_mapping_.push_back((uint32_t)inc);
						include_info_[inc].is_external = 0;
						source_list[sl]->add_include_me(*this);
					}
				}
			}
		}
	}
}

uint32_t Source::level()
{
	return level_;
}

void Source::set_enable_id(uint32_t enable_id)
{
	enabled_id_ = enable_id;
}

bool Source::enable_update(const uint32_t &en_id)
{
	const size_t src_i_include_count = src_i_include_.size();
	uint32_t old_enabled_id = enabled_id_;

	for (size_t src = 0; src < src_i_include_count; ++src)
	{
		if (src_i_include_[src]->enabled_id_ == en_id)
		{
			enabled_id_ = en_id;
			break;
		}
	}

	return old_enabled_id != enabled_id_;
}

bool Source::report()
{
	if (level_ == ~0)
		return false;

	const size_t src_i_include_count = src_i_include_.size();
	const size_t src_include_me_count = src_include_me_.size();
	const size_t external_includes = include_info_.size();

	uint32_t externals = 0;

	for (size_t e = 0; e < external_includes; ++e)
	{
		externals += include_info_[e].is_external;
	}

	printf("%03u. %s\n",level_,name_full_.c_str());

	if (src_include_me_count) {
		printf("\n--------included by------------\n");

		for (size_t src = 0; src < src_include_me_count; ++src)
		{
			printf("\t%03u. %s\n", 
				src_include_me_[src]->level_, 
				src_include_me_[src]->name_full_.c_str());
		}
	}

	if (src_i_include_count) {
		printf("\n--------includes------------\n");

		for (size_t src = 0; src < src_i_include_count; ++src)
		{
			printf("\t%03u. %s [%s]\n",
				src_i_include_[src]->level_,
				src_i_include_[src]->name_full_.c_str(),
				include_list_full_[source_to_include_mapping_[src]].c_str());
		}
	}

	if (externals)
	{
		printf("\n--------external------------\n");

		for (size_t src = 0; src < external_includes; ++src)
		{
			if (include_info_[src].is_external)
				printf("\t%s\n", include_list_full_[src].c_str());
		}
	}

	printf("\n\n");
	return true;
}