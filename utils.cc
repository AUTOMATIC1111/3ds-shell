#include "utils.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

bool exists(const char *filename){
	return GetFileAttributesA(filename)!=0xffffffff;
}

int lprintf(char *s,size_t length,const char *fmt,...){
	va_list args;
	va_start(args,fmt);

	_vsnprintf(s,length,fmt,args);
	s[length-1]='\0';

	va_end(args);

	return 0;
}

static void fwritesx(char *s,FILE *f){
	for(char *c=s;*c!='\0';c++){
		if(*c=='\n') fputc('\\',f),fputc('n',f);
		else if(*c=='\r') fputc('\\',f),fputc('r',f);
		else if(*c=='\\') fputc('\\',f),fputc('\\',f);
		else fputc(*c,f);
	}
}


char *escapeString(char *src,char *dst,size_t dstLength){
	char *c,*dd=dst;

	if(dstLength==0) return dst;
	
	for(c=src;*c!='\0';c++){
		if(*c=='\n' && dstLength>=2) *dst++='\\',*dst++='n',dstLength-=2;
		else if(*c=='\r' && dstLength>=2) *dst++='\\',*dst++='r',dstLength-=2;
		else if(*c=='\\' && dstLength>=2) *dst++='\\',*dst++='\\',dstLength-=2;
		else if(dstLength>=1) *dst++=*c,dstLength--;
		else break;
	}

	if(dstLength>=1) *dst='\0';
	else *(dst-1)='\0';

	return dd;
}

char *unescapeString(char *src,char *dst,size_t dstLength){
	char *c,*dd=dst;
	
	if(dstLength==0) return dst;

	for(c=src;*c!='\0';){
		if(dstLength==0) break;
		dstLength--;

		if(*c!='\\'){
			*dst++=*c++;
			continue;
		}

		c++;

		switch(*c){
		case 'r': *dst++='\r'; break;
		case 'n': *dst++='\n'; break;
		default: *dst++=*c; break;
		}

		c++;
	}

	if(dstLength>=1) *dst='\0';
	else *(dst-1)='\0';

	return dd;
}

char *slurp(const char *filename,char *text,size_t size){
	size_t count;
	FILE *f=fopen(filename,"rb");
	if(f==NULL){
		*text='\0';
		return text;
	}

	fseek(f,0,2);
	count=ftell(f);
	fseek(f,0,0);
	
	if(size<=count) count=size-1;
	fread(text,1,count,f);
	text[count]='\0';

	fclose(f);

	return text;
}

void spit(const char *filename,const char *text){
	FILE *f=fopen(filename,"wb");
	if(f==NULL) return;

	fwrite(text,1,strlen(text),f);

	fclose(f);
}

char *Strings::add(char *str){
	if(count==size) expand();

	size_t len=strlen(str)+1;
	char *s=(char *) malloc(len);
	memcpy(s,str,len);

	data[count++]=s;

	return s;
}
char *Strings::get(int index){
	if(index<0 || index>=count) return NULL;

	return data[index];
}
void Strings::remove(int index){
	if(index<0 || index>=count) return;

	char *res=data[index];

	for(int i=index;i<count-1;i++)
		data[i]=data[i+1];

	count--;

	free(res);
}
void Strings::clear(){
	for(int i=0;i<count;i++)
		free(data[i]);

	count=0;
}
void Strings::spit(char *filename){
	FILE *f=fopen(filename,"w");
	if(f==NULL) return;

	for(int i=0;i<count;i++)
		fprintf(f,"%s\n",data[i]);

	fclose(f);
}
void Strings::slurp(char *filename){
	char s[0x800];
	clear();

	FILE *f=fopen(filename,"r");
	if(f==NULL) return;

	while(fgets(s,sizeof(s),f)!=NULL){
		size_t len=strlen(s);
		while(s[len-1]=='\r' || s[len-1]=='\n')
			s[len-1]='\0',len--;

		if(s[0]!='\0')
			add(s);
	}

	fclose(f);
}

int Strings::index(char *str){
	for(int i=0;i<count;i++)
		if(strcmp(data[i],str)==0)
			return i;

	return -1;
}

void Strings::expand(){
	char **p=data;

	int newSize=size*2;
	data=(char **)malloc(sizeof(char *)*newSize);
	memcpy(data,p,sizeof(char *)*size);

	size=newSize;
	free(p);
}

Strings::Strings(){
	size=8;
	data=(char **)malloc(sizeof(char *)*size);
	count=0;
}
Strings::~Strings(){
	clear();

	free(data);
}

bool isYes(const char *s){
	if(s==NULL) return 0;

	return strcmp(s,"yes")==0;
}


void stolower(char *s){
	while(*s!='\0')
		*s=tolower(*s),s++;
}


// trim from start
std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

void split(std::string line,int separator,std::string & a,std::string & b){		
	char *p=(char *)line.c_str();
	char *c=strchr(p,'=');
	if(c==NULL) return;

	while(c>=p && (*c=='=' || *c==' ')) c--; c++;
	a=trim(std::string(p,c-p));

	while(*c=='=' || *c==' ') c++;
	b=trim(std::string(c));
}


const unsigned char *
boyermoore_horspool_memmem(const unsigned char* haystack, size_t hlen,
                           const unsigned char* needle,   size_t nlen)
{
    size_t scan = 0;
    size_t bad_char_skip[UCHAR_MAX + 1]; /* Officially called:
                                          * bad character shift */
 
    /* Sanity checks on the parameters */
    if (nlen <= 0 || !haystack || !needle)
        return NULL;
 
    /* ---- Preprocess ---- */
    /* Initialize the table to default value */
    /* When a character is encountered that does not occur
     * in the needle, we can safely skip ahead for the whole
     * length of the needle.
     */
    for (scan = 0; scan <= UCHAR_MAX; scan = scan + 1)
        bad_char_skip[scan] = nlen;
 
    /* C arrays have the first byte at [0], therefore:
     * [nlen - 1] is the last byte of the array. */
    size_t last = nlen - 1;
 
    /* Then populate it with the analysis of the needle */
    for (scan = 0; scan < last; scan = scan + 1)
        bad_char_skip[needle[scan]] = last - scan;
 
    /* ---- Do the matching ---- */
 
    /* Search the haystack, while the needle can still be within it. */
    while (hlen >= nlen)
    {
        /* scan from the end of the needle */
        for (scan = last; haystack[scan] == needle[scan]; scan = scan - 1)
            if (scan == 0) /* If the first byte matches, we've found it. */
                return haystack;
 
        /* otherwise, we need to skip some bytes and start again.
           Note that here we are getting the skip value based on the last byte
           of needle, no matter where we didn't match. So if needle is: "abcd"
           then we are skipping based on 'd' and that value will be 4, and
           for "abcdd" we again skip on 'd' but the value will be only 1.
           The alternative of pretending that the mismatched character was
           the last character is slower in the normal case (E.g. finding
           "abcd" in "...azcd..." gives 4 by using 'd' but only
           4-2==2 using 'z'. */
        hlen     -= bad_char_skip[haystack[last]];
        haystack += bad_char_skip[haystack[last]];
    }
 
    return NULL;
}
