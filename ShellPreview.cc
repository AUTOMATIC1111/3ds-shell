// Implementation of DLL Exports.

#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <atlconv.h>
#include <comdef.h>
#include <shlobj.h>

#include <initguid.h>
#include "ShellPreview_h.h"
#include "ShellPreview_i.c"
#include "ShellPreviewExt.h"
#include "defines.h"
#include "Thumbnailer.h"
#include "ShellUtils.h"
#include <gdiplus.h>

TCHAR dllFilename[0x1000];
TCHAR dllShortFilename[0x1000];
TCHAR dllDirectory[0x1000];
TCHAR Guid[0x100];
Thumbnailer *thumbnailer;

HRESULT (*PropVariantToStringDLL)(REFPROPVARIANT propvar, PWSTR psz, UINT cch);
HRESULT (*PSRegisterPropertySchemaDLL)(PCWSTR pszPath);
HRESULT (*PSUnregisterPropertySchemaDLL)(PCWSTR pszPath);


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_ShellPreviewExt, CShellPreviewExt)
END_OBJECT_MAP()

HMODULE GetCurrentModuleHandle()
{
    HMODULE hMod = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
          reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
          &hMod);
     return hMod;
}

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/){
	HMODULE propsys=LoadLibrary(_T("propsys.dll"));
	if(propsys!=NULL){
		PropVariantToStringDLL=(HRESULT (*)(REFPROPVARIANT,PWSTR,UINT))GetProcAddress(propsys,"PropVariantToString");
		PSRegisterPropertySchemaDLL=(HRESULT (*)(PCWSTR))GetProcAddress(propsys,"PSRegisterPropertySchema");
		PSUnregisterPropertySchemaDLL=(HRESULT (*)(PCWSTR))GetProcAddress(propsys,"PSUnregisterPropertySchema");
	}

	if (dwReason == DLL_PROCESS_ATTACH){
		_stprintf(Guid,_T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
			CLSID_ShellPreviewExt.Data1,
			CLSID_ShellPreviewExt.Data2,
			CLSID_ShellPreviewExt.Data3,
			CLSID_ShellPreviewExt.Data4[0],
			CLSID_ShellPreviewExt.Data4[1],
			CLSID_ShellPreviewExt.Data4[2],
			CLSID_ShellPreviewExt.Data4[3],
			CLSID_ShellPreviewExt.Data4[4],
			CLSID_ShellPreviewExt.Data4[5],
			CLSID_ShellPreviewExt.Data4[6],
			CLSID_ShellPreviewExt.Data4[7]
		);

		GetModuleFileName(GetCurrentModuleHandle(),dllFilename,0x1000);
		_tcscpy(dllDirectory,dllFilename);
		PathRemoveFileSpec(dllDirectory);
		GetShortPathName(dllFilename,dllShortFilename,0x1000);

		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		/* Catch first UnableToOpenConfigureFile `type.xml' exception */
		try{
			Magick::Image img("256x256","transparent");
			img.annotate("a",Magick::CenterGravity);
		} catch(...){}

		try {
			thumbnailer=CreateThumbnailer();
		} catch(Magick::Exception &error){
			TCHAR explanation[0x200];
			mbstowcs(explanation,error.what(),0x200);
			Log(_T("%s\n"),explanation);
		}


        _Module.Init(ObjectMap, hInstance, &LIBID_FILEICONSLib);
        DisableThreadLibraryCalls(hInstance);
    } else if (dwReason == DLL_PROCESS_DETACH){
		delete thumbnailer;

        _Module.Term();
	}

	return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow()
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

bool RegistryWrite(HKEY root,LPCTSTR path,LPCTSTR key,LPCTSTR val){
	CRegKey regkey;
	LONG lRet;

	regkey.Create(root,path);
	lRet = regkey.Open ( root,path, KEY_SET_VALUE );

	if ( ERROR_SUCCESS == lRet ){
		lRet = regkey.SetValue ( val, key );
	} else{
		regkey.Close();
		return false;
	}
	
	regkey.Close();
	return lRet==ERROR_SUCCESS;
}
String RegistryRead(HKEY root,LPCTSTR path,LPCTSTR key){
	CRegKey regkey;

	regkey.Create(root,path);

	if (regkey.Open(root,path,KEY_READ)!=ERROR_SUCCESS){
		regkey.Close();
		return _T("");
	}
	
	TCHAR value[0x200]=_T("");
	ULONG nchars=sizeof(value)/sizeof(value[0]);

	regkey.QueryStringValue(key,value,&nchars);
	regkey.Close();
	return value;
}
bool RegistryCreate(HKEY root,LPCTSTR path){
	CRegKey regkey;
	LONG lRet;

	lRet = regkey.Create(root,path);

	return lRet==ERROR_SUCCESS;
}

bool RegistryDelete(HKEY root,LPCTSTR path,LPCTSTR key){
	CRegKey regkey;
	LONG lRet;

	lRet = regkey.Open ( root,path, KEY_SET_VALUE|KEY_CREATE_SUB_KEY );

	if ( ERROR_SUCCESS == lRet ){
		lRet = regkey.DeleteValue ( key );
	} else{
		regkey.Close();
		return false;
	}
	
	regkey.Close();
	return lRet==ERROR_SUCCESS;
}

void touch(char *filename){
	FILE *f=fopen(filename,"w");
	fclose(f);
} 

STDAPI DllRegisterServer(){
	TCHAR key[0x1000];
	
	if(thumbnailer->extensions.empty())
		thumbnailer->extensions.push_back(EXTENSION);
	
	std::vector<String> types;
	types.push_back(_T(CLASSNAME));
	
	for(int i=0; i < thumbnailer->extensions.size(); i++){
		String regKey=_T(".")+s2ws(thumbnailer->extensions[i]);
		RegistryWrite(HKEY_CLASSES_ROOT,cstr(regKey),_T(""),_T(CLASSNAME));
		
		/* If user has custom handler for file type, insert \\ShellEx\\IconHandler
		 * into it as well. */
		String regPath=String(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\"))+regKey+_T("\\UserChoice");
		
		String handler=RegistryRead(HKEY_CURRENT_USER,cstr(regPath),_T("Progid"));
		if(! handler.empty()){
			types.push_back(handler);
		}
	}

	for(int i=0; i < types.size(); i++){
		String type=types[i];

		RegistryCreate(HKEY_CLASSES_ROOT,cstr(type));
		RegistryCreate(HKEY_CLASSES_ROOT,cstr(type+_T("\\ShellEx")));
		RegistryWrite(HKEY_CLASSES_ROOT,cstr(type+_T("\\ShellEx\\IconHandler")),_T(""),Guid);
	}

	_stprintf(key,_T("CLSID\\%s"),Guid);
	RegistryCreate(HKEY_CLASSES_ROOT,key);
	RegistryWrite(HKEY_CLASSES_ROOT,key,_T(""),_T("ShellPreviewExt Class"));
	
	_stprintf(key,_T("CLSID\\%s\\InprocServer32"),Guid);
	RegistryCreate(HKEY_CLASSES_ROOT,key);
	RegistryWrite(HKEY_CLASSES_ROOT,key,_T(""),dllFilename);
	RegistryWrite(HKEY_CLASSES_ROOT,key,_T("ThreadingModel"),_T("Apartment"));


	/* property handler */
	for(int i=0; i < thumbnailer->extensions.size(); i++){
		String regKey=_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\PropertySystem\\PropertyHandlers\\.")+s2ws(thumbnailer->extensions[i]);
		RegistryWrite(HKEY_LOCAL_MACHINE,cstr(regKey),_T(""),Guid);
	}
	
	thumbnailer->registerProperties();
	String propInfo=_T("prop:System.Size");
	std::vector<ThumbPropertyType *>::iterator iter;
	for(iter=thumbnailer->properties.begin();iter!=thumbnailer->properties.end();iter++){
		propInfo+=_T(";");
		propInfo+=(*iter)->name;
	}
	propInfo+=_T(";System.DateChanged;System.DateCreated;System.ItemFolderPathDisplay");
	
	
	for(int i=0; i < types.size(); i++){
		String type=types[i];

		RegistryWrite(HKEY_CLASSES_ROOT,cstr(type),_T("PreviewDetails"),cstr(propInfo));
		RegistryWrite(HKEY_CLASSES_ROOT,cstr(type+_T("\\DefaultIcon")),_T(""),_T("%1"));

		/* register moveover hint handler */
		RegistryWrite(HKEY_CLASSES_ROOT,cstr(type+_T("\\ShellEx\\{00021500-0000-0000-C000-000000000046}")) ,_T(""),Guid);

		/* register thumb handler */
		RegistryWrite(HKEY_CLASSES_ROOT,cstr(type+_T("\\ShellEx\\{E357FCCD-A995-4576-B01F-234630154E96}")),_T(""),Guid);
		RegistryWrite(HKEY_CLASSES_ROOT,cstr(type),_T("Treatment"),_T("0"));
	
		/* Register explorer context click menu commands */
		for(int i=0; i < thumbnailer->fileCommands.size(); i++){
			std::string text=thumbnailer->fileCommands[i].first;
			std::string function=thumbnailer->fileCommands[i].second;
			String command=String(_T("rundll32 "))+dllShortFilename+_T(",")+s2ws(function)+_T(" %1");

			RegistryCreate(HKEY_CLASSES_ROOT,cstr(type+_T("\\shell")));
			RegistryCreate(HKEY_CLASSES_ROOT,cstr(type+_T("\\shell\\")+s2ws(text)));
			RegistryCreate(HKEY_CLASSES_ROOT,cstr(type+_T("\\shell\\")+s2ws(text)+_T("\\command")));
			RegistryWrite(HKEY_CLASSES_ROOT,cstr(type+_T("\\shell\\")+s2ws(text)+_T("\\command")),_T(""),cstr(command));
		}
	}

	RegistryWrite(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),Guid,_T(DESCRIPTION));
    

    HRESULT res=_Module.RegisterServer(false);
	
	if(res==S_OK) touch("registered");

	reloadExplorer();

	return res;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(){
	RegistryDelete(HKEY_CLASSES_ROOT,_T(CLASSNAME) _T("\\DefaultIcon"),_T(""));
	RegistryDelete(HKEY_CLASSES_ROOT,_T(CLASSNAME) _T("\\ShellEx\\IconHandler"),_T(""));
	RegistryDelete(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),Guid);
	
	thumbnailer->unregisterProperties();

    HRESULT res=_Module.UnregisterServer(false);

	if(res==S_OK) touch("unregistered");

	reloadExplorer();
    CoFreeUnusedLibraries ();

	return res;
}
