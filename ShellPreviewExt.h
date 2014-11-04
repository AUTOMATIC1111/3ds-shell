#ifndef __SHELLPREVIEWEXT_H_
#define __SHELLPREVIEWEXT_H_

#include "Thumbnailer.h"
#include "defines.h"
#include "exstream.h"
#include "Thumbcache.h"
#include "Propsys.h"
#include "Propkey.h"
#include "propvarutil.h"

void Log(TCHAR *mask,...);

class ATL_NO_VTABLE CShellPreviewExt :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CShellPreviewExt, &CLSID_ShellPreviewExt>,
    public IPropertyStore,
    public IPropertyStoreCapabilities,
#ifdef MOUSEOVER_HINTS
	public IQueryInfo,
#endif
#ifdef THUMB_METHOD_ITHUMBPROVIDER
	public IThumbnailProvider,
#endif
#ifdef THUMB_METHOD_IEXTRACTICON
	public IExtractIcon,
#endif
	public IPersistFile,
	public IInitializeWithStream

{
public:
    CShellPreviewExt();
    ~CShellPreviewExt();

    BEGIN_COM_MAP(CShellPreviewExt)
		COM_INTERFACE_ENTRY(IPropertyStore)
		COM_INTERFACE_ENTRY(IPropertyStoreCapabilities)
#ifdef MOUSEOVER_HINTS
		COM_INTERFACE_ENTRY(IQueryInfo)
#endif
#ifdef THUMB_METHOD_ITHUMBPROVIDER
		COM_INTERFACE_ENTRY(IThumbnailProvider)
#endif
#ifdef THUMB_METHOD_IEXTRACTICON
		COM_INTERFACE_ENTRY(IExtractIcon)
#endif 
        COM_INTERFACE_ENTRY(IPersistFile)
        COM_INTERFACE_ENTRY(IInitializeWithStream)
    END_COM_MAP()

    DECLARE_REGISTRY(0,_T(""),_T(""),0u,THREADFLAGS_APARTMENT)

public:
    // IPersistFile
    STDMETHODIMP GetClassID( CLSID* ) { return E_NOTIMPL; }
    STDMETHODIMP IsDirty() { return E_NOTIMPL; }
    STDMETHODIMP Save( LPCOLESTR, BOOL ) { return E_NOTIMPL; }
    STDMETHODIMP SaveCompleted( LPCOLESTR ) { return E_NOTIMPL; }
    STDMETHODIMP GetCurFile( LPOLESTR* ) { return E_NOTIMPL; }

	STDMETHODIMP Initialize(IStream *pstream,DWORD grfMode);
    STDMETHODIMP Load( LPCOLESTR wszFile, DWORD );

	// IQueryInfo
	STDMETHODIMP GetInfoFlags(DWORD*)     { return E_NOTIMPL; }
	STDMETHODIMP GetInfoTip(DWORD, LPWSTR*);

#ifdef THUMB_METHOD_IEXTRACTICON
    STDMETHODIMP GetIconLocation( UINT uFlags, LPTSTR szIconFile, UINT cchMax,
                                  int* piIndex, UINT* pwFlags );
    STDMETHODIMP Extract( LPCTSTR pszFile, UINT nIconIndex, HICON* phiconLarge,
                          HICON* phiconSmall, UINT nIconSize );
#endif

#ifdef THUMB_METHOD_ITHUMBPROVIDER
	STDMETHODIMP GetThumbnail(UINT cx,HBITMAP *phbmp,WTS_ALPHATYPE *pdwAlpha);
#endif

    // IPropertyStore
    IFACEMETHODIMP GetCount(DWORD *pcProps);
    IFACEMETHODIMP GetAt(DWORD iProp, PROPERTYKEY *pkey);
    IFACEMETHODIMP GetValue(REFPROPERTYKEY key, PROPVARIANT *pPropVar);
    IFACEMETHODIMP SetValue(REFPROPERTYKEY key, REFPROPVARIANT propVar);
    IFACEMETHODIMP Commit();

    // IPropertyStoreCapabilities
    IFACEMETHODIMP IsPropertyWritable(REFPROPERTYKEY key);

protected:
//    TCHAR		m_szFilename[MAX_PATH];   // Full path to the file in question.
	Thumb		*thumb;
	bool		processed;

	Stream		*stream;

	void		process(int task);
	int			taskFinished[0x10];
	CRITICAL_SECTION	creationCriticalSection; 
};

#ifdef THUMB_METHOD_IEXTRACTICON
#ifdef THUMB_METHOD_ITHUMBPROVIDER
#error You can not define both THUMB_METHOD_IEXTRACTICON and THUMB_METHOD_ITHUMBPROVIDER
#endif
#endif

#endif