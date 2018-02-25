#ifndef __COMMON_DEFINE__
#define __COMMON_DEFINE__

/// Four-byte alignment
#define PAD_SIZE(s) (((s)+3)&~3)

//for windows
#ifdef _WIN32 //windows相关

#define HTHREAD					HANDLE
#define THREAD_RET				unsigned long

typedef signed __int8			int8_t;
typedef signed __int16			int16_t;
typedef signed __int32			int32_t;
typedef signed __int64			int64_t;

typedef unsigned __int8			uint8_t;
typedef unsigned __int16		uint16_t;
typedef unsigned __int32		uint32_t;
typedef unsigned __int64		uint64_t;

typedef   long					LONG_PTR;
typedef   unsigned long			ULONG_PTR;
typedef   ULONG_PTR				DWORD_PTR;

#ifndef UINT64_MAX
#define UINT64_MAX				18446744073709551615UL
#endif

#ifndef INT64_MAX
#define INT64_MAX				9223372036854775807L
#endif

#define TEMP_DATA				0x7FF0000000000000L

#else //LINUX

typedef signed long long 		LONGLONG;

typedef unsigned int			HRESULT;
typedef int						INT;
typedef unsigned int			UINT;
typedef long					LONG;
typedef unsigned int			DWORD;
typedef int						BOOL;
typedef unsigned char			BYTE;
typedef unsigned short			WORD;
typedef float					FLOAT;
typedef struct sockaddr			SOCKADDR;
typedef void					void_t;
typedef char					char_t;
typedef int						bool_t;
typedef int						int_t;
typedef unsigned short			word_t;
typedef unsigned short			USHORT;

typedef unsigned long long		UINT64;

#define VIRTUAL					virtual
#define CALLBACK
#define WINAPI

#ifndef TRUE
#define TRUE					1
#endif
#ifndef FALSE
#define FALSE					0
#endif
#define MAX_PATH				256
#define INVALID_SOCKET			-1
#define INVALID_HANDLE_VALUE	0
#define SOCKET_ERROR			-1
#define VIRTUAL					virtual
#define HANDLE					void*
#define HINSTANCE				void*
#define HMODULE					void*
#define HMONITOR				void*
#define HWND					void*
#define HDATABUF				void*
#define PLONG					long*
#define LPVOID					void*
#define LPARAM					unsigned int*
#define LPDWORD					unsigned int*
#define HDC						void*
#define LPCVOID					const void*
#define VOID					void
#define	SOCKET					int
#define STATUS_WAIT_0           (0x00000000L)
#define WAIT_OBJECT_0			(STATUS_WAIT_0+0)
#define WAIT_OBJECT_1			(STATUS_WAIT_0+1)
#define WAIT_OBJECT_2			(STATUS_WAIT_0+2)
#define WAIT_TIMEOUT            (0x00000102)    

#define IGNORE					0       // Ignore signal
#define INFINITE				0xFFFFFFFF  // Infinite timeout

#define WSAEVENT				HANDLE
#define HTHREAD					pthread_t
#define THREAD_RET				void*
#define TCHAR					char
#define WAVE_FORMAT_PCM		1

#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3

#ifndef UINT64_MAX
#define UINT64_MAX				18446744073709551615ULL
#endif

#ifndef INT64_MAX
#define INT64_MAX				9223372036854775807LL
#endif


typedef unsigned int			COLORREF;
typedef char *					LPSTR;

#define TEMP_DATA				0x7FF0000000000000LL

typedef struct tagRECT
{
	int left;
	int top;
	int right;
	int bottom;
}RECT;

typedef struct  tagPOINT
{
	int x;
	int y;
}POINT;

typedef struct _OVERLAPPED
{
	unsigned int*	Internal;
	unsigned int*	InternalHigh;
	unsigned int	Offset;
	unsigned int	OffsetHigh;
	HANDLE			hEvent;
}OVERLAPPED, *LPOVERLAPPED;

typedef struct _SECURITY_ATTRIBUTES
{
	unsigned int	nLength;
	LPVOID			lpSecurityDescriptor;
	BOOL			bInheritHandle;
}SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#define FILE_ATTRIBUTE_READONLY             0x00000001
#define FILE_ATTRIBUTE_HIDDEN               0x00000002
#define FILE_ATTRIBUTE_SYSTEM               0x00000004
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020
#define FILE_ATTRIBUTE_DEVICE               0x00000040
#define FILE_ATTRIBUTE_NORMAL               0x00000080
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800
#define FILE_ATTRIBUTE_OFFLINE              0x00001000
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000

#define FILE_BEGIN							0
#define FILE_CURRENT						1
#define FILE_END							2

#define STD_INPUT_HANDLE					((uint32_t)-10)
#define STD_OUTPUT_HANDLE					((uint32_t)-11)
#define STD_ERROR_HANDLE					((uint32_t)-12)

typedef struct
{
	unsigned short		wFormatTag;
	unsigned short		nChannels;
	unsigned int		nSamplesPerSec;
	unsigned int		nAvgBytesPerSec;
	unsigned short		nBlockAlign;
}WAVEFORMAT, *LPWAVEFORMAT;

typedef struct
{
	unsigned short  wFormatTag; //音频格式MP3:0X55 ;mula : 0x0007
	unsigned short  nChannels;  //声道数
	unsigned int	nSamplesPerSec; //采样率:48000 ,8000.....
	unsigned int	nAvgBytesPerSec;
	unsigned short  nBlockAlign;
	unsigned short  wBitsPerSample;
	unsigned short  cbSize;
}WAVEFORMATEX, *LPWAVEFORMATEX;

typedef struct _SYSTEMTIME
{
	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDayOfWeek;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wMilliseconds;
	unsigned short wTimeZone;

}SYSTEMTIME, *PSYSTEMTIME;

typedef struct _FILETIME
{
	unsigned int dwLowDateTime;
	unsigned int dwHighDateTime;
}FILETIME, *PFILETIME;

typedef struct _WIN32_FIND_DATA
{
	unsigned int	dwFileAttributes;
	FILETIME		ftCreationTime;
	FILETIME		ftLastAccessTime;
	FILETIME		ftLastWriteTime;
	unsigned int	nFileSizeHigh;
	unsigned int	nFileSizeLow;
	unsigned int	dwReserved0;
	unsigned int	dwReserved1;
	TCHAR			cFileName[MAX_PATH];
	TCHAR			cAlternateFileName[14];
} WIN32_FIND_DATA, *PWIN32_FIND_DATA;

typedef struct tagRGBQUAD
{
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
}RGBQUAD;

typedef struct tagGUID
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} GUID;

typedef struct tagBITMAPINFOHEADER
{
	unsigned int	biSize;
	int				biWidth;
	int				biHeight;
	unsigned short  biPlanes;
	unsigned short  biBitCount;
	unsigned int	biCompression;
	unsigned int	biSizeImage;
	int				biXPelsPerMeter;
	int				biYPelsPerMeter;
	unsigned int	biClrUsed;
	unsigned int	biClrImportant;
}BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD             bmiColors[1];
}BITMAPINFO, *PBITMAPINFO;

typedef struct _SYSTEM_INFO
{
	union
	{
		unsigned int dwOemId;          // Obsolete field...do not use
		struct
		{
			unsigned short wProcessorArchitecture;
			unsigned short wReserved;
		};
	};
	unsigned int	dwPageSize;
	void*			lpMinimumApplicationAddress;
	void*			lpMaximumApplicationAddress;
	unsigned int	dwActiveProcessorMask;
	unsigned int	dwNumberOfProcessors;
	unsigned int	dwProcessorType;
	unsigned int	dwAllocationGranularity;
	unsigned short	wProcessorLevel;
	unsigned short	wProcessorRevision;
}SYSTEM_INFO, *LPSYSTEM_INFO;

#define BI_RGB				0L
#define BI_RLE8				1L
#define BI_RLE4				2L
#define BI_BITFIELDS		3L

#define DLL_PROCESS_ATTACH	1    
#define DLL_THREAD_ATTACH	2    
#define DLL_THREAD_DETACH	3    
#define DLL_PROCESS_DETACH	0   

typedef int(*FARPROC)();

#define MONITOR_DEFAULTTONEAREST	0 
#define MONITOR_DEFAULTTONULL		1
#define MONITOR_DEFAULTTOPRIMARY	2 

enum VARENUM
{
	VT_EMPTY = 0,
	VT_NULL = 1,
	VT_I2 = 2,
	VT_I4 = 3,
	VT_R4 = 4,
	VT_R8 = 5,
	VT_CY = 6,
	VT_DATE = 7,
	VT_BSTR = 8,
	VT_DISPATCH = 9,
	VT_ERROR = 10,
	VT_BOOL = 11,
	VT_VARIANT = 12,
	VT_UNKNOWN = 13,
	VT_DECIMAL = 14,
	VT_I1 = 16,
	VT_UI1 = 17,
	VT_UI2 = 18,
	VT_UI4 = 19,
	VT_I8 = 20,
	VT_UI8 = 21,
	VT_INT = 22,
	VT_UINT = 23,
	VT_VOID = 24,
	VT_HRESULT = 25,
	VT_PTR = 26,
	VT_SAFEARRAY = 27,
	VT_CARRAY = 28,
	VT_USERDEFINED = 29,
	VT_LPSTR = 30,
	VT_LPWSTR = 31,
	VT_RECORD = 36,
	VT_INT_PTR = 37,
	VT_UINT_PTR = 38,
	VT_FILETIME = 64,
	VT_BLOB = 65,
	VT_STREAM = 66,
	VT_STORAGE = 67,
	VT_STREAMED_OBJECT = 68,
	VT_STORED_OBJECT = 69,
	VT_BLOB_OBJECT = 70,
	VT_CF = 71,
	VT_CLSID = 72,
	VT_VERSIONED_STREAM = 73,
	VT_BSTR_BLOB = 0xfff,
	VT_VECTOR = 0x1000,
	VT_ARRAY = 0x2000,
	VT_BYREF = 0x4000,
	VT_RESERVED = 0x8000,
	VT_ILLEGAL = 0xffff,
	VT_ILLEGALMASKED = 0xfff,
	VT_TYPEMASK = 0xfff
};

#endif //WIN32

#ifndef ABS
#define  	ABS( a )	  ( ( ( a ) < 0 )? - ( a ):( a ) )
#endif
#ifndef MAX
#define  	MAX( a, b )   ( ( ( a ) > ( b ) )? ( a ):( b ) )
#endif
#ifndef MIN
#define  	MIN( a, b )   ( ( ( a ) < ( b ) )? ( a ):( b ) )
#endif

#define PI						3.1415926
#define ANGLE_TO_RADIAN(x)		((float)(x)*PI/180.0)
#define RADIAN_TO_ANGLE(x)		((float)(x)*180.0/PI)
#define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))

#define INVALID_PROCESSID		0xFFFFFFFF
#define INVALID_THREADID		0xFFFFFFFF

#endif //_TYPEDEF_H__
