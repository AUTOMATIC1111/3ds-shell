#include "properties.h"

void Properties::read(String filename){
	FileStream infostream(cstr(filename),_T("r"));
	if(infostream.file==NULL) return;
	
	std::string line;
	while(! (line=infostream.readline()).empty()){
		char *s=(char *)line.c_str();

		char *p=strchr(s,':');
		if(p==NULL) continue;
		*p=NULL;
		p++; while(isspace(*p)) p++;

		int length;
		while(length=(int)strlen(p), length>0 && (p[length-1]=='\r' || p[length-1]=='\n'))
			p[length-1]='\0';
		
		map[s]=p;
	}
}

void Properties::write(String filename){
	FileStream infostream(cstr(filename),_T("w"));
	if(infostream.file==NULL) return;
	
	for(std::map<std::string,std::string>::iterator iter=map.begin();iter!=map.end();++iter){
		std::string s=iter->first+": "+iter->second+"\n";
		infostream.write((void *) s.c_str(),s.length());
	}
}

void Properties::set(const char *key,std::string value){
	map[key]=value;
}

std::string Properties::get(const char *key,std::string value){
	if(map.count(key)==0)
		return value;

	return map[key];
}
std::string Properties::get(const char *key){
	if(map.count(key)==0)
		return "";

	return map[key];
}
bool Properties::contains(const char *key){
	return map.count(key)!=0;
}


