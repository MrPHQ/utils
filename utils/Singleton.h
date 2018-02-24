#ifndef __UTILS_SINGLETON__
#define __UTILS_SINGLETON__
#include <cassert>

namespace UTILS{
template <typename T> 
class SingleTon
{
protected:
	static T* ms_Singleton;

public:
	SingleTon(void){
		assert(!ms_Singleton);
		ms_Singleton = static_cast<T*>(this);
	}
	~SingleTon(void){
		assert(ms_Singleton);  ms_Singleton = 0;
	}
	static T& getSingleton(void){
		assert(ms_Singleton);  return (*ms_Singleton);
	}
	static T* getSingletonPtr(void){
		return (ms_Singleton);
	}

private:
	SingleTon& operator=(const SingleTon&) { return *this; }
	SingleTon(const SingleTon&) {}
};
}
// use 
// CX: public SingleTon<CX>
// template<> CX* SingleTon<CX>::ms_Singleton = 0;

#endif	// end
