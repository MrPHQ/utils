#ifndef __UTILS_PIPE__2
#define __UTILS_PIPE__
#include <utils/config.h>

namespace UTILS
{
	class UTILS_API AbstractPipe
	{
	public:
		AbstractPipe();
		AbstractPipe(HANDLE);
		virtual ~AbstractPipe() = 0;

		/// Close socket
		virtual void Close();
		virtual bool isOpen() const;
		virtual bool isError() const;


	protected:
		HANDLE _pipe;
		int _err;
		char _pipe_name[256];
	};

	class UTILS_API ClientPipe : public AbstractPipe
	{
	public:
		// ctor and dtor
		ClientPipe();
		ClientPipe(HANDLE pipe);
		ClientPipe(const char* ip,unsigned short port);
		virtual ~ClientPipe();

		void Attach(HANDLE pipe);
		bool isConnect() const;

		// methods
		bool Connect(const char* pipe_name,unsigned int uiTimeOut = 5000);
		/// ×èÈû..
		int read(char* pBuff, int iBuffLen,const int ciReadLen = 0);
		int write(const char* pBuff, int iBuffLen);

	private:
		bool _Connected;
	};

	class UTILS_API ServerPipe : public AbstractPipe
	{
	public:
		ServerPipe();
		ServerPipe(const char* pipe_name);
		virtual ~ServerPipe();

		int Init(const char* pipe_name);
		void UnInit();

		bool Accept(const char*, ClientPipe&);
		HANDLE Accept(const char*);
	};

	HANDLE AcceptPipe(const char*,int& error,
		DWORD dwOpenMode = PIPE_ACCESS_DUPLEX,
		DWORD dwPipeMode= PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE |PIPE_WAIT,
		DWORD nOutBufferSize =1024*4,
		DWORD nInBufferSize = 1024 * 4);
}
#endif