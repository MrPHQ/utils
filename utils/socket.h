#ifndef __UTILS_SOCKET__
#define __UTILS_SOCKET__
#include <utils/config.h>

namespace UTILS
{
	class UTILS_API AbstractSocket
	{
	public:
		AbstractSocket();
		/*
		/pref

			sock,如果套接字有效，直接内部设置为了异步socket
		*/
		AbstractSocket(SOCKET sock);
		virtual ~AbstractSocket() = 0;

		/// Close socket
		virtual void Close();
		virtual bool isOpen() const;
		virtual bool isError() const;

		static void InitEnv();
		static void UninitEnv();

		int SetNoDelay();
		void SetNoblock();

		static int DuplicateSocket(int iSocket, DWORD pid, 
			BYTE* pProtocolInfo, int iBuffLen, int* pDataLen);
		static int GetDuplicateSocket(BYTE* pProtocolInfo,
			int iDataLen, int* pSocket);

		int DuplicateSocket(DWORD pid,BYTE* pProtocolInfo, int iBuffLen, int* pDataLen);
		int GetDuplicateSocket(BYTE* pProtocolInfo,int iDataLen);


	protected:
		SOCKET _sock;
		int _err;
	};

	class UTILS_API ClientSocket : public AbstractSocket
	{
	public:
		// ctor and dtor
		ClientSocket();
		ClientSocket(SOCKET sock);
		ClientSocket(const char* ip,unsigned short port);
		virtual ~ClientSocket();

		void Attach(SOCKET sock);
		bool isConnect() const;

		// methods
		bool Connect(const char* ip,
			unsigned short port,
			unsigned int uiTimeOut = 5000);
		int read(char* pBuff, int iBuffLen,
			const int ciReadLen = 0,
			unsigned int uiTimeOut = 5000);
		int write(const char* pBuff, int iBuffLen,
			unsigned int uiTimeOut = 5000);

	private:
		bool _Connected;
	};

	class UTILS_API ServerSocket : public AbstractSocket
	{
	public:
		ServerSocket();
		ServerSocket(unsigned short port,
			bool accept_block = true,
			const std::string& host = std::string());
		virtual ~ServerSocket();

		int Init(const char* ip, unsigned short port);
		void UnInit();

		bool Accept(ClientSocket&);
		SOCKET Accept();
	};

	SOCKET OpenSocket(const std::string& host,
		unsigned short port,
		int& error);

	SOCKET ConnectSocket(const std::string& hostn,
		unsigned short port,
		int& error,
		bool bNonBlock = true,
		unsigned int uiTimeOut = 5000);
	SOCKET AcceptSocket(SOCKET sock,int& error);
	int CloseSocket(SOCKET sock);
	int ShutdownSocket(SOCKET sock);

	long read(SOCKET sock,
		char* pBuff, int iBuffLen,
		int& error,
		const int ciReadLen = 0,
		unsigned int uiTimeOut = 5000);
	long write(SOCKET sock,
		const char* pBuff, int iBuffLen,
		int& error,
		unsigned int uiTimeOut = 5000);

	std::string getHostname(bool fqdn);
	int SetTCPNoDelay(SOCKET, bool, int& error);
	void SetAsyncSkt(SOCKET);
	int getPeerInfo(SOCKET skt, char* ip, int len, int* port);

	static inline int getLastSocketError()
	{
#ifdef WIN32	
		return WSAGetLastError();
#else
		return errno;
#endif
	}
}
#endif