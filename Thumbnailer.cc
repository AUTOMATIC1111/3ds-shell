#include "Thumbnailer.h"
#include "tchar.h"
#include "exstream.h"
#include "Propsys.h"
#include "defines.h"

//#pragma comment (lib,"Propsys.lib")

using namespace Magick;

Thumb::Thumb(){
	properies=NULL;
}

Thumb::~Thumb(){
	if(properies!=NULL)
		delete [] properies;
}
/*
void Thumb::describe(TCHAR *fmt,...){
	TCHAR ds2[0x400];

	int remaining=0x400-(int)desc_offset;

	if(description[0]!='\0'){
		if(remaining<2) return;
		remaining-=2;
		
		description[desc_offset++]='\r';
		description[desc_offset++]='\n';
	}

	va_list args;
	va_start(args,fmt);

	_vsntprintf(ds2,0x400,fmt,args);
	ds2[0x400-1]='\0';

	va_end(args);

	size_t len=_tcslen(ds2);
	if(len>=remaining) return;
	memcpy(description+desc_offset,ds2,(len+1)*sizeof(TCHAR));
	desc_offset+=len;
}
*/
ThumbPropertyType::ThumbPropertyType(int i,std::tstring n,std::tstring l){
	name=n;
	label=l;

	invitationText=(std::tstring)_T("Specify ")+lcfirst(label);

	key.pid=i;
	HRESULT res=CLSIDFromString(_T("{") _T(QUOTE(GUID_PROPERTY) _T("}")), &key.fmtid);

	editable=false;
}
ThumbPropertyType::ThumbPropertyType(PROPERTYKEY k,std::tstring n){
	name=n;
	key=k;

	editable=false;
}

void ThumbPropertyType::setValue(Thumb *thumb,String value){
	if(thumb->properies==NULL)
		thumb->properies=new ThumbProperty[thumbnailer->properties.size()];

	thumb->properies[index].value=value;
}
String ThumbPropertyType::getValue(Thumb *thumb){
	if(thumb->properies==NULL) return _T("");
	
	return thumb->properies[index].value;
}

void Thumbnailer::registerProperties(){
	if(LOBYTE(LOWORD(GetVersion()))<6) return;

	int count=0;
	for(std::vector<ThumbPropertyType *>::iterator iter=properties.begin();iter!=properties.end();iter++){
		if((*iter)->label.empty()) continue;

		count++;
	}

	if(count==0) return;

	String filename=format(_T("%s\\") _T(EXTENSION) _T(".propdesc"),dllDirectory);
	FileStream stream(cstr(filename),_T("w"));

	fprintf(stream.file,
		"<?xml version='1.0' encoding='utf-8'?>" "\n"
		"<schema xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'" "\n"
		"        xmlns='http://schemas.microsoft.com/windows/2006/propertydescription'" "\n"
		"        schemaVersion='1.0'>" "\n"
		"  <propertyDescriptionList publisher='Microsoft' product='Shell Extension'>" "\n"
	);

	for(std::vector<ThumbPropertyType *>::iterator iter=properties.begin();iter!=properties.end();iter++){
		if((*iter)->label.empty()) continue;

		std::string name=ws2s((*iter)->name);
		std::string description=ws2s((*iter)->description);
		std::string label=ws2s((*iter)->label);
		std::string invitationText=ws2s((*iter)->invitationText);

		fprintf(stream.file,
			"    <propertyDescription name='%s' formatID='{" QUOTE(GUID_PROPERTY) "}' propID='%d'>" "\n"
			"      <description>%s</description>" "\n"
			"      <searchInfo inInvertedIndex='true' isColumn='true' columnIndexType='OnDisk'/>" "\n"
			"      <typeInfo type='String' multipleValues='false' isViewable='true' isQueryable='true'/>" "\n"
			"      <labelInfo label='%s' invitationText='%s' />" "\n"
			"    </propertyDescription>" "\n",
			name.c_str(),
			(*iter)->key.pid,
			description.c_str(),
			label.c_str(),
			invitationText.c_str()
		);
	}

	fprintf(stream.file,
		"  </propertyDescriptionList>" "\n"
		"</schema>" "\n"
		"" "\n"
	);

	stream.close();

	PSUnregisterPropertySchemaDLL(cstr(filename));
	PSRegisterPropertySchemaDLL(cstr(filename));
}
void Thumbnailer::unregisterProperties(){
	String filename=format(_T("%s\\") _T(EXTENSION) _T(".propdesc"),dllDirectory);
	PSUnregisterPropertySchemaDLL(cstr(filename));
}

void Thumbnailer::setProperty(Thumb *thumb,const PROPERTYKEY & key,TCHAR *value){
	for(std::vector<ThumbPropertyType *>::iterator iter=properties.begin();iter!=properties.end();iter++){
		if(key!=(*iter)->key) continue;

		(*iter)->setValue(thumb,value);
	}
}

void ReadImageFromFile(TCHAR *filename,Magick::Image *result){
	FILE *f=_tfopen(filename,_T("rb"));
	if(f==NULL) return;

	fseek(f,0,2);
	size_t length=ftell(f);
	fseek(f,0,0);

	char *data=(char *)malloc(length);
	if(data==NULL){
		fclose(f);
		return;
	}

	fread(data,1,length,f);

	fclose(f);

	Blob blob(data,length);
	result->read(blob);

	free(data);
}
