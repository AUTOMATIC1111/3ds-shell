#ifndef __UTILS__H__
#define __UTILS__H__

#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <string>

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef   signed char	s8;
typedef   signed short	s16;
typedef   signed int	s32;

#define KB *1024
#define MB *1024 KB
#define GB *1024 MB
#define TB *1024 GB

template<class T> struct Local{
	T *object;

	T *attach(T *p){if(object!=NULL) delete object; object=p; return p; };
	T *detach(){T *p=object; object=NULL; return p;};
	operator T*(){ return object; }
	Local(T *o){object=o;}
	Local(){object=NULL;}
	~Local(){ if(object!=NULL) delete object; }
};

template<class T> struct Locala{
	T *object;
	int count;

	operator T*(){ return object; }
	T *operator [](int index){ if(count!=-1 && index>=count) return NULL; return &object[index]; }
	void alloc(int c){if(object!=NULL) delete object;object=new T[c];count=c;}
	Locala(int c){object=new T[c];count=c;}
	Locala(T *o){object=o;count=-1;}
	Locala(){object=NULL;count=-1;}
	~Locala(){ delete [] object; }
};

template<class T> struct Localc{
	T *object;

	T *attach(T *p){if(object!=NULL) free(object); object=p; return p; };
	T *detach(){T *p=object; object=NULL; return p;};
	operator T*(){ return object; }
	T *operator [](int index){ return &object[index]; }
	Localc(T *o){object=o;}
	Localc(){object=NULL;}
	~Localc(){ if(object!=NULL) free(object); }
};

struct Strings{
	char **data;
	int count;

	char *add(char *str);
	char *get(int index);
	void remove(int index);
	void clear();

	int index(char *str);

	Strings();
	~Strings();

	void spit(char *filename);
	void slurp(char *filename);

private:
	int size;

	void expand();
};

/* I can't breathe without this. */
#define elems(v) (sizeof(v)/sizeof((v)[0]))

bool exists(const char *filename);

/* This is a safe version of snprintf that guarantees \0 at
 * the end of string (windows doesn't even have snprintf...) */
int lprintf(char *s,size_t length,const char *fmt,...);

void dumpSjisLines(char *filename,int memStart,int memEnd);
void readSjisLines(char *filename);


char *escapeString(char *src,char *dst,size_t dstLength);
char *unescapeString(char *src,char *dst,size_t dstLength);


char *slurp(const char *filename,char *text,size_t size);
void spit(const char *filename,const char *text);


int alert(const char *s);

int editInplace(const char *filename,char *(*update)(char *line,void *p,int last),void *p);
int updateConfig(const char *key,const char *value);

bool isYes(const char *s);

void stolower(char *s);


std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);
std::string &trim(std::string &s);

void split(std::string input,int separator,std::string & a,std::string & b);

const unsigned char *
boyermoore_horspool_memmem(const unsigned char* haystack, size_t hlen,
                           const unsigned char* needle,   size_t nlen);

#endif
