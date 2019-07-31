#include "hashManager.h"
#include <iostream>

int main() {
	try {
		hashManager test;
		uint32_t te = hashManager::hashKey<string>(string("sfe"));
		uint32_t te1 = hashManager::hashKey<string>(string("ssdfe"));
		uint32_t te2 = hashManager::hashKey<string>(string("sfdssde"));
		uint32_t te3 = hashManager::hashKey<string>(string("sfsde"));
		uint32_t te4 = hashManager::hashKey<string>(string("ssdsdfe"));
		uint32_t te5 = hashManager::hashKey<string>(string("sdsdsdfe"));
		test.insert(te, 200, 2);
		test.insert(te1, 200, 2);
		test.insert(te2, 200, 2);
		test.insert(te3, 200, 2);
		test.insert(te4, 200, 2);
		test.insert(te5, 200, 2);
	}
	catch (string s) {
		cout << s;
	}
	
	return 0;

}