#include "../utils/api.h"


namespace UTILS {namespace API {

	void* Malloc(int size, bool init /*= false*/){
		void* buf = NULL;
		if (size > 0){
			buf = malloc(size);
			if (init && buf != NULL){
				memset(buf, 0, size);
			}
		}
		return buf;
	}

	void Free(void* buf){
		if (buf != NULL){
			free(buf);
		}
	}

	void* Memcpy(void* dest, const void* src, int count){
		return memcpy(dest, src, count);
	}

	void* Memset(void* dest, int c, int count, int bCatch /*=0*/){
		return memset(dest, c, count);
	}

	void* Memmove(void* dest, const void* src, int count){
		return memmove(dest, src, count);
	}

	int Memcmp(const void* dest, const void* src, int count){
		return memcmp(dest, src, count);
	}
	

	int Sprintf(char* buffer, int size, const char* format, ...) {
		if ((buffer == NULL) || (size <= 0)){
			return -1;
		}
		va_list va;
		int iRet = 0;
		va_start(va, format);
#ifdef _WIN32
		iRet = _vsnprintf_s(buffer, size, size-1, format, va);
#else
		iRet = vsnprintf(buffer, count, format, argptr);
#endif
		va_end(va);
		buffer[min(iRet, size-1)] = '\0';
		return iRet;
	}

	int Strcpy(char* _Destination, char const* _Source, int _MaxCount) {
		if ((_Destination == nullptr) || (_Source == nullptr)) {
			return -1;
		}
		int iRet = 0;
#ifdef _WIN32
		iRet = strncpy_s(_Destination, _TRUNCATE, _Source, _MaxCount);
#else
		iRet = strncpy(_Destination, _Source, _MaxCount);
#endif
		return iRet;
	}
}}