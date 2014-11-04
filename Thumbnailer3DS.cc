#include "Thumbnailer.h"
#include "tchar.h"
#include "propkey.h"
#include "ShellUtils.h"
#include <vector> 
#include <list>
#include <unordered_map> 

void Log(TCHAR *s,...);

using namespace Magick;
using namespace std;

#define elems(x) (sizeof(x)/sizeof((x)[0]))

ThumbPropertyType propertyRawPid(1,_T("Dump3DSFile.RAWPID"),_T("Raw product ID, 3DS"));
ThumbPropertyType propertyPid(2,_T("Dump3DSFile.PID"),_T("Product ID, 3DS"));
ThumbPropertyType propertyTitle(3,_T("Dump3DSFile.Title"),_T("Title, 3DS"));
ThumbPropertyType propertyRegion(4,_T("Dump3DSFile.Region"),_T("Region, 3DS"));
ThumbPropertyType propertyDate(5,_T("Dump3DSFile.Date"),_T("Release date, 3DS"));
ThumbPropertyType propertyRating(6,_T("Dump3DSFile.Rating"),_T("Rating, 3DS"));
ThumbPropertyType propertyPublisher(7,_T("Dump3DSFile.Publisher"),_T("Publisher, 3DS"));

class Thumbnailer3DS;

class Thumb3DS :public Thumb{
public:
	Thumb3DS(Stream *stream,Thumbnailer3DS *thumbnailer);
	~Thumb3DS();

	Thumbnailer3DS *th;

	std::string pid;
	std::string rawPid;

	std::string title;
	std::string region;
	std::string date;
	std::string rating;
	std::string publisher;
	std::string boxshot;
	std::string barcode;

	void Thumbnail();
	void ReadProperties();
	void WriteProperties();

	Stream *stream;
};

class Thumbnailer3DS :public Thumbnailer{
public:
	Thumbnailer3DS();
	~Thumbnailer3DS();

	Image bg,mask,overlay,empty;

	Thumb *Process(Stream *stream);
};

Thumbnailer *CreateThumbnailer(){
	return new Thumbnailer3DS();
}

Thumb3DS::Thumb3DS(Stream *st,Thumbnailer3DS *thumbnailer){
	stream=st;
	th=thumbnailer;

	stream->seek(0x1150,0);

	char pidfield[0x11]={0,};
	stream->read(pidfield,0x10);

	rawPid=pidfield;
	pid=pidfield+6;

	String filename=format(_T("%s\\info\\%s.txt"),dllDirectory,cstr(s2ws(pid)));
	FileStream infostream(cstr(filename),_T("r"));
	if(infostream.file!=NULL){
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
			
			if(strcmp(s,"Title")==0)
				title=p;
			else if(strcmp(s,"Region")==0)
				region=p;
			else if(strcmp(s,"Date")==0)
				date=p;
			else if(strcmp(s,"Rating")==0)
				rating=p;
			else if(strcmp(s,"Publisher")==0)
				publisher=p;
			else if(strcmp(s,"Boxshot")==0)
				boxshot=p;
			else if(strcmp(s,"Barcode")==0)
				barcode=p;
		}
	}

}
Thumb3DS::~Thumb3DS(){

}

void Thumb3DS::Thumbnail(){
	Image result("256x256","transparent");

	int width=256,height=256;
	
	result.composite(th->bg,Geometry(width,width),OverCompositeOp);
	
	Image img;
	String filename=format(_T("%s\\boxshot\\%s"),dllDirectory,cstr(s2ws(boxshot)));
	ReadImageFromFile(cstr(filename),&img);
	if(! img.isValid()){
		img=th->empty;
	}
	img.resize(Geometry(182,182));
	
	Image iconfull(Geometry(width,width),"transparent");
	iconfull.composite(img,32,29,MagickCore::OverCompositeOp);	
	iconfull.composite(th->mask,CenterGravity,MagickCore::DstInCompositeOp);
	result.composite(iconfull,CenterGravity,OverCompositeOp);
	
	result.composite(th->overlay,CenterGravity,OverCompositeOp);

	
	std::string s=std::string("b:/")+pid+".png";
	result.write(s);
	image=result;
}
void Thumb3DS::ReadProperties(){
	
	propertyPid.setValue(this,s2ws(pid));

	propertyTitle.setValue(this,s2ws(title));
	propertyRegion.setValue(this,s2ws(region));
	propertyDate.setValue(this,s2ws(date));
	propertyRating.setValue(this,s2ws(rating));
	propertyPublisher.setValue(this,s2ws(publisher));

	propertyRawPid.setValue(this,s2ws(rawPid));
}
void Thumb3DS::WriteProperties(){
}

Thumbnailer3DS::Thumbnailer3DS(){
	TCHAR filename[0x1000];
	
	_stprintf(filename,_T("%s\\images\\bg.png"),dllDirectory);
	ReadImageFromFile(filename,&bg);

	_stprintf(filename,_T("%s\\images\\mask.png"),dllDirectory);
	ReadImageFromFile(filename,&mask);

	_stprintf(filename,_T("%s\\images\\overlay.png"),dllDirectory);
	ReadImageFromFile(filename,&overlay);

	_stprintf(filename,_T("%s\\images\\empty.png"),dllDirectory);
	ReadImageFromFile(filename,&empty);

	extensions.push_back("3ds");
	extensions.push_back("3dz");

	properties.push_back(&propertyPid);
	properties.push_back(&propertyTitle);
	properties.push_back(&propertyRegion);
	properties.push_back(&propertyDate);
	properties.push_back(&propertyRating);
	properties.push_back(&propertyPublisher);
	properties.push_back(&propertyRawPid);
}

Thumbnailer3DS::~Thumbnailer3DS(){
}

Thumb *Thumbnailer3DS::Process(Stream *stream){
	return new Thumb3DS(stream,this);
}
