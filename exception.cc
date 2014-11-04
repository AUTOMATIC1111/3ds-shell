#include "exception.h"

int sanityEnabled=1;

GeneralException::GeneralException(String cause){
	info=cause;
}

SanityException::SanityException(String cause) :GeneralException(cause){
	
}
