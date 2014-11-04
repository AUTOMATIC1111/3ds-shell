

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Tue Nov 04 02:05:31 2014
 */
/* Compiler settings for ShellPreview.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ShellPreview_h_h__
#define __ShellPreview_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IShellPreviewExt_FWD_DEFINED__
#define __IShellPreviewExt_FWD_DEFINED__
typedef interface IShellPreviewExt IShellPreviewExt;
#endif 	/* __IShellPreviewExt_FWD_DEFINED__ */


#ifndef __ShellPreviewExt_FWD_DEFINED__
#define __ShellPreviewExt_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellPreviewExt ShellPreviewExt;
#else
typedef struct ShellPreviewExt ShellPreviewExt;
#endif /* __cplusplus */

#endif 	/* __ShellPreviewExt_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "propsys.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IShellPreviewExt_INTERFACE_DEFINED__
#define __IShellPreviewExt_INTERFACE_DEFINED__

/* interface IShellPreviewExt */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IShellPreviewExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D3BCFB6B-17A5-42A6-AA8C-5D5E0479C0B1")
    IShellPreviewExt : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IShellPreviewExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellPreviewExt * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellPreviewExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellPreviewExt * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellPreviewExt * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellPreviewExt * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellPreviewExt * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellPreviewExt * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IShellPreviewExtVtbl;

    interface IShellPreviewExt
    {
        CONST_VTBL struct IShellPreviewExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellPreviewExt_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IShellPreviewExt_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IShellPreviewExt_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IShellPreviewExt_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IShellPreviewExt_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IShellPreviewExt_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IShellPreviewExt_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IShellPreviewExt_INTERFACE_DEFINED__ */



#ifndef __FILEICONSLib_LIBRARY_DEFINED__
#define __FILEICONSLib_LIBRARY_DEFINED__

/* library FILEICONSLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_FILEICONSLib;

EXTERN_C const CLSID CLSID_ShellPreviewExt;

#ifdef __cplusplus

class DECLSPEC_UUID("FC548317-B187-4404-B7EC-FA9EE87EFE84")
ShellPreviewExt;
#endif
#endif /* __FILEICONSLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


