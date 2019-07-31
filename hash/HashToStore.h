#pragma once
#include <string>
#include <hash_fun.h>

using namespace std;

class HashToStore {


private:
	uint32_t curent_pos = 0;
	string filename;
	FILE *file;
};