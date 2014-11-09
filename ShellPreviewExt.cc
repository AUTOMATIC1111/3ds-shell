#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <atlconv.h>
#include <comdef.h>
#include <shlobj.h>

#include "ShellPreview_h.h"
#include "ShellPreviewExt.h"


#include <Magick++.h>
#include <magick/composite.h>
#include "thumbnailer.h"
#include "exception.h"

#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

using namespace Magick;

#if 1
static FILE *logfile;
void Log(TCHAR *fmt,...){
	va_list args;
	va_start(args,fmt);

	if(logfile==NULL){
		TCHAR filename[0x1000];
		_stprintf(filename,_T("%s\\log.txt"),dllDirectory);
		logfile=_tfopen(filename,_T("a"));
	}

	if(logfile!=NULL){
		std::string s=ws2s(format(fmt,args));
		fprintf(logfile,"%s",cstr(s));
		fflush(logfile);
	}

	va_end(args);
}
#else
void Log(TCHAR *fmt,...){}
#endif

CShellPreviewExt::CShellPreviewExt(){
	stream=NULL;
	thumb=NULL;
	processed=false;

	memset(taskFinished,0,sizeof(taskFinished));

	int index=0;
	std::vector<ThumbPropertyType *>::iterator iter;
	for(iter=thumbnailer->properties.begin();iter!=thumbnailer->properties.end();iter++){
		(*iter)->index=index++;
	}

	InitializeCriticalSectionAndSpinCount(&creationCriticalSection,0x00000400); 
}
CShellPreviewExt::~CShellPreviewExt(){
	if(stream!=NULL) delete stream;
	if(thumb!=NULL) delete thumb;

	DeleteCriticalSection(&creationCriticalSection);
}

struct Mutex{
	CRITICAL_SECTION *cs;

	Mutex(CRITICAL_SECTION *c){
		cs=c;
		EnterCriticalSection(cs);
	}
	~Mutex(){
		LeaveCriticalSection(cs);
	}
};

void CShellPreviewExt::process(int task){
	if(stream==NULL) return;

	try{

		{
			Mutex mutex(&creationCriticalSection);

			if(thumb==NULL)
				thumb=thumbnailer->Process(stream);
		}

		

		if(thumb!=NULL){
			if(!taskFinished[task]) switch(task){
			case 0: break;
			case 1: thumb->Thumbnail(); break;
			case 2: thumb->ReadProperties(); break;
			case 3: thumb->WriteProperties(); break;
			}
		}
		taskFinished[task]++;
	} catch( Exception &error_ ) {
		String explanation=cs2ws(error_.what());
		Log(_T("%s: %s\n"),stream->filename.c_str(),cstr(explanation)); 
	} catch( GeneralException &e ) {
		Log(_T("%s: %s\n"),cstr(stream->filename),cstr(e.info)); 
	} catch(...) {
		Log(_T("%s: oooops!\n"),stream->filename.c_str()); 
	}
}


STDMETHODIMP CShellPreviewExt::Initialize(IStream *pstream,DWORD grfMode){
	if(stream!=NULL)
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);

	stream=new IStreamStream(pstream);

	return S_OK;
}
STDMETHODIMP CShellPreviewExt::Load( LPCOLESTR wszFile, DWORD ){ 
	USES_CONVERSION;

	if(stream!=NULL) delete stream;
	stream=new FileStream((TCHAR *)wszFile,_T("rb"));

	return S_OK;
}
STDMETHODIMP CShellPreviewExt::GetInfoTip(DWORD dwFlags, LPWSTR* ppwszTip){
	if(ppwszTip==NULL) return E_FAIL;
	/*
	process();

	if(thumb.description[0]=='\0'){
		*ppwszTip=NULL;
		return S_OK;
	}
	
	size_t len=_tcslen(thumb.description);
	*ppwszTip=(LPWSTR) CoTaskMemAlloc((len+1)*sizeof(TCHAR));
	_tcscpy(*ppwszTip,thumb.description);
	*/
    return S_OK;
}

HBITMAP hbmpFromImage(Image orig,int w){
	Blob res;
	HBITMAP hbitmap=NULL;

	Magick::Geometry geo(w,w);

	Image image(orig);
	image.resize(geo);
	image.write(&res,"PNG");

	const void *d=res.data();
	size_t l=res.length();

	CComPtr<IStream> pStream = NULL;
	HGLOBAL m_hBuffer  = ::GlobalAlloc(GMEM_FIXED, l);

	LPVOID pImage = ::GlobalLock(m_hBuffer);
	memcpy(pImage,d,l);
	::GlobalUnlock(m_hBuffer);

	::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream);

	Gdiplus::Bitmap b(pStream);

	b.GetHBITMAP(Gdiplus::Color(0,0,0,0),&hbitmap);

	GlobalFree(m_hBuffer);

	return hbitmap;
}

HICON fromImage(Image orig,int w){
	Blob res;
	HICON hicon=NULL;

	Magick::Geometry geo(w,w);

	Image image(orig);
	image.resize(geo);
	image.write(&res,"PNG");

	const void *d=res.data();
	size_t l=res.length();

	CComPtr<IStream> pStream = NULL;
	HGLOBAL m_hBuffer  = ::GlobalAlloc(GMEM_FIXED, l);

	LPVOID pImage = ::GlobalLock(m_hBuffer);
	memcpy(pImage,d,l);
	::GlobalUnlock(m_hBuffer);

	::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream);

	Gdiplus::Bitmap b(pStream);

	b.GetHICON(&hicon);

	GlobalFree(m_hBuffer);

	return hicon;
}

#ifdef THUMB_METHOD_IEXTRACTICON
STDMETHODIMP CShellPreviewExt::GetIconLocation (UINT uFlags,  LPTSTR szIconFile, UINT cchMax,int* piIndex, UINT* pwFlags ){
    *pwFlags = GIL_NOTFILENAME | GIL_DONTCACHE;

    return S_OK;
}
STDMETHODIMP CShellPreviewExt::Extract (LPCTSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall,UINT nIconSize ){
	WORD wSmallIconSize = HIWORD(nIconSize), wLargeIconSize = LOWORD(nIconSize);

	process(1);

	if(thumb==NULL)
		return S_OK;

	if(! thumb->image.isValid())
		return S_OK;

	try{
		if (phiconLarge!=NULL) *phiconLarge=fromImage(thumb->image,wLargeIconSize);
		if (phiconSmall!=NULL) *phiconSmall=fromImage(thumb->image,wSmallIconSize);
	} catch( Exception &error_ ) {
		TCHAR explanation[0x200];
		mbstowcs(explanation,error_.what(),0x200);
		Log(_T("  Caught exception: %s\n"),explanation); 
	} catch(...) {
		Log(_T("  Oooops!\n")); 
	}

	return S_OK;
}
#endif

#ifdef THUMB_METHOD_ITHUMBPROVIDER
STDMETHODIMP CShellPreviewExt::GetThumbnail(UINT cx,HBITMAP *phbmp,WTS_ALPHATYPE *pdwAlpha){
	*pdwAlpha=WTSAT_ARGB;

	process();

	if(! thumb.image.isValid())
		return S_OK;

	try{
		*phbmp=hbmpFromImage(thumb.image,cx);
	} catch( Exception &error_ ) {
		TCHAR explanation[0x200];
		mbstowcs(explanation,error_.what(),0x200);
		Log(_T("  Caught exception: %s\n"),explanation); 
	} catch(...) {
		Log(_T("  Oooops!\n")); 
	}

	return S_OK;
}
#endif

IFACEMETHODIMP CShellPreviewExt::GetCount(DWORD *pcProps){
	process(0);
	if(thumb==NULL || thumb->properies==NULL) return S_OK;

	*pcProps=(DWORD)thumbnailer->properties.size();

	return S_OK;
}

IFACEMETHODIMP CShellPreviewExt::GetAt(DWORD iProp, PROPERTYKEY *pkey){
	process(0);
	if(thumb==NULL || thumb->properies==NULL) return S_OK;

	if(iProp>=thumbnailer->properties.size()) return S_OK;
	
	*pkey=thumbnailer->properties.at(iProp)->key;

	return S_OK;
}

//PKEY_Comment
IFACEMETHODIMP CShellPreviewExt::GetValue(REFPROPERTYKEY key, PROPVARIANT *pPropVar){
	process(2);
	if(thumb==NULL || thumb->properies==NULL) return S_OK;

	std::vector<ThumbPropertyType *>::iterator iter;

	for(iter=thumbnailer->properties.begin();iter!=thumbnailer->properties.end();iter++){
		REFPROPERTYKEY myKey=(*iter)->key;
		if(key!=myKey) continue;
		
		int index=(*iter)->index;
		InitPropVariantFromString(cstr(thumb->properies[index].value),pPropVar);
		break;
	}

	return S_OK;
}

IFACEMETHODIMP CShellPreviewExt::SetValue(REFPROPERTYKEY key, REFPROPVARIANT propVar){
	process(0);
	if(thumb==NULL || thumb->properies==NULL) return S_OK;
	
	TCHAR value[0x200];
	PropVariantToStringDLL(propVar,value,0x200);
	
	thumbnailer->setProperty(thumb,key,value);

	return S_OK;
}

IFACEMETHODIMP CShellPreviewExt::Commit(){
	process(3);
	if(thumb==NULL) return S_OK;

	stream->commit();

	return S_OK;
}

IFACEMETHODIMP CShellPreviewExt::IsPropertyWritable(REFPROPERTYKEY key){
	process(0);
	if(thumb==NULL || thumb->properies==NULL) return S_OK;

	std::vector<ThumbPropertyType *>::iterator iter;
	for(iter=thumbnailer->properties.begin();iter!=thumbnailer->properties.end();iter++){
		REFPROPERTYKEY myKey=(*iter)->key;
		if(key!=myKey) continue;
		
		if((*iter)->editable)
			return S_OK;

		break;
	}

	return S_FALSE;
}
