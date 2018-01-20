#include "MyHash.h"
#include <string.h>


MyHash::MyHash()
{
}


MyHash::~MyHash()
{
}

void MyHash::string_hash(std::string str, uint64_t &hash)
{
	char length_str[16];
	sprintf(length_str, "%u", (uint32_t)str.length());
	str.append(std::string(length_str));
	
	generate((uint8_t*)str.c_str(), str.length(), hash);
}

void MyHash::generate(uint8_t* block_ptr, const size_t length, uint64_t &hash)
{
	const static size_t big_hash_size = sizeof(uint64_t);

	char _blk_name[big_hash_size + 1];
	memset(_blk_name, 0, sizeof(_blk_name));

	int roll_prev = 0;
	size_t cycles = length;

	if (cycles < big_hash_size)
		cycles = big_hash_size;

	size_t i = 0;
	size_t offset = 0;

	for (size_t _i = 0; _i < cycles; _i++) {
		i = _i % length;
		int roll_current = (int)_blk_name[offset] + (int)block_ptr[i] + roll_prev;
		roll_current %= 256;

		_blk_name[offset] = roll_current;

		roll_prev = (int)_blk_name[offset];
		offset++;
		offset %= big_hash_size;
	}

	memcpy(&hash, _blk_name, sizeof(uint64_t));
}

void hex_nibble(char *c, uint8_t &nibble)
{
	if (nibble >= 0 && nibble <= 9)
	{
		*c = '0' + (char)nibble;
	}
	else if (nibble >= 0xa && nibble <= 0xf)
	{
		*c = 'a' + (char)(nibble - 0xa);
	}
}
std::string MyHash::hexstring64(const uint64_t &hash)
{
	char str[32];
	memset(str, 0, sizeof(str));

	uint8_t* p_hash = (uint8_t*)&hash;
	uint8_t upper_nibble = 0;
	uint8_t lower_nibble = 0;
	int32_t loc = 15;
	for (uint32_t b = 0; b < 8; ++b)
	{
		lower_nibble = *p_hash & 0xf;
		upper_nibble = (*p_hash++ >> 4) & 0xf;

		hex_nibble(&str[loc--], lower_nibble);
		hex_nibble(&str[loc--], upper_nibble);
	}

	return std::string(str);
}