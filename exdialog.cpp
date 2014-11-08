#include "exdialog.h"

Dialog::Dialog(){
	flags=0;
}

bool Dialog::open(){
	OPENFILENAME ofn ;

	ZeroMemory(&ofn, sizeof(ofn));
	TCHAR fn[0x1000];
	_sntprintf(fn,0x100,_T("%s"),cstr(filename));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrDefExt = cstr(defaultExtension);
	ofn.lpstrFile = fn;
	ofn.nMaxFile = 0x1000;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 0;
	ofn.lpstrInitialDir = 0;
	ofn.lpstrTitle = cstr(title);
	ofn.Flags = flags;

	GetOpenFileName(&ofn);
	filename=fn;

	if(fn[0]=='\0')
		return false;

	return true;
}
