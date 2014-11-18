#include "Thumbnailer.h"
#include "tchar.h"
#include "propkey.h"
#include "exdialog.h"
#include "ShellUtils.h"
#include "properties.h"
#include "utils.h"
#include <vector> 
#include <list>
#include <unordered_map> 
#include <Shlwapi.h>

void Log(TCHAR *s,...);

using namespace Magick;
using namespace std;

#define elems(x) (sizeof(x)/sizeof((x)[0]))

ThumbPropertyType propertyRawPid(1,_T("Dump3DSFile.RAWPID"),_T("Raw code, 3DS"));
ThumbPropertyType propertyPid(2,_T("Dump3DSFile.PID"),_T("Product code, 3DS"));
ThumbPropertyType propertyTitle(3,_T("Dump3DSFile.Title"),_T("Title, 3DS"));
ThumbPropertyType propertyRegion(4,_T("Dump3DSFile.Region"),_T("Region, 3DS"));
ThumbPropertyType propertyDate(5,_T("Dump3DSFile.Date"),_T("Release date, 3DS"));
ThumbPropertyType propertySoftwareRating(6,_T("Dump3DSFile.Rating"),_T("Rating, 3DS"));
ThumbPropertyType propertyPublisher(7,_T("Dump3DSFile.Publisher"),_T("Publisher, 3DS"));
ThumbPropertyType propertyProgramId(8,_T("Dump3DSFile.ProgramId"),_T("Program Id, 3DS"));
ThumbPropertyType propertyMediatype(9,_T("Dump3DSFile.Mediatype"),_T("Save type, 3DS"));
ThumbPropertyType propertyRating(PKEY_Rating,_T("System.Rating"));

class Thumbnailer3DS;

struct Thumb3DSGeneric :public Thumb{
	Thumb3DSGeneric(Stream *stream,Thumbnailer3DS *thumbnailer);

	unsigned long long programId;
	std::string pid;

	virtual void init();

	std::string title;
	std::string region;
	std::string date;
	std::string rating;
	std::string publisher;
	std::string boxshot;
	std::string barcode;
	std::string userRating;

	void ReadProperties();
	void WriteProperties();

	void readProps(Properties & props);
	void writeProps(Properties & props);

	void Thumbnail();
	virtual Image *getImage();

	Thumbnailer3DS *th;
	Stream *stream;
};

class Thumb3DS :public Thumb3DSGeneric{
public:
	Thumb3DS(Stream *stream,Thumbnailer3DS *thumbnailer);
	~Thumb3DS();

	unsigned char mediaType;

	std::string rawPid;

	void ReadProperties();

	void init();
};


class ThumbCIA :public Thumb3DSGeneric{
public:
	ThumbCIA(Stream *stream,Thumbnailer3DS *thumbnailer);
	~ThumbCIA();

	size_t imageOffset;
	Image customImage;
	Image *getImage();
	void init();
};

class Thumbnailer3DS :public Thumbnailer{
public:
	Thumbnailer3DS();
	~Thumbnailer3DS();

	Image bg,mask,overlay,empty;

	void init();
	Thumb *Process(Stream *stream);
};

Thumbnailer *CreateThumbnailer(){
	return new Thumbnailer3DS();
}

Thumb3DSGeneric::Thumb3DSGeneric(Stream *st,Thumbnailer3DS *thumbnailer){
	stream=st;
	th=thumbnailer;
}

void Thumb3DSGeneric::init(){
	Properties props;
	props.read(cstr(format(_T("%s\\info\\%s.txt"),dllDirectory,cstr(s2ws(pid)))));
	props.read(cstr(format(_T("%s\\user-info\\%s.txt"),dllDirectory,cstr(s2ws(pid)))));
	readProps(props);
}
void Thumb3DSGeneric::readProps(Properties & props){
	if(props.contains("Title"))			title=props.get("Title");
	if(props.contains("UserRating"))	userRating=props.get("UserRating");
	if(props.contains("Region"))		region=props.get("Region");
	if(props.contains("Date"))			date=props.get("Date");
	if(props.contains("Rating"))		rating=props.get("Rating");
	if(props.contains("Publisher"))		publisher=props.get("Publisher");
	if(props.contains("Boxshot"))		boxshot=props.get("Boxshot");
	if(props.contains("Barcode"))		barcode=props.get("Barcode");
}
void Thumb3DSGeneric::writeProps(Properties & props){
	props.set("Title",ws2s(propertyTitle.getValue(this)));
	props.set("UserRating",ws2s(propertyRating.getValue(this)));
	props.set("Region",ws2s(propertyRegion.getValue(this)));
	props.set("Date",ws2s(propertyDate.getValue(this)));
	props.set("Rating",ws2s(propertySoftwareRating.getValue(this)));
	props.set("Publisher",ws2s(propertyPublisher.getValue(this)));
	props.set("Boxshot",boxshot);
}
void Thumb3DSGeneric::ReadProperties(){
	propertyPid.setValue(this,s2ws(pid));

	propertyTitle.setValue(this,s2ws(title));
	propertyRegion.setValue(this,s2ws(region));
	propertyDate.setValue(this,s2ws(date));
	propertySoftwareRating.setValue(this,s2ws(rating));
	propertyPublisher.setValue(this,s2ws(publisher));
	propertyRating.setValue(this,s2ws(userRating));
	
	propertyProgramId.setValue(this,format(_T("%016llx"),programId));
}
void Thumb3DSGeneric::WriteProperties(){
	if(pid.empty()) return;

	Properties props;

	writeProps(props);

	props.write(cstr(format(_T("%s\\user-info\\%s.txt"),dllDirectory,cstr(s2ws(pid)))));
}
void Thumb3DSGeneric::Thumbnail(){
	Image result("256x256","transparent");

	int width=256,height=256;
	
	result.composite(th->bg,Geometry(width,width),OverCompositeOp);
	
	Image img;

	Image *customImg=getImage();
	if(! img.isValid()){
		String filename=format(_T("%s\\user-boxshot\\%s"),dllDirectory,cstr(s2ws(boxshot)));
		ReadImageFromFile(cstr(filename),&img);
	}
	if(! img.isValid()){
		String filename=format(_T("%s\\boxshot\\%s"),dllDirectory,cstr(s2ws(boxshot)));
		ReadImageFromFile(cstr(filename),&img);
	}
	if(customImg!=NULL && ! img.isValid()){
		img=*customImg;
	}
	if(! img.isValid()){
		img=th->empty;
	}
	
	if(img.columns()<=182/2) img.filterType(BoxFilter);
	img.resize(Geometry("182x"));
	
	Image iconfull(Geometry(width,width),"transparent");
	iconfull.composite(img,32,29,MagickCore::OverCompositeOp);	
	iconfull.composite(th->mask,CenterGravity,MagickCore::DstInCompositeOp);
	result.composite(iconfull,CenterGravity,OverCompositeOp);
	
	result.composite(th->overlay,CenterGravity,OverCompositeOp);

	image=result;
}
Image *Thumb3DSGeneric::getImage(){
	return NULL;
}


Thumb3DS::Thumb3DS(Stream *st,Thumbnailer3DS *thumbnailer) :Thumb3DSGeneric(st,thumbnailer){

}
Thumb3DS::~Thumb3DS(){

}
void Thumb3DS::init(){
	stream->seek(0x1150,0);

	char pidfield[0x11]={0,};
	stream->read(pidfield,0x10);

	rawPid=pidfield;
	pid=pidfield+6;

	for(const char *p=pid.c_str();*p!='\0';p++){
		char c=*p;

		if(c>='a' && c<='z') continue;
		if(c>='A' && c<='Z') continue;
		if(c>='0' && c<='9') continue;
		if(c=='-') continue;

		pid="";
		break;
	}

	if(pid.empty())
		return;

	stream->seek(0x18d,0);
	stream->read(&mediaType,1);

	stream->seek(0x1118,0);
	stream->read(&programId,8);

	Thumb3DSGeneric::init();
}

void Thumb3DS::ReadProperties(){
	Thumb3DSGeneric::ReadProperties();

	propertyRawPid.setValue(this,s2ws(rawPid));
	char *mediaTypes[]={ "Inner Device", "Card1", "Card2", "Extended Device" };
	if(mediaType<elems(mediaTypes))
		propertyMediatype.setValue(this,cs2ws(mediaTypes[mediaType]));
	else
		propertyMediatype.setValue(this,format(_T("Unknown(%d)"),mediaType));
}

ThumbCIA::ThumbCIA(Stream *st,Thumbnailer3DS *thumbnailer) :Thumb3DSGeneric(st,thumbnailer){

}
ThumbCIA::~ThumbCIA(){

}
void ThumbCIA::init(){
	imageOffset=-1;

	stream->seek(8,0);

	size_t sizeHeader=ALIGN(0x2020,64);
	size_t sizeCert=ALIGN(stream->readInt(),64);
	size_t sizeTicket=ALIGN(stream->readInt(),64);
	size_t sizeTMD=ALIGN(stream->readInt(),64);
	size_t sizeMeta=ALIGN(stream->readInt(),64);
	size_t sizeApp=ALIGN(stream->readInt(),64);

	size_t startTMD=sizeHeader+sizeCert+sizeTicket;

	if(sizeTMD==0)
		return;

	stream->seek((int)startTMD,0);
	int sigsize;
	int sigtype=stream->readIntBE();
	switch(sigtype){
	case 0x10000: sigsize=0x23c; break;
	case 0x10001: sigsize=0x13c; break;
	case 0x10002: sigsize=0x7c; break;
	case 0x10003: sigsize=0x23c; break;
	case 0x10004: sigsize=0x13c; break;
	case 0x10005: sigsize=0x7c; break;
	default: return;
	}

	stream->seek(sigsize,1);
	size_t dataTMD=stream->tell();

	stream->seek((int)dataTMD+0x4C,0);
	stream->read(&programId,8);

	pid=ws2s(format(_T("%016llx"),programId));

	Thumb3DSGeneric::init();

	size_t startSMDH=sizeHeader+sizeCert+sizeTicket+sizeTMD+sizeApp+0x400;

	char head[5]={0,};
	stream->seek((int)startSMDH,0);
	stream->read(head,4);
	if(strcmp(head,"SMDH")!=0)
		return;
	
	imageOffset=startSMDH+0x24C0;

	TCHAR longDescrField[0x80];
	TCHAR publisherField[0x40];

	stream->seek((int)startSMDH+8+0x200+0x80,0);
	stream->read(longDescrField,0x100);
	stream->read(publisherField,0x80);

	longDescrField[0x7f]='\0';
	publisherField[0x3f]='\0';

	title=ws2s(longDescrField);
	publisher=ws2s(publisherField);
}


static void decodeTile(int width,int iconSize, int tileSize, int ax, int ay, PixelPacket *pixels, Stream *fs){
	if (tileSize == 0){
		unsigned short color;
		fs->read(&color,2);
		
		int blue=(color>>0)&0x1f;
		int green=(color>>5)&0x3f;
		int red=(color>>11)&0x1f;

		Color magickColor=Color(red*QuantumRange/0x1f,green*QuantumRange/0x3f,blue*QuantumRange/0x1f);
		pixels[ax+ay*width]=magickColor;
	} else{
		for (int y = 0; y < iconSize; y += tileSize)
			for (int x = 0; x < iconSize; x += tileSize)
				decodeTile(width,tileSize, tileSize / 2, x + ax, y + ay, pixels, fs);
	}
}


Image *ThumbCIA::getImage(){
	if(imageOffset==-1)
		return NULL;

	int width=48;
	int height=48;
	int rows=width/4;
	int cols=height/4;

	Image image(Geometry(width,height), "white");

	image.type(TrueColorType);
	image.modifyImage();
	Pixels view(image);

	stream->seek((int)imageOffset,0);

	PixelPacket *pixels = view.get(0,0,width,height);
	for(int y=0;y<height;y+=8){
		for(int x=0;x<width;x+=8){
			 decodeTile(width, 8, 8, x, y, pixels, stream);
		}
	}

	view.sync();

	customImage=image;

	return &customImage;
}

extern "C" __declspec(dllexport) void CALLBACK WriteBoxshotW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow){
	FileStream stream(lpszCmdLine,_T("rb"));
	if(stream.file==NULL){
		MessageBox(hwnd,cstr(String(_T("File not found: "))+lpszCmdLine),_T("Error"),0);
		return;
	}

	Thumbnailer3DS thumbnailer;
	Thumb3DSGeneric *thumb=(Thumb3DSGeneric *) thumbnailer.Process(&stream);

	if(thumb==NULL || thumb->pid.empty()){
		MessageBox(hwnd,cstr(String(_T("Bad file: "))+lpszCmdLine),_T("Error"),0);
		return;
	}

	stream.close();

	Dialog dialog;
	
	dialog.title=_T("Choose picture");
	dialog.filter=_T("Known image types\0*.JPG;*.JPEG;*.PNG;*.GIF;*.BMP;*.TGA\0All files\0*.*\0");
	dialog.flags=OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;

	if(! dialog.open())
		return;

	FileStream input(cstr(dialog.filename),_T("rb"));
	if(input.file==NULL){
		MessageBox(hwnd,cstr(String(_T("Couldn't open file for reading: "))+dialog.filename),_T("Error"),0);
		return;
	}

	TCHAR *ext=PathFindExtension(cstr(dialog.filename));

	String outputFilename=format(_T("%s\\user-boxshot\\%s%s"),dllDirectory,cstr(s2ws(thumb->pid)),ext);
	FileStream output(cstr(outputFilename),_T("wb"));
	if(output.file==NULL){
		MessageBox(hwnd,cstr(String(_T("Couldn't open file for writing: "))+outputFilename),_T("Error"),0);
		return;
	}

	char data[0x400];
	size_t length;
	while((length=input.read(data,sizeof(data)))!=0){
		output.write(data,length);
	}

	output.close();
	input.close();

	String propLocation=format(_T("%s\\user-info\\%s.txt"),dllDirectory,cstr(s2ws(thumb->pid)));
	Properties props;
	props.read(cstr(format(_T("%s\\info\\%s.txt"),dllDirectory,cstr(s2ws(thumb->pid)))));
	props.read(cstr(propLocation));
	props.set("Boxshot",thumb->pid+ws2s(ext));
	props.write(cstr(propLocation));

	reloadExplorer();

	delete thumb;
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

	CreateDirectory(cstr(format(_T("%s\\user-info"),dllDirectory)),NULL);
	CreateDirectory(cstr(format(_T("%s\\user-boxshot"),dllDirectory)),NULL);

	extensions.push_back("3ds");
	extensions.push_back("3dz");
	extensions.push_back("cia");

	addFileCommand("Change picture...","WriteBoxshot");

	propertyTitle.editable=true;
	propertyRating.editable=true;
	propertyRegion.editable=true;
	propertyDate.editable=true;
	propertySoftwareRating.editable=true;
	propertyPublisher.editable=true;

	properties.push_back(&propertyPid);
	properties.push_back(&propertyTitle);
	properties.push_back(&propertyRegion);
	properties.push_back(&propertyDate);
	properties.push_back(&propertySoftwareRating);
	properties.push_back(&propertyPublisher);
	properties.push_back(&propertyRawPid);
	properties.push_back(&propertyProgramId);
	properties.push_back(&propertyMediatype);
	properties.push_back(&propertyRating);
}

Thumbnailer3DS::~Thumbnailer3DS(){
}

Thumb *Thumbnailer3DS::Process(Stream *stream){
	char head[5]={0,};
	char headCIA[5]={0x20,0x20,0,0,0};
	Thumb3DSGeneric *res=NULL;
	
	if(res==NULL){
		stream->read(head,4);
		stream->seek(0,0);

		if(*(int *)head==*(int *)headCIA){
			res=new ThumbCIA(stream,this);
		}
	}
	
	if(res==NULL){
		stream->seek(0x100,0);
		stream->read(head,4);
		stream->seek(0,0);

		if(strcmp(head,"NCSD")==0){
			res=new Thumb3DS(stream,this);
		}
	}
	
	if(res!=NULL){
		res->init();
	}

	return res;
}
