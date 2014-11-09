#ifndef __EX_STREAM__H__
#define __EX_STREAM__H__

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include "ShellUtils.h"


class Stream{
public:
	virtual size_t read(void *p,size_t count)=0;
	virtual size_t write(void *p,size_t count)=0;
	virtual size_t seek(int offset,int origin)=0;
	virtual size_t tell()=0;
	virtual void truncate()=0;
	virtual void close();
	virtual void commit();
	
	size_t writeByte(int c);
	size_t writeStream(Stream *other,size_t count=0xffffffff);
	size_t writeInt(int val);


	std::string readline();
	std::string readUntil(int ch);
	int readInt();
	int readIntBE();

	virtual ~Stream();

	size_t size();

	std::wstring filename;
};

class FileStream :public Stream{
public:
	size_t read(void *p,size_t count);
	size_t write(void *p,size_t count);
	size_t seek(int offset,int origin);
	size_t tell();
	void truncate();
	void close();

	bool owned;

	FileStream(FILE *file);
	FileStream(TCHAR *filename,TCHAR *mode);
	~FileStream();

	FILE *file;
};

class TmpFileStream :public FileStream{
public:
	String filename;
	bool backup;

	void close();

	TmpFileStream(String fn,bool backup=true);
	~TmpFileStream();
};

class IStreamStream :public Stream{
public:
	size_t read(void *p,size_t count);
	size_t write(void *p,size_t count);
	size_t seek(int offset,int origin);
	size_t tell();
	void truncate();
	void commit();

	bool owned;

	IStreamStream(IStream *stream);
	~IStreamStream();
private:
	IStream *stream;
};

class MemoryStream :public Stream{
public:
	size_t read(void *p,size_t count);
	size_t write(void *p,size_t count);
	size_t seek(int offset,int origin);
	size_t tell();
	void truncate();

	MemoryStream();
	~MemoryStream();

	void *detach();

	void *data;
	size_t size;

private:
	int position;
	size_t allocated;

	void expand(int addition);
};

#endif
