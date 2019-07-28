#pragma once

#include <string>
#include <hash_fun.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <memory.h>
using namespace std;



class hashManager {
private:
	static const int INIT_SIZE = 2;
	static const int LOAD_FACTOR_PERCENT = 90;


	struct index {
		uint32_t hash;
		uint32_t pos;
		uint32_t offset;
		
		index(uint32_t hash_, uint32_t pos_, uint32_t offset_):hash(hash_),pos(pos_), offset(offset_) {
		}
		~index() {}
	};

	template<class Key>
	static uint32_t hashKey(Key &key) {
		std::hash<Key> hasher;
		auto h = static_cast<uint32_t>(hasher(key));
		h &= 0x7fffffff;
		h |= h == 0;
		return h;
	}                                 

	void malloc_() {
		
		int fd = open("/root/wr/test1.txt", O_RDWR | O_CREAT, 00777);
	//	auto p = lseek(fd,0, SEEK_END);
	//	if(p==1)
		lseek(fd, capacity * sizeof(index), SEEK_SET);
			write(fd, "1", 1);
		if (fd >= 0) {
			buffer = reinterpret_cast<index*>(mmap(NULL, capacity * sizeof(index), PROT_READ |PROT_WRITE, MAP_SHARED,fd,0));
		}
		close(fd);
		//buffer = reinterpret_cast<index*>(malloc(capacity * sizeof(index)));
	//	if (p == 1) {
			for (int i = 0; i < capacity; ++i) {
				buffer[i].hash = 0;
			}
	//	}
		
		resize_old = capacity * LOAD_FACTOR_PERCENT / 100;
		mask = capacity - 1;
	}
	
	void grow() {
		index*  old_buffer = reinterpret_cast<index*>(malloc(capacity * sizeof(index)));
		memcpy(old_buffer,buffer, capacity * sizeof(index));
		memset(buffer,0, capacity * sizeof(index));
		munmap(buffer, capacity * sizeof(index));
		unsigned int old_capacity = capacity;

		capacity *= 2;
		malloc_();

		for (unsigned i = 0; i < old_capacity; ++i) {
			auto &ele = old_buffer[i];
			uint32_t hash = ele.hash;
			if (hash != 0) {
				insert_helper(hash, std::move(ele.pos), std::move(ele.offset));
				ele.~index();
			}
		}
		free(old_buffer);
	}

	void insert_helper(uint32_t hash, uint32_t pos_key, uint32_t offset) {
		int pos = desired_pos(hash);
		int dist = 0;
		for (;;) {
			if (buffer[pos].hash == 0) {
				construct(pos, hash, pos_key, offset);
				return;
			}

			int existing_ele_dist = probe_distance(buffer[pos].hash, pos);
			if (existing_ele_dist < dist) {
				std::swap(hash, buffer[pos].hash);
				std::swap(pos_key, buffer[pos].pos);
				std::swap(offset, buffer[pos].offset);
				dist = existing_ele_dist;
			}
			pos = (pos + 1)&mask;
			++dist;
		}
	}

	unsigned int desired_pos(uint32_t hash) const {
		return hash & mask;
	}

	void construct(int ix, uint32_t hash, uint32_t pos, uint32_t offset) {
		new(&buffer[ix]) index(hash,pos,offset);
	}

	unsigned int probe_distance(uint32_t hash, uint32_t slot_index) const {
		return (slot_index + capacity - desired_pos(hash))&mask;
	}

public:
	hashManager() :buffer(nullptr), capacity(INIT_SIZE), entry_num(0) {
		malloc_();
	}
	hashManager(unsigned int size_,unsigned int num) :buffer(nullptr), capacity(size_), entry_num(num) {
		int fd = open("/root/wr/test1.txt", O_RDWR | O_CREAT, 00777);
		//int filelen = lseek(fd, capacity * sizeof(index), SEEK_END);
		//if (filelen == 0) {
		//	write(fd,"0",0);
		//}
		if (fd >= 0) {
			buffer = (index*)mmap(NULL, capacity * sizeof(index), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		}
		close(fd);
		//if (filelen==0) {
		//	for (int i = 0; i < capacity; ++i) {
		//		buffer[i].hash = 0;
		//	}
		//}
		resize_old = capacity * LOAD_FACTOR_PERCENT / 100;
		mask = capacity - 1;
	}
	void insert(uint32_t hash, uint32_t pos,uint32_t offset) {
		if (++entry_num > resize_old) {
			grow();
		}
		insert_helper(hash, pos, offset);
	}

	~hashManager() {
		int k = msync((void*)buffer, capacity * sizeof(index), MS_SYNC);
		munmap(buffer, capacity * sizeof(index));
	}

	template<class Key>
	static uint32_t hashKey(Key key) {
		std::hash<Key> hasher;
		auto h = static_cast<uint32_t>(hasher(key));
		h &= 0x7fffffff;
		h |= h == 0;
		return h;
	}
private:
	index* __restrict buffer;
	unsigned int capacity;
	unsigned int resize_old;
	unsigned int entry_num;
	uint32_t mask;

};