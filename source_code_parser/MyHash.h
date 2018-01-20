#ifndef MY_HASH_H
#define MY_HASH_H
#include <string>
#include <stdint.h>

class MyHash
{
public:
	MyHash();
	~MyHash();
	static void string_hash(std::string str, uint64_t &hash);
	static void generate(uint8_t* block_ptr, const size_t length, uint64_t &hash);
	static std::string hexstring64(const uint64_t &hash);
};
#endif

