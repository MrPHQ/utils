#ifndef __UTILS_DLL__
#define __UTILS_DLL__
#include <utils/utils.h>
namespace UTILS {

#ifdef _WIN32
#define DLLIMPORTCLASSBEGIN(CLASS,DLLPATH) class CLASS; \
extern CLASS g##CLASS;                             \
class CLASS                                             \
{                                                       \
protected:                                              \
    HMODULE m_hModule;                                  \
    BOOL bSuccess;                                   \
public:                                                 \
    CLASS(): m_hModule(NULL)                            \
    {                                                   \
        m_hModule = LoadLibraryEx(DLLPATH, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);               \
        m_hModule == NULL ? bSuccess=FALSE:bSuccess=TRUE;   \
    }                                                   \
    ~CLASS()                                            \
    {                                                   \
        if(m_hModule)                                   \
            FreeLibrary(m_hModule);                     \
    }                                                   \

#define FUNCTIONENTRY(ENTRYTYPE,ENTRYPOINT) public:     \
    typedef ENTRYTYPE;                                  \
    ENTRYPOINT ENTRYPOINT##Ptr;                         \
public:                                                 \
    ENTRYPOINT ENTRYPOINT##Func(){                      \
		if(!bSuccess) return NULL;                       \
        ENTRYPOINT##Ptr =                               \
    (ENTRYPOINT)GetProcAddress(m_hModule,#ENTRYPOINT);  \
        return ENTRYPOINT##Ptr;                         \
    }                                                   \

#define DLLIMPORTCLASSEND() };

#else
#endif
}
#define DLLIMPORTCLASSIMPLEMENT(CLASS) CLASS g##CLASS; 
#define DLLENTRYEXIST(CLASS,ENTRYPOINT) (g##CLASS.ENTRYPOINT##Func() == NULL?FALSE:TRUE)
#define DLLIMPORTCALL(CLASS,ENTRYPOINT) g##CLASS.ENTRYPOINT##Ptr

#endif