#ifndef __UTILS_INTERNAL__
#define __UTILS_INTERNAL__
#include "../utils/dll.h"


#include <libiconv\include\iconv.h>
//类定义  
DLLIMPORTCLASSBEGIN(__libiconv, "libiconv.dll")
FUNCTIONENTRY(iconv_t(*libiconv_open)(const char* tocode, const char* fromcode), libiconv_open)
FUNCTIONENTRY(size_t(*libiconv)(iconv_t cd, char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft), libiconv)
FUNCTIONENTRY(size_t(*libiconv_close)(iconv_t cd), libiconv_close)
DLLIMPORTCLASSEND()

#ifdef UTILS_ENABLE_REGEDIT
#include <regedit\RegEdit.h>
//类定义  
DLLIMPORTCLASSBEGIN(__regedit, "RegEdit.dll")
FUNCTIONENTRY(HANDLE(*RegEdit_Init)(HKEY hKey, LPCTSTR lpszRegistryKey), RegEdit_Init)
FUNCTIONENTRY(int(*RegEdit_GetProfileString)(HANDLE hReg, LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault), RegEdit_GetProfileString)
FUNCTIONENTRY(int(*RegEdit_GetProfileInt)(HANDLE hReg, LPCTSTR lpszSection, LPCTSTR lpszEntry, UINT nDefault), RegEdit_GetProfileInt)
FUNCTIONENTRY(int(*RegEdit_UInit)(HANDLE hReg), RegEdit_UInit)
DLLIMPORTCLASSEND()
#endif

BOOL libiconvLoadSucc();
BOOL RegEditLoadSucc();
#endif	//__CRITSEC_H__
