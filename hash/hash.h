/*
*@Describle: HASH表的定义
*@Author: wr
*Time: 2019.7.24
*/

#pragma once
#include <string>
#include <hash_fun.h>
#include <malloc.h>
using namespace std;

template<class Key,class Value>
class Hash {
	
	static const int INIT_SIZE = 4;
	static const int LOAD_FACTOR_PERCENT = 90;
	
	struct element
	{	
		Key key;
		Value value;
		element(Key &&key_, Value &&value_):key(move(key_)),value(move(value_)) {
		}
		uint32_t hash;
	};

	static uint32_t hashKey(Key &key) {
		std::hash<Key> hasher;
		auto h = static_cast<uint32_t>(hasher(key));
		h &= 0x7fffffff;
		h |= h == 0;
		return h;
	}

	void malloc_() {
		buffer = reinterpret_cast<element*>(malloc(capacity * sizeof(element)));

		for (int i = 0; i < capacity; ++i) {
			buffer[i].hash = 0;
		}
		resize_old = capacity * LOAD_FACTOR_PERCENT / 100;
		mask = capacity - 1;
	}

	void grow() {
		element*  old_buffer = buffer;
		buffer = nullptr;
		unsigned int old_capacity = capacity;
		
		capacity *= 2;
		malloc_();

		for (unsigned i = 0; i < old_capacity; ++i) {
			auto &ele = old_buffer[i];
			uint32_t hash = ele.hash;
			if (hash != 0) {
				insert_helper(hash, std::move(ele.key), std::move(ele.value));
				ele.~element();
			}
		}
		free(old_buffer);
	}

	unsigned int desired_pos(uint32_t hash) const {
		return hash & mask;
	}

	unsigned int probe_distance(uint32_t hash, uint32_t slot_index) const {
		return (slot_index + capacity - desired_pos(hash))&mask;
	}

	void construct(int ix,uint32_t hash,Key&& key, Value && value) {
		new(&buffer[ix]) element(std::move(key), std::move(value));
		buffer[ix].hash = hash;
	}
	void insert_helper(uint32_t hash, Key&& key, Value &&value) {
		int pos = desired_pos(hash);
		int dist = 0;
		for (;;) {
			if (buffer[pos].hash == 0) {
				construct(pos, hash, std::move(key), std::move(value));
				return;
			}
			
			int existing_ele_dist = probe_distance(buffer[pos].hash,pos);
			if (buffer[pos].key == key) {
				buffer[pos].value = std::move(value);
			}
			if (existing_ele_dist < dist) {
				std::swap(hash, buffer[pos].hash);
				std::swap(key,buffer[pos].key);
				std::swap(value,buffer[pos].value);
				dist = existing_ele_dist;
			}
			pos = (pos + 1)&mask;
			++dist;
		}
	}

public:
	Hash():buffer(nullptr),capacity(INIT_SIZE),entry_num(0) {
		malloc_();
	}

	void insert(Key key, Value value) {
		if (++entry_num > resize_old) {
			grow();
		}
		insert_helper(hashKey(key), std::move(key), std::move(value));
	}

	~Hash() {
		for (int i = 0; i < capacity; ++i) {
			if (buffer[i].hash != 0)
				buffer[i].~element();
		}
		_aligned_free(buffer);
	}
private:
	element* __restrict buffer;
	unsigned int capacity;
	unsigned int resize_old;
	unsigned int entry_num;
	uint32_t mask;
};