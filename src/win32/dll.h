#ifndef __UTILS_DLL_WRAPPER__
#define __UTILS_DLL_WRAPPER__
#include "../../utils/api.h"
#include <pcre/include/pcre.h>

class CDll
{
public:
	CDll();
	CDll(const char* path);
	~CDll();

	/**
	* @brief 加载DLL文件
	*/
	virtual int Load(const char* path);
	/**
	* @brief 卸载
	*/
	void UnLoad();
	/**
	* @brief 是否记载DLL成功
	*/
	BOOL IsLoaded() const{ return m_bSuccess; }
	/**
	* @brief 所有的接口地址是否获取成功
	*/
	virtual BOOL IsProcAddress() = 0;
protected:
	/**<dll实例.*/
	HMODULE m_hModule;
	/**<是否加载成功.*/
	BOOL m_bSuccess;
	/**<DLL文件路径.*/
	char m_szPath[MAX_PATH];
};

class CDll_PCRE
	: public CDll
{
public:
	/**
	* @brief 实例被创建默认加载 pcre.dll
	*/
	CDll_PCRE();
	CDll_PCRE(const char* path);
	~CDll_PCRE();

public:
	typedef void*(*(*pfn_pcre_malloc))(size_t);
	typedef void(*(*pfn_pcre_free))(void *);
	typedef void*(*(*pfn_pcre_stack_malloc))(size_t);
	typedef void(*(*pfn_pcre_stack_free))(void *);
	typedef int(*(*pfn_pcre_callout))(pcre_callout_block *);
	typedef int(*(*pfn_pcre_stack_guard))(void);
	typedef pcre*(*pfn_pcre_compile)(const char *, int, const char **, int *, const unsigned char *);
	typedef pcre*(*pfn_pcre_compile2)(const char *, int, int *, const char **, int *, const unsigned char *);
	typedef int(*pfn_pcre_config)(int, void *);
	typedef int(*pfn_pcre_copy_named_substring)(const pcre *, const char *, int *, int, const char *, char *, int);
	typedef int(*pfn_pcre_copy_substring)(const char *, int *, int, int, char *, int);
	typedef int(*pfn_pcre_dfa_exec)(const pcre *, const pcre_extra *, const char *, int, int, int, int *, int, int *, int);
	typedef int(*pfn_pcre_exec)(const pcre *, const pcre_extra *, PCRE_SPTR, int, int, int, int *, int);
	typedef int(*pfn_pcre_jit_exec)(const pcre *, const pcre_extra *, PCRE_SPTR, int, int, int, int *, int, pcre_jit_stack *);
	typedef void(*pfn_pcre_free_substring)(const char *);
	typedef void(*pfn_pcre_free_substring_list)(const char **);
	typedef int(*pfn_pcre_fullinfo)(const pcre *, const pcre_extra *, int, void *);
	typedef int(*pfn_pcre_get_named_substring)(const pcre *, const char *,
		int *, int, const char *, const char **);
	typedef int(*pfn_pcre_get_stringnumber)(const pcre *, const char *);
	typedef int(*pfn_pcre_get_stringtable_entries)(const pcre *, const char *,char **, char **);
	typedef int(*pfn_pcre_get_substring)(const char *, int *, int, int,const char **);
	typedef int(*pfn_pcre_get_substring_list)(const char *, int *, int,const char ***);
	typedef const unsigned char *(*pfn_pcre_maketables)(void);
	typedef int(*pfn_pcre_refcount)(pcre *, int);
	typedef pcre_extra *(*pfn_pcre_study)(const pcre *, int, const char **);
	typedef void(*pfn_pcre_free_study)(pcre_extra *);
	typedef const char *(*pfn_pcre_version)(void);
	typedef int(*pfn_pcre_pattern_to_host_byte_order)(pcre *, pcre_extra *,const unsigned char *);
	typedef pcre_jit_stack *(*pfn_pcre_jit_stack_alloc)(int, int);
	typedef void(*pfn_pcre_jit_stack_free)(pcre_jit_stack *);
	typedef void(*pfn_pcre_assign_jit_stack)(pcre_extra *,pcre_jit_callback, void *);
	typedef void(*pfn_pcre_jit_free_unused_memory)(void);

	/**
	* @brief 加载DLL文件
	*/
	virtual int Load(const char* path);
	BOOL IsProcAddress();
	/**
	* @brief 获取函数地址
	*/
	pfn_pcre_malloc pfnMalloc() { return m_pfn_pcre_malloc; }
	pfn_pcre_free pfnFree(){ return m_pfn_pcre_free; }
	pfn_pcre_stack_malloc pfnStackMalloc(){ return m_pfn_pcre_stack_malloc; }
	pfn_pcre_stack_free pfnStackFree(){ return  m_pfn_pcre_stack_free; }
	pfn_pcre_callout pfnCallOut(){ return m_pfn_pcre_callout; }
	pfn_pcre_stack_guard pfnStackGuard(){ return m_pfn_pcre_stack_guard; }
	pfn_pcre_compile pfnCompile(){ return m_pfn_pcre_compile; }
	pfn_pcre_compile2 pfnCompile2(){ return m_pfn_pcre_compile2; }
	pfn_pcre_config pfnConfig(){ return m_pfn_pcre_config; }
	pfn_pcre_copy_named_substring pfnCopyNamedSubstring(){ return m_pfn_pcre_copy_named_substring; }
	pfn_pcre_copy_substring pfnCopySubstring(){ return m_pfn_pcre_copy_substring; }
	pfn_pcre_dfa_exec pfnDfaExec(){ return m_pfn_pcre_dfa_exec; }
	pfn_pcre_exec pfnExec(){ return  m_pfn_pcre_exec; }
	pfn_pcre_jit_exec pfnJitExec(){ return m_pfn_pcre_jit_exec; }
	pfn_pcre_free_substring pfnFreeSubstring(){ return m_pfn_pcre_free_substring; }
	pfn_pcre_free_substring_list pfnFreeSubstringList(){ return m_pfn_pcre_free_substring_list; }
	pfn_pcre_fullinfo pfnFullInfo(){ return m_pfn_pcre_fullinfo; }
	pfn_pcre_get_named_substring pfnGetNamedSubstring(){ return m_pfn_pcre_get_named_substring; }
	pfn_pcre_get_stringnumber pfnGetStringnumber(){ return m_pfn_pcre_get_stringnumber; }
	pfn_pcre_get_stringtable_entries pfnGetStringtableEntries(){ return m_pfn_pcre_get_stringtable_entries; }
	pfn_pcre_get_substring pfnGetSubstring(){ return m_pfn_pcre_get_substring; }
	pfn_pcre_get_substring_list pfnGetSubstringList(){ return m_pfn_pcre_get_substring_list; }
	pfn_pcre_maketables pfnMaketables(){ return m_pfn_pcre_maketables; }
	pfn_pcre_refcount pfnRefCount(){ return m_pfn_pcre_refcount; }
	pfn_pcre_study pfnStudy(){ return m_pfn_pcre_study; }
	pfn_pcre_free_study pfnFreeStudy(){ return m_pfn_pcre_free_study; }
	pfn_pcre_version pfnVersion(){ return m_pfn_pcre_version; }
	pfn_pcre_pattern_to_host_byte_order pfnPatternToHostByteOrder(){ return m_pfn_pcre_pattern_to_host_byte_order; }
	pfn_pcre_jit_stack_alloc pfnJitStackAlloc(){ return m_pfn_pcre_jit_stack_alloc; }
	pfn_pcre_jit_stack_free pfnJitStackFree(){ return m_pfn_pcre_jit_stack_free; }
	pfn_pcre_assign_jit_stack pfnAssignJitStack(){ return m_pfn_pcre_assign_jit_stack; }
	pfn_pcre_jit_free_unused_memory pfnJitFreeUnusedMemory(){ return m_pfn_pcre_jit_free_unused_memory; }
private:
	pfn_pcre_malloc m_pfn_pcre_malloc;
	pfn_pcre_free m_pfn_pcre_free;
	pfn_pcre_stack_malloc m_pfn_pcre_stack_malloc;
	pfn_pcre_stack_free m_pfn_pcre_stack_free;
	pfn_pcre_callout m_pfn_pcre_callout;
	pfn_pcre_stack_guard m_pfn_pcre_stack_guard;
	pfn_pcre_compile m_pfn_pcre_compile;
	pfn_pcre_compile2 m_pfn_pcre_compile2;
	pfn_pcre_config m_pfn_pcre_config;
	pfn_pcre_copy_named_substring m_pfn_pcre_copy_named_substring;
	pfn_pcre_copy_substring m_pfn_pcre_copy_substring;
	pfn_pcre_dfa_exec m_pfn_pcre_dfa_exec;
	pfn_pcre_exec m_pfn_pcre_exec;
	pfn_pcre_jit_exec m_pfn_pcre_jit_exec;
	pfn_pcre_free_substring m_pfn_pcre_free_substring;
	pfn_pcre_free_substring_list m_pfn_pcre_free_substring_list;
	pfn_pcre_fullinfo m_pfn_pcre_fullinfo;
	pfn_pcre_get_named_substring m_pfn_pcre_get_named_substring;
	pfn_pcre_get_stringnumber m_pfn_pcre_get_stringnumber;
	pfn_pcre_get_stringtable_entries m_pfn_pcre_get_stringtable_entries;
	pfn_pcre_get_substring m_pfn_pcre_get_substring;
	pfn_pcre_get_substring_list m_pfn_pcre_get_substring_list;
	pfn_pcre_maketables m_pfn_pcre_maketables;
	pfn_pcre_refcount m_pfn_pcre_refcount;
	pfn_pcre_study m_pfn_pcre_study;
	pfn_pcre_free_study m_pfn_pcre_free_study;
	pfn_pcre_version m_pfn_pcre_version;
	pfn_pcre_pattern_to_host_byte_order m_pfn_pcre_pattern_to_host_byte_order;
	pfn_pcre_jit_stack_alloc m_pfn_pcre_jit_stack_alloc;
	pfn_pcre_jit_stack_free m_pfn_pcre_jit_stack_free;
	pfn_pcre_assign_jit_stack m_pfn_pcre_assign_jit_stack;
	pfn_pcre_jit_free_unused_memory m_pfn_pcre_jit_free_unused_memory;

private:
	BOOL GetInterface(const char*, void*&);
	/**<接口地址是否加载成功.*/
	BOOL m_bProcAddressState;
};

#endif	//__CRITSEC_H__
