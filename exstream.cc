#include "ShellUtils.h"
#include "exstream.h"
#include <io.h>

void Stream::close(){

}
void Stream::commit(){

}
	
size_t Stream::size(){
	size_t pos=tell();

	size_t res=seek(0,2);
	
	seek((int)pos,0);

	return res;
}

size_t Stream::writeByte(int c){
	char ch=c;
	
	return write(&c,1);
}
size_t Stream::writeInt(int val){
	return write(&val,4);
}

size_t Stream::writeStream(Stream *other,size_t count){
	char buffer[0x400];
	size_t totalWritten=0;

	while(count!=0){
		size_t c=count>sizeof(buffer)?sizeof(buffer):count;
		size_t cc=other->read(buffer,c);
		size_t written=write(buffer,cc);
		if(written==0) break;

		totalWritten+=written;
		count-=written;
	}

	return totalWritten;
}

int Stream::readInt(){
	int res;

	read(&res,4);

	return res;
}
int Stream::readIntBE(){
	unsigned int res=(unsigned int)readInt();
	unsigned char *data=(unsigned char *) &res;

	return (data[3]<<0) | (data[2]<<8) | (data[1]<<16) | (data[0]<<24);
}


std::string Stream::readline(){
	std::string res;

	res.resize(0x100);
	int startpos=(int)tell();

	while(1){
		size_t to_read=res.size();
		size_t count=read((char *)res.c_str(),to_read);

		if(count==0) return "";

		size_t pos=res.find('\n');
		if(pos!=-1){
			std::string res2(res,0,pos+1);
			seek(startpos+(int)pos+1,0);
			return res2;
		}

		if(count<to_read){
			res.append("\n");
			seek(startpos+(int)count,0);
			return res;
		}

		seek(startpos,0);
		res.resize(to_read*2);
	}
}
std::string Stream::readUntil(int ch){
	std::string res;

	res.resize(0x100);
	int startpos=(int)tell();

	while(1){
		size_t to_read=res.size();
		size_t count=read((char *)res.c_str(),to_read);

		if(count==0) return "";

		size_t pos=res.find(ch);
		if(pos!=-1){
			std::string res2(res,0,pos);
			seek(startpos+(int)pos+1,0);
			return res2;
		}

		if(count<to_read){
			seek(startpos+(int)count,0);
			return res;
		}

		seek(startpos,0);
		res.resize(to_read*2);
	}
}

Stream::~Stream(){
	close();
}


size_t FileStream::read(void *p,size_t count){
	return fread(p,1,count,file);
}
size_t FileStream::write(void *p,size_t count){
	return fwrite(p,1,count,file);
}
size_t FileStream::seek(int offset,int origin){
	fseek(file,offset,origin);

	size_t pos=ftell(file);

	return pos;
}
size_t FileStream::tell(){
	return ftell(file);
}
void FileStream::truncate(){
	_chsize( _fileno(file), (long)tell());
}
void FileStream::close(){
	if(owned && file!=NULL){
		fclose(file);
		file=NULL;
	}
}


FileStream::FileStream(FILE *f){
	owned=false;
	file=f;
}
FileStream::FileStream(TCHAR *fn,TCHAR *mode){
	owned=true;
	file=_tfopen(fn,mode);

	filename=fn;
}
FileStream::~FileStream(){
	close();
}

TmpFileStream::TmpFileStream(String fn,bool bak):
	FileStream(cstr(String(fn+_T(".tmp"))),_T("wb"))
{
	filename=fn;
	backup=bak;
}

void TmpFileStream::close(){
	if(file==NULL) return;
	FileStream::close();

	String tmpFilename=filename+_T(".tmp");
	String bakFilename=filename+_T(".bak");

	if(backup){
		DeleteFile(cstr(bakFilename));
		MoveFile(cstr(filename),cstr(bakFilename));
	}

	DeleteFile(cstr(filename));
	MoveFile(cstr(tmpFilename),cstr(filename));
}
TmpFileStream::~TmpFileStream(){
	close();
}

size_t IStreamStream::read(void *p,size_t count){
	ULONG cnt;
	
	stream->Read(p,(ULONG)count,&cnt);

	return cnt;
}
size_t IStreamStream::write(void *p,size_t count){
	ULONG cnt;
	
	stream->Write(p,(ULONG)count,&cnt);

	return cnt;
}
size_t IStreamStream::seek(int offset,int origin){
	ULARGE_INTEGER pos={0,};
	
	LARGE_INTEGER off;
	off.QuadPart=offset;
	
	stream->Seek(off,origin,&pos);

	return (size_t) pos.QuadPart;
}
size_t IStreamStream::tell(){
	return seek(0,1);
}
void IStreamStream::truncate(){
	ULARGE_INTEGER pos={0,};
	pos.QuadPart=tell();

	stream->SetSize(pos);
}
void IStreamStream::commit(){
	stream->Commit(0);
}

IStreamStream::IStreamStream(IStream *st){
	owned=true;
	stream=st;

	stream->AddRef();

	STATSTG statstg;
	stream->Stat(&statstg,0);

	filename=String(_T("<stream>/"))+statstg.pwcsName;

	CoTaskMemFree(statstg.pwcsName);
}
IStreamStream::~IStreamStream(){
	if(owned && stream!=NULL){
		stream->Release();
	} 
}


size_t MemoryStream::read(void *p,size_t count){
	if(position+count>size)
		count=size-position;

	memcpy(p,(char *)data+position,count);
	position+=(int)count;

	return count;
}
size_t MemoryStream::write(void *p,size_t count){
	expand((int)count);
	
	memcpy((char *)data+position,p,count);
	position+=(int)count;

	if((int)size<position) size=position;

	return count;
}

size_t MemoryStream::seek(int offset,int origin){
	switch(origin){
	case 0: position=0; break;
	case 1: break;
	case 2: position=(int)size; break;
	default: position=0; break;
	}

	position+=offset;

	if(position<0) position=0;
	if(position>(int)size) position=(int)size;

	return position;
}
size_t MemoryStream::tell(){
	return position;
}
void MemoryStream::truncate(){
	size=position;
}
MemoryStream::MemoryStream(){
	data=new char[allocated=1024];
	position=0;
	size=0;
}
MemoryStream::~MemoryStream(){
	if(data!=NULL) delete [] data;
}

void *MemoryStream::detach(){
	void *res=data;

	data=NULL;

	return res;
}

void MemoryStream::expand(int addition){
	size_t newAlloc=allocated;

	while(position+addition>(int)newAlloc)
		newAlloc*=2;

	if(newAlloc==allocated)
		return;

	void *d=new char[newAlloc];
	memcpy(d,data,allocated);
	delete [] data;

	data=d;
	allocated=newAlloc;
}
