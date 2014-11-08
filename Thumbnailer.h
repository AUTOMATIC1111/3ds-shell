#ifndef __THUMBNAILER__H__
#define __THUMBNAILER__H__

#include "ShellUtils.h"
#include <Magick++.h>
#include <vector>

extern TCHAR dllDirectory[];

struct ThumbProperty{
	String value;
};

class Thumb{
public:
	Magick::Image image;
	ThumbProperty *properies;

	Thumb();
	virtual ~Thumb();

	virtual void Thumbnail()=0;
	virtual void ReadProperties()=0;
	virtual void WriteProperties()=0;
};

struct ThumbPropertyType{
	std::tstring name;
	bool editable;

	std::tstring label;
	std::tstring invitationText;
	std::tstring description;

	ThumbPropertyType(int id,std::tstring name,std::tstring label);
	ThumbPropertyType(PROPERTYKEY key,std::tstring name);

	void setValue(Thumb *thumb,String value);
	String getValue(Thumb *thumb);

	PROPERTYKEY key;
	int index;
};

class Thumbnailer{
public:
	virtual ~Thumbnailer(){};

	std::vector<std::string> extensions;
	std::vector<std::pair<std::string,std::string>> fileCommands;

	void addFileCommand(std::string text,std::string functionName);

	std::vector<ThumbPropertyType *> properties;
	void registerProperties();
	void unregisterProperties();

	void setProperty(Thumb *thumb,const PROPERTYKEY &key,TCHAR *value);

	virtual Thumb *Process(Stream *stream)=0;
};

void ReadImageFromFile(TCHAR *filename,Magick::Image *result);

Thumbnailer *CreateThumbnailer();
extern Thumbnailer *thumbnailer;

#endif
