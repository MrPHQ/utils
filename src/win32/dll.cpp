#include "dll.h"
#include "../internal.h"
CDll::CDll() 
: m_hModule(NULL)
{
	memset(m_szPath, 0, sizeof(m_szPath));
	m_bSuccess = FALSE;
}

CDll::CDll(const char* path)
{
	memset(m_szPath, 0, sizeof(m_szPath));
	if (NULL != path)
	{
		_snprintf_s(m_szPath, _TRUNCATE, "%s", path);
	}
	m_bSuccess = FALSE;
	m_hModule = NULL;
	Load(NULL);
}

CDll::~CDll()
{
}

int CDll::Load(const char* path)
{
	if (NULL != path)
	{
		_snprintf_s(m_szPath, _TRUNCATE, "%s", path);
	}
	if (strlen(m_szPath) <= 0)
	{
		return -1;
	}
	m_hModule = LoadLibraryEx(m_szPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	(m_hModule == NULL) ? m_bSuccess = FALSE : m_bSuccess = TRUE;
	return m_bSuccess ? 0 : -2;
}

void CDll::UnLoad()
{
	if (NULL != m_hModule)
	{
		FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
	m_bSuccess = FALSE;
}

CDll_PCRE::CDll_PCRE()
:CDll()
{
	m_bProcAddressState = FALSE;
	m_pfn_pcre_malloc = NULL;
	m_pfn_pcre_free = NULL;
	m_pfn_pcre_stack_malloc = NULL;
	m_pfn_pcre_stack_free = NULL;
	m_pfn_pcre_callout = NULL;
	m_pfn_pcre_stack_guard = NULL;
	m_pfn_pcre_compile = NULL;
	m_pfn_pcre_compile2 = NULL;
	m_pfn_pcre_config = NULL;
	m_pfn_pcre_copy_named_substring = NULL;
	m_pfn_pcre_copy_substring = NULL;
	m_pfn_pcre_dfa_exec = NULL;
	m_pfn_pcre_exec = NULL;
	m_pfn_pcre_jit_exec = NULL;
	m_pfn_pcre_free_substring = NULL;
	m_pfn_pcre_free_substring_list = NULL;
	m_pfn_pcre_fullinfo = NULL;
	m_pfn_pcre_get_named_substring = NULL;
	m_pfn_pcre_get_stringnumber = NULL;
	m_pfn_pcre_get_stringtable_entries = NULL;
	m_pfn_pcre_get_substring = NULL;
	m_pfn_pcre_get_substring_list = NULL;
	m_pfn_pcre_maketables = NULL;
	m_pfn_pcre_refcount = NULL;
	m_pfn_pcre_study = NULL;
	m_pfn_pcre_free_study = NULL;
	m_pfn_pcre_version = NULL;
	m_pfn_pcre_pattern_to_host_byte_order = NULL;
	m_pfn_pcre_jit_stack_alloc = NULL;
	m_pfn_pcre_jit_stack_free = NULL;
	m_pfn_pcre_assign_jit_stack = NULL;
	m_pfn_pcre_jit_free_unused_memory = NULL;
	Load("pcre.dll");
}

CDll_PCRE::CDll_PCRE(const char* path)
:CDll(path)
{
	m_bProcAddressState = FALSE;
	m_pfn_pcre_malloc = NULL;
	m_pfn_pcre_free = NULL;
	m_pfn_pcre_stack_malloc = NULL;
	m_pfn_pcre_stack_free = NULL;
	m_pfn_pcre_callout = NULL;
	m_pfn_pcre_stack_guard = NULL;
	m_pfn_pcre_compile = NULL;
	m_pfn_pcre_compile2 = NULL;
	m_pfn_pcre_config = NULL;
	m_pfn_pcre_copy_named_substring = NULL;
	m_pfn_pcre_copy_substring = NULL;
	m_pfn_pcre_dfa_exec = NULL;
	m_pfn_pcre_exec = NULL;
	m_pfn_pcre_jit_exec = NULL;
	m_pfn_pcre_free_substring = NULL;
	m_pfn_pcre_free_substring_list = NULL;
	m_pfn_pcre_fullinfo = NULL;
	m_pfn_pcre_get_named_substring = NULL;
	m_pfn_pcre_get_stringnumber = NULL;
	m_pfn_pcre_get_stringtable_entries = NULL;
	m_pfn_pcre_get_substring = NULL;
	m_pfn_pcre_get_substring_list = NULL;
	m_pfn_pcre_maketables = NULL;
	m_pfn_pcre_refcount = NULL;
	m_pfn_pcre_study = NULL;
	m_pfn_pcre_free_study = NULL;
	m_pfn_pcre_version = NULL;
	m_pfn_pcre_pattern_to_host_byte_order = NULL;
	m_pfn_pcre_jit_stack_alloc = NULL;
	m_pfn_pcre_jit_stack_free = NULL;
	m_pfn_pcre_assign_jit_stack = NULL;
	m_pfn_pcre_jit_free_unused_memory = NULL;
	Load(path);
}

CDll_PCRE::~CDll_PCRE()
{

}

int CDll_PCRE::Load(const char* path)
{
	if (IsLoaded())
	{
		return 0;
	}
	int ret = CDll::Load(path);
	if (ret != 0)
	{
		return -1;
	}
	int iFailCnt = 0;
	iFailCnt += (!GetInterface("pcre_malloc", (void*&)m_pfn_pcre_malloc) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_free", (void*&)m_pfn_pcre_free) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_stack_malloc", (void*&)m_pfn_pcre_stack_malloc) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_stack_free", (void*&)m_pfn_pcre_stack_free) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_callout", (void*&)m_pfn_pcre_callout) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_stack_guard", (void*&)m_pfn_pcre_stack_guard) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_compile", (void*&)m_pfn_pcre_compile) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_compile2", (void*&)m_pfn_pcre_compile2) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_config", (void*&)m_pfn_pcre_config) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_copy_named_substring", (void*&)m_pfn_pcre_copy_named_substring) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_copy_substring", (void*&)m_pfn_pcre_copy_substring) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_dfa_exec", (void*&)m_pfn_pcre_dfa_exec) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_exec", (void*&)m_pfn_pcre_exec) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_free_substring", (void*&)m_pfn_pcre_free_substring) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_free_substring_list", (void*&)m_pfn_pcre_free_substring_list) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_fullinfo", (void*&)m_pfn_pcre_fullinfo) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_get_named_substring", (void*&)m_pfn_pcre_get_named_substring) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_get_stringnumber", (void*&)m_pfn_pcre_get_stringnumber) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_get_stringtable_entries", (void*&)m_pfn_pcre_get_stringtable_entries) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_get_substring", (void*&)m_pfn_pcre_get_substring) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_get_substring_list", (void*&)m_pfn_pcre_get_substring_list) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_maketables", (void*&)m_pfn_pcre_maketables) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_refcount", (void*&)m_pfn_pcre_refcount) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_study", (void*&)m_pfn_pcre_study) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_free_study", (void*&)m_pfn_pcre_free_study) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_version", (void*&)m_pfn_pcre_version) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_pattern_to_host_byte_order", (void*&)m_pfn_pcre_pattern_to_host_byte_order) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_jit_stack_alloc", (void*&)m_pfn_pcre_jit_stack_alloc) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_jit_stack_free", (void*&)m_pfn_pcre_jit_stack_free) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_assign_jit_stack", (void*&)m_pfn_pcre_assign_jit_stack) ? 1 : 0);
	iFailCnt += (!GetInterface("pcre_jit_free_unused_memory", (void*&)m_pfn_pcre_jit_free_unused_memory) ? 1 : 0);
	if (iFailCnt > 0)
	{
		m_bProcAddressState = FALSE;
		MSG_INFO("PCRE DLL GetProcAddress Fail");
	}
	else{
		m_bProcAddressState = TRUE;
	}
	return m_bProcAddressState ? 0 : -2;
}

BOOL CDll_PCRE::IsProcAddress()
{
	return m_bProcAddressState;
}

BOOL CDll_PCRE::GetInterface(const char* name, void*& pAddr)
{
	if (NULL == name)
	{
		return FALSE;
	}
	if (!IsLoaded())
	{
		Load("pcre.dll");
	}
	if (!IsLoaded())
	{
		return FALSE;
	}
	pAddr = GetProcAddress(m_hModule, name);
	return NULL != pAddr;
}
