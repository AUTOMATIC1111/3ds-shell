#ifndef __PROPERTIES__
#define __PROPERTIES__

#include <map>
#include <string>
#include "ShellUtils.h"

struct Properties{
	std::map<std::string,std::string> map;

	void read(String filename);
	void write(String filename);

	void set(const char *key,std::string value);
	std::string get(const char *key,std::string value);
	std::string get(const char *key);
	bool contains(const char *key);
};

#endif
