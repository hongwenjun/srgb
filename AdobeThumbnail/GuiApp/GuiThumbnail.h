#ifndef GUITHUMBNAIL_H_INCLUDED
#define GUITHUMBNAIL_H_INCLUDED

#include "../atpch.h"


extern "C" {
#define UNICODE 1
#include "Everything.h"
int EVERYTHINGAPI Everything_GetResultFullPathNameA(int nIndex,LPSTR buf,int bufsize);
int EVERYTHINGAPI Everything_GetResultFullPathNameW(int nIndex,LPWSTR wbuf,int wbuf_size_in_wchars);
}



#endif // GUITHUMBNAIL_H_INCLUDED
