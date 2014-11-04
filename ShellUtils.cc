#include "ShellUtils.h"

std::tstring format(const TCHAR *fmt,va_list ap){
    int size = 100;
    std::tstring str;

    while (1) {
        str.resize(size);

        int n = _vsntprintf((TCHAR *)str.c_str(), size, fmt, ap);

        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }

        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
    return str;
}

std::tstring format(const TCHAR *fmt, ...) {
    va_list ap;

	va_start(ap, fmt);
 	std::tstring res=format(fmt,ap);
	va_end(ap);
	
	return res;
}
std::string formatA(const char *fmt, ...){
    int size = 100;
    std::string str;
    va_list ap;

    while (1) {
        str.resize(size);

        va_start(ap, fmt);
        int n = _vsnprintf((char *)str.c_str(), size, fmt, ap);
        va_end(ap);

        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }

        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
    return str;
}


std::wstring s2ws(const std::string& s){
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, 0, 0); 
    std::wstring r(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, &r[0], len);
	r.resize(len-1);
    return r;
}

std::string ws2s(const std::wstring& s){
    int len;
    int slength = (int)s.length() + 1;
    len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), slength, 0, 0, 0, 0); 
    std::string r(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, s.c_str(), slength, &r[0], len, 0, 0); 
 	r.resize(len-1);
	return r;
}
std::wstring cs2ws(const char *s){
    int len;
    int slength = (int)strlen(s) + 1;
    len = MultiByteToWideChar(CP_UTF8, 0, s, slength, 0, 0); 
    std::wstring r(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s, slength, &r[0], len);
 	r.resize(len-1);
    return r;
}

std::string cws2s(const TCHAR *s){
    int len;
    int slength = (int)_tcslen(s) + 1;
    len = WideCharToMultiByte(CP_UTF8, 0, s, slength, 0, 0, 0, 0); 
    std::string r(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, s, slength, &r[0], len, 0, 0); 
 	r.resize(len-1);
    return r;
}

std::wstring js2ws(const std::string& s){
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(932, 0, s.c_str(), slength, 0, 0); 
    std::wstring r(len, L'\0');
    MultiByteToWideChar(932, 0, s.c_str(), slength, &r[0], len);
	r.resize(len-1);
    return r;
}

std::string ws2js(const std::wstring& s){
    int len;
    int slength = (int)s.length() + 1;
    len = WideCharToMultiByte(932, 0, s.c_str(), slength, 0, 0, 0, 0); 
    std::string r(len, '\0');
    WideCharToMultiByte(932, 0, s.c_str(), slength, &r[0], len, 0, 0); 
 	r.resize(len-1);
	return r;
}

std::wstring jcs2ws(const char *s){
    int len;
    int slength = (int)strlen(s) + 1;
    len = MultiByteToWideChar(932, 0, s, slength, 0, 0); 
    std::wstring r(len, L'\0');
    MultiByteToWideChar(932, 0, s, slength, &r[0], len);
 	r.resize(len-1);
    return r;

}
std::string cws2js(const TCHAR *s){
    int len;
    int slength = (int)_tcslen(s) + 1;
    len = WideCharToMultiByte(932, 0, s, slength, 0, 0, 0, 0); 
    std::string r(len, '\0');
    WideCharToMultiByte(932, 0, s, slength, &r[0], len, 0, 0); 
 	r.resize(len-1);
    return r;
}


std::tstring lcfirst(const std::tstring& s){
	std::tstring res=std::tstring(_T(""))+(TCHAR)tolower(s.at(0))+s.substr(1,999999);

	return res;
}

std::tstring ucfirst(const std::tstring& s){
	std::tstring res=std::tstring(_T(""))+(TCHAR)toupper(s.at(0))+s.substr(1,999999);

	return res;
}

std::tstring filesize(size_t size){
	std::tstring res;
	double s=(double)size;

	if(s<1024) return format(_T("%.0fb"),s); s/=1024;
	if(s<1024) return format(_T("%.1fkB"),s); s/=1024;
	if(s<1024) return format(_T("%.2fMb"),s); s/=1024;
	if(s<1024) return format(_T("%.2fGb"),s); s/=1024;
	if(s<1024) return format(_T("%.2fTb"),s);
	
	return _T("HUGE!");
}

void list(TCHAR *path, void (*handler)(TCHAR *filename)){
    WIN32_FIND_DATA fdFile; 
    HANDLE hFind = NULL; 

    TCHAR sPath[4096]; 

   _sntprintf(sPath,4096, _T("%s\\*.*"), path); 

    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
        return; 

    do{ 
        if(_tcscmp(fdFile.cFileName, _T(".")) == 0) continue;
        if(_tcscmp(fdFile.cFileName, _T("..")) == 0) continue;

        _sntprintf(sPath,4096,_T("%s\\%s"), path, fdFile.cFileName); 

		if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { 
			list(sPath,handler);
		} else{ 
			handler(sPath);
        }
    } 
    while(FindNextFile(hFind, &fdFile));

    FindClose(hFind);
}
