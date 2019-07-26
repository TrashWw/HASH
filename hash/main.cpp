#include"hash.h"


int main() {
	Hash<string,string>* t  = new Hash<string,string>();
	string s("sdf");
	t->insert("sf","d");
	t->insert("sf", "d");
	t->insert("sf1", "d2");
	t->insert("s1f", "d21");
	t->insert("2sf", "12d");
	return 0;

}