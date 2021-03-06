#ifndef __UTILS_INTERNAL__
#define __UTILS_INTERNAL__
#include "../utils/dll.h"
#include "../utils/api.h"

#include <libiconv\include\iconv.h>
//�ඨ��  
DLLIMPORTCLASSBEGIN(__libiconv, "libiconv.dll")
FUNCTIONENTRY(iconv_t(*libiconv_open)(const char* tocode, const char* fromcode), libiconv_open)
FUNCTIONENTRY(size_t(*libiconv)(iconv_t cd, char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft), libiconv)
FUNCTIONENTRY(size_t(*libiconv_close)(iconv_t cd), libiconv_close)
DLLIMPORTCLASSEND()

BOOL libiconvLoadSucc();

#define MSG_INFO(...) UTILS::API::DEBUG_INFO2("utils", __FILE__, __LINE__, __VA_ARGS__);
#define MSG_ERROR MSG_INFO("ERROR")
#endif	//__CRITSEC_H__
