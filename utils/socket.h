#ifndef __UTILS_SOCKET__
#define __UTILS_SOCKET__
#include <utils/config.h>

namespace UTILS
{
	enum PROTO_TYPE
	{
		PROTO_TYPE_NONE =0,
		PROTO_TYPE_TCP = 1,
		PROTO_TYPE_UDP =2
	};
	class UTILS_API AbstractSocket
	{
	public:
		AbstractSocket(PROTO_TYPE);
		/*
		/pref

			sock,如果套接字有效，直接内部设置为了异步socket
		*/
		AbstractSocket(SOCKET sock, PROTO_TYPE);
		virtual ~AbstractSocket() = 0;

		/// Close socket
		virtual void Close();
		virtual bool isOpen() const;
		virtual bool isError() const;
		virtual bool isConnect() const =0;

		static void InitEnv();
		static void UninitEnv();

		void SetPtotoType(PROTO_TYPE proto_type){
			_proto_type = proto_type; 
		}
		int SetNoDelay();
		void SetNoblock();
		int GetSockName(sockaddr*, int*) const;

		static int DuplicateSocket(int iSocket, DWORD pid, BYTE* pProtocolInfo, int iBuffLen, int* pDataLen);
		static int GetDuplicateSocket(BYTE* pProtocolInfo,int iDataLen, int* pSocket);

		int DuplicateSocket(DWORD pid,BYTE* pProtocolInfo, int iBuffLen, int* pDataLen);
		int GetDuplicateSocket(BYTE* pProtocolInfo,int iDataLen);

		int read(char* pBuff,int iBuffLen,const int ciReadLen = 0,unsigned int uiTimeOut = 5000);

		int read_from(char* pBuff,int iBuffLen,
		struct sockaddr *from,int *fromlen,const int ciReadLen = 0,unsigned int uiTimeOut = 5000);

		int write(const char* pBuff,int iBuffLen,unsigned int uiTimeOut = 5000);

		int write_to(const char* pBuff,int iBuffLen,
		struct sockaddr *to,int tolen,unsigned int uiTimeOut = 5000);
	protected:
		SOCKET _sock;
		int _err;
		PROTO_TYPE _proto_type;

	};

	class UTILS_API ClientSocket : public AbstractSocket
	{
	public:
		// ctor and dtor
		ClientSocket();
		ClientSocket(PROTO_TYPE);
		ClientSocket(SOCKET sock, PROTO_TYPE);
		ClientSocket(PROTO_TYPE, const char* ip, unsigned short port);
		virtual ~ClientSocket();

		bool UDPBind(const char* ip, unsigned short port);
		void Attach(SOCKET sock);
		bool isConnect() const;

		// methods
		bool Connect(const char* ip,unsigned short port,unsigned int uiTimeOut = 5000);
		
	private:
		bool _Connected;
	};

	class UTILS_API ServerSocket : public AbstractSocket
	{
	public:
		ServerSocket();
		ServerSocket(PROTO_TYPE);
		ServerSocket(PROTO_TYPE,unsigned short port,bool accept_block = true,const std::string& host = std::string());
		virtual ~ServerSocket();

		int Init(PROTO_TYPE,const char* ip, unsigned short port);
		void UnInit();

		bool Accept(ClientSocket&);
		SOCKET Accept();

		bool isConnect() const { return true; }
	};

	void UTILS_API GetSockAddr(PROTO_TYPE proto_type, const char* ip, unsigned short port, sockaddr* buff);
	SOCKET OpenSocket(PROTO_TYPE, const std::string& host,unsigned short port,int& error);

	SOCKET ConnectSocket(PROTO_TYPE,const std::string& hostn,
		unsigned short port,int& error,bool bNonBlock = true,unsigned int uiTimeOut = 5000);
	SOCKET AcceptSocket(SOCKET sock,int& error);
	int CloseSocket(SOCKET sock);
	int ShutdownSocket(SOCKET sock);

	long read(SOCKET sock,char* pBuff, int iBuffLen,int& error,bool recvfrom = false,
	struct sockaddr *from = nullptr, int *fromlen = nullptr,const int ciReadLen = 0,unsigned int uiTimeOut = 5000);

	long write(SOCKET sock,const char* pBuff, int iBuffLen,int& error,bool sendto = false,
	struct sockaddr *to = nullptr, int tolen = 0,unsigned int uiTimeOut = 5000);

	std::string getHostname(bool fqdn);
	int SetTCPNoDelay(SOCKET, bool, int& error);
	void SetAsyncSkt(SOCKET);
	int UTILS_API getPeerInfo(SOCKET skt, char* ip, int len, int* port);
	int UTILS_API getSockName(SOCKET skt, char* ip, int len, int* port);

	int getSockName(SOCKET skt, sockaddr*, int*);

	static inline int getLastSocketError(){
#ifdef WIN32	
		return WSAGetLastError();
#else
		return errno;
#endif
	}
}
#endif