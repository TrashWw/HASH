/**
*@Describle:通过mmap映射文件，快速访问
*@Author:wr
*@Time:2019.7.25
*/
#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class FileIO {
	FileIO() {
		fd_map = open("test.txt",O_RDWR|O_CREAT,00777);
		if (fd_map >= 0) {
		}
	}


private:
	int fd_map;

};
