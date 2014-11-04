#ifndef __EXCEPTION__H__
#define __EXCEPTION__H__

#include "ShellUtils.h"

extern int sanityEnabled;

#define SANITY_CHECK(condition,error) do{\
	if(! ( condition ))\
		throw SanityException(String(_T("Sanity check failed: "))+error+_T(" at ")+_T(__FILE__)+_T(",")+_T(QUOTE(__LINE__))); \
} while(0)

#define SANITY_LIMIT(variable,max,title) do{\
	SANITY_CHECK(( variable ) <= ( max ),_T(title)+format(_T(" too large: %d (max %d)"),variable,max));\
	SANITY_CHECK(( variable ) >= ( 0 ),_T(title)+format(_T(" is negative: %d"),variable));\
} while(0)

#define SANITY_LIMIT_BYTES(variable,max,title) do{\
	SANITY_CHECK(( variable ) <= ( max ),_T(title)+format(_T(" too large: %s (max %s)"),cstr(filesize(variable)),cstr(filesize(max))));\
	SANITY_CHECK(( variable ) >= ( 0 ),_T(title)+format(_T(" is negative: %d"),variable));\
} while(0)

class GeneralException{
public:
	GeneralException(String cause);
	String info;
};

class SanityException :public GeneralException{
public:
	SanityException(String cause);
};

#endif
