#ifndef __SHELL__UTILS__
#define __SHELL__UTILS__

#include <windows.h>
#include <string>


namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
};

typedef std::tstring String;
#define cstr(a) ((TCHAR *)((a).c_str()))

std::tstring format(const TCHAR *fmt,va_list ap);
std::tstring format(const TCHAR *fmt, ...);
std::string formatA(const char *fmt, ...);

std::tstring format(const TCHAR *fmt, ...);

#include "exstream.h"

#define QUOTEQUOTE(a) #a
#define QUOTE(a) QUOTEQUOTE(a)

std::wstring s2ws(const std::string& s);
std::string ws2s(const std::wstring& s);
std::wstring cs2ws(const char *s);
std::string cws2s(const TCHAR *s);
std::wstring js2ws(const std::string& s);
std::string ws2js(const std::wstring& s);
std::wstring jcs2ws(const char *s);
std::string cws2js(const TCHAR *s);

std::tstring lcfirst(const std::tstring& s);
std::tstring ucfirst(const std::tstring& s);

extern HRESULT (*PropVariantToStringDLL)(REFPROPVARIANT propvar, PWSTR psz, UINT cch);
extern HRESULT (*PSRegisterPropertySchemaDLL)(PCWSTR pszPath);
extern HRESULT (*PSUnregisterPropertySchemaDLL)(PCWSTR pszPath);

std::tstring filesize(size_t size);

void list(TCHAR *path, void (*handler)(TCHAR *filename));

#endif
