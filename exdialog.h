#ifndef __EXDIALOG__
#define __EXDIALOG__

#include "ShellUtils.h"

class Dialog{
public:
    Dialog();

	String defaultExtension;
	String filename;
	
	TCHAR *filter;

    int flags;
    String initialDirectory;

    String title;

    bool open();
};

#endif
